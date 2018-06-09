/*
	cd_sdl.c
	$Id: cd_sdl.c,v 1.7 2006/03/24 15:45:22 sezero Exp $

	Copyright (C) 2001  Mark Baker <homer1@together.net>
	Taken from SDLquake with modifications to make it work
	with Linux Hexen II: Hammer of Thyrion

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA
*/

#ifdef _USE_SDL_CDROM

#ifndef H_SDL_CDROM
#define H_SDL_CDROM

#include "quakedef.h"
#include "sdl.h"
#include "sdl_mixer.h"

static qboolean cdValid = false;
static qboolean	initialized = false;
static qboolean	enabled = true;
static qboolean playLooping = false;

Mix_Music *current_music = NULL;

static void CDAudio_Eject(void)
{
}

void music_done()
{
	Mix_FreeMusic(current_music);
	current_music = NULL;
}

void CDAudio_Play(byte track, qboolean looping)
{
	char track_name[32];	
	int fade_msec = 500;

	looping *= -1;

	sprintf(track_name, "D:\\music\\track%02d.ogg", track); //MARTY

	current_music = Mix_LoadMUS(track_name);         

	if (current_music) 
	{
		if (Mix_PlayingMusic())
		{
			// Fade-in effect sounds cleaner if we're already playing something
			Mix_FadeInMusic(current_music, looping, fade_msec);
		}
		else
		{
			Mix_PlayMusic(current_music, looping);
		}

		Mix_HookMusicFinished(music_done);
		printf("Now playing: %s\n", track_name);
	}
	else 
	{
		printf("Music %s could not be loaded\n", track_name);
		return;
	}

	playLooping = looping;
}


void CDAudio_Stop(void)
{
	Mix_HaltMusic();
}

void CDAudio_Pause(void)
{
	Mix_PauseMusic();
}


void CDAudio_Resume(void)
{
	Mix_ResumeMusic();
}

static void CD_f (void)
{
	char	*command;

	if (Cmd_Argc() < 2)
	{
		Con_Printf("commands:");
		Con_Printf("on, off, play, stop, \n");
		Con_Printf("loop, pause, resume\n");
		Con_Printf("eject, close, info\n");
		return;
	}

	command = Cmd_Argv (1);

	if (Q_strcasecmp(command, "on") == 0)
	{
		enabled = true;
		return;
	}

	if (Q_strcasecmp(command, "off") == 0)
	{
		return;
	}

	if (Q_strcasecmp(command, "play") == 0)
	{
		CDAudio_Play((byte)atoi(Cmd_Argv (2)), false);
		return;
	}

	if (Q_strcasecmp(command, "loop") == 0)
	{
		CDAudio_Play((byte)atoi(Cmd_Argv (2)), true);
		return;
	}

	if (Q_strcasecmp(command, "stop") == 0)
	{
		CDAudio_Stop();
		return;
	}

	if (Q_strcasecmp(command, "pause") == 0)
	{
		CDAudio_Pause();
		return;
	}

	if (Q_strcasecmp(command, "resume") == 0)
	{
		CDAudio_Resume();
		return;
	}

	if (Q_strcasecmp(command, "eject") == 0)
	{
		CDAudio_Eject();
		cdValid = false;
		return;
	}

	if (Q_strcasecmp(command, "info") == 0)
	{
		return;
	}
}

void CDAudio_Update(void)
{
}

int CDAudio_Init(void)
{
	initialized = true;
	enabled = true;
	cdValid = true;

	Con_Printf("CDAudio initialized (using SDL)\n");

	Cmd_AddCommand ("cd", CD_f);

	return 0;
}

void CDAudio_Shutdown(void)
{
}

#endif //H_SDL_CDROM
#endif //_USE_SDL_CDROM