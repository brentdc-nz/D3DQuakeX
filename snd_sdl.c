#include <stdio.h>
#include "sdl.h"
#include "quakedef.h"
#include "sdl_mixer.h"

static dma_t the_shm;
static int snd_inited;

extern int desired_speed;
extern int desired_bits;
   
static void paint_audio(void *unused, Uint8 *stream, int len)
{
	if ( shm ) {
		shm->buffer = stream;
		shm->samplepos += len/(shm->samplebits/8)/2;
		// Check for samplepos overflow?
		S_PaintChannels (shm->samplepos);
	}
}

qboolean SNDDMA_Init(void)
{
	SDL_AudioSpec desired, obtained;
	char	drivername[128];

	snd_inited = 0;

	if (SDL_InitSubSystem (SDL_INIT_AUDIO) < 0)
	{
		Con_Printf("Couldn't init SDL audio: %s\n", SDL_GetError());
		return 0;
	}

	/* Set up the desired format */
	desired.freq = desired_speed;
	switch (desired_bits)
	{
		case 8:
			desired.format = AUDIO_U8;
			break;
		case 16:
			if ( SDL_BYTEORDER == SDL_BIG_ENDIAN )
				desired.format = AUDIO_S16MSB;
			else
				desired.format = AUDIO_S16LSB;
			break;
	}

	desired.channels = 2;
	desired.samples  = 1024; // previously 512 S.A.
	desired.callback = paint_audio;
	desired.userdata = NULL;

	/* Open the audio device */
	if (Mix_OpenAudio(desired.freq, desired.format, desired.channels, desired.samples) == - 1){
		printf("La cagamos");
		return 0;
	}

#if 0
	if ( SDL_OpenAudio(&desired, &obtained) < 0 )
	{
		Con_Printf("Couldn't open SDL audio: %s\n", SDL_GetError());
		return 0;
	}
#endif

	/* Make sure we can support the audio format */
	switch (desired.format)
	{
		case AUDIO_U8:
			/* Supported */
			break;
		case AUDIO_S16LSB:
		case AUDIO_S16MSB:
			if ( ((desired.format == AUDIO_S16LSB) && (SDL_BYTEORDER == SDL_LIL_ENDIAN)) ||
				((desired.format == AUDIO_S16MSB) && (SDL_BYTEORDER == SDL_BIG_ENDIAN)) )
				break;	/* Supported */
			else
				Con_Printf ("Warning: sound format / endianness mismatch\n");
		default:
			/* Not supported -- force SDL to do our bidding */
			Con_Printf ("Warning: unsupported audio format received\n");
			Con_Printf ("Warning: will try forcing sdl audio\n");
			SDL_CloseAudio();
			if ( SDL_OpenAudio(&desired, NULL) < 0 )
			{
				Con_Printf("Couldn't open SDL audio: %s\n", SDL_GetError());
				return 0;
			}
			memcpy(&obtained, &desired, sizeof(desired));
			break;
	}

	/* Fill the audio DMA information block */
	shm = &sn;
	shm->splitbuffer = 0;
	shm->samplebits = (desired.format & 0xFF); // first byte of format is bits
//	if (obtained.freq != desired_speed)
//		Con_Printf ("Warning: Rate set (%i) didn't match requested rate (%i)!\n", obtained.freq, desired_speed);
	shm->speed = desired.freq;
	shm->channels = desired.channels;
	shm->samples = desired.samples*shm->channels;
	shm->samplepos = 0;
	shm->submission_chunk = 1;

	shm->buffer = NULL;

	Mix_SetPostMix(desired.callback, NULL);

	if (SDL_AudioDriverName(drivername, sizeof (drivername)) == NULL)
		strcpy(drivername, "(UNKNOWN)");

	snd_inited = 1;
	SDL_PauseAudio(0);

	Con_Printf("Audio Subsystem initialized in SDL mode.\n");
	Con_Printf ("SDL audio driver: %s\n", drivername);
	Con_Printf ("%5d stereo\n", shm->channels - 1);
	Con_Printf ("%5d samples\n", shm->samples);
	Con_Printf ("%5d samplepos\n", shm->samplepos);
	Con_Printf ("%5d samplebits\n", shm->samplebits);
	Con_Printf ("%5d submission_chunk\n", shm->submission_chunk);
	Con_Printf ("%5d speed\n", shm->speed);
	Con_Printf ("0x%x dma buffer address\n", (int) shm->buffer);
	Con_Printf ("%5d total_channels\n", total_channels);
	return 1;
}

int SNDDMA_GetDMAPos(void)
{
	return shm->samplepos;
}

void SNDDMA_Shutdown(void)
{
	if (snd_inited)
	{
		Con_Printf ("Shutting down SDL sound\n");
		snd_inited = 0;
//		SDL_PauseAudio (1);
		SDL_CloseAudio();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}
}

void SNDDMA_Submit(void)
{
}