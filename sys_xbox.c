#ifdef _XBOX
#include <xtl.h>
#include "direct.h"
#endif
#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "quakedef.h"

qboolean	isDedicated;

int noconinput = 0;

char *basedir = ".";
char *cachedir = "T:\\";

int		modmenu_argc = 0;
char	**modmenu_argv = NULL;

cvar_t  sys_linerefresh = {"sys_linerefresh","0"}; // set for entity display
cvar_t  sys_nostdout = {"sys_nostdout","0"};

// =======================================================================
// General routines
// =======================================================================

void Sys_DebugNumber(int y, int val)
{
}

void Sys_Printf (char *fmt, ...)
{
	va_list		argptr;
	char		text[1024];
	
	va_start (argptr,fmt);
	vsprintf (text,fmt,argptr);
	va_end (argptr);
	fprintf(stderr, "%s", text);
	
	//Con_Print (text);
}

void Sys_Quit (void)
{
	LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
	
	Host_Shutdown();	
	XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
}

void Sys_Init(void)
{
#ifdef _XBOX // MARTY - 128mb XBox machines needs this to fucntion correctly, taken from XBMC
	MEMORYSTATUS	lpBuffer;
	lpBuffer.dwLength = sizeof( MEMORYSTATUS );

	GlobalMemoryStatus( &lpBuffer );

	// Set MTRRDefType memory type to write-back as done in other XBox apps - seems a bit of a hack as really the def type
	// should be uncachable and the mtrr/mask for ram instead set up for 128MB with writeback as is done in cromwell.
	if( lpBuffer.dwTotalPhys > 67108864 ) // Check if > 64mb
	{
		__asm
		{
			mov ecx, 0x2ff
			rdmsr
			mov al, 0x06
			wrmsr
		}
	}
#endif //_XBOX

#if id386
	Sys_SetFPCW();
#endif
}

#if !id386

/*
================
Sys_LowFPPrecision
================
*/
void Sys_LowFPPrecision (void)
{
// causes weird problems on Nextstep
}

/*
================
Sys_HighFPPrecision
================
*/
void Sys_HighFPPrecision (void)
{
// causes weird problems on Nextstep
}

#endif	// !id386

void Sys_Log(char* LogInfo)
{
   FILE * fp;

   fp = fopen ("D:\\DebugLog.txt", "a+");
 
   fprintf(fp, "%s %s %s %s", "[8:50:00]", " [12800] ", LogInfo, "\n");
   
   fclose(fp);
}

void Sys_Error (char *error, ...)
{ 
    va_list     argptr;
    char        string[1024];

    va_start (argptr,error);
    vsprintf (string,error,argptr);
    va_end (argptr);
	fprintf(stderr, "Error: %s\n", string);

	Host_Shutdown ();
	exit (1);
} 

void Sys_Warn (char *warning, ...)
{ 
    va_list     argptr;
    char        string[1024];
    
    va_start (argptr,warning);
    vsprintf (string,warning,argptr);
    va_end (argptr);
	fprintf(stderr, "Warning: %s", string);
} 

/*
===============================================================================

FILE IO

===============================================================================
*/

#define	MAX_HANDLES		10
FILE	*sys_handles[MAX_HANDLES];

int		findhandle (void)
{
	int		i;
	
	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}

/*
================
Qfilelength
================
*/
static int Qfilelength (FILE *f)
{
	int		pos;
	int		end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead (char *path, int *hndl)
{
	FILE	*f;
	int		i;
	
	i = findhandle ();

	f = fopen(path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;
	
	return Qfilelength(f);
}

int Sys_FileOpenWrite (char *path)
{
	FILE	*f;
	int		i;
	
	i = findhandle ();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));
	sys_handles[i] = f;
	
	return i;
}

void Sys_FileClose (int handle)
{
	if ( handle >= 0 ) 
	{
		fclose (sys_handles[handle]);
		sys_handles[handle] = NULL;
	}
}

void Sys_FileSeek (int handle, int position)
{
	if ( handle >= 0 ) 
	{
		fseek (sys_handles[handle], position, SEEK_SET);
	}
}

int Sys_FileRead (int handle, void *dst, int count)
{
	char *data;
	int size, done;

	size = 0;
	if ( handle >= 0 ) 
	{
		data = dst;
		while ( count > 0 ) 
		{
			done = fread (data, 1, count, sys_handles[handle]);
			if ( done == 0 ) 
			{
				break;
			}
			data += done;
			count -= done;
			size += done;
		}
	}
	return size;
}

int Sys_FileWrite (int handle, void *src, int count)
{
	char *data;
	int size, done;

	size = 0;
	if ( handle >= 0 ) 
	{
		data = src;
		while ( count > 0 ) 
		{
			done = fread (data, 1, count, sys_handles[handle]);
			if ( done == 0 ) 
			{
				break;
			}
			data += done;
			count -= done;
			size += done;
		}
	}
	return size;
}

int	Sys_FileTime (char *path)
{
	FILE	*f;
	
	f = fopen(path, "rb");
	if (f)
	{
		fclose(f);
		return 1;
	}
	
	return -1;
}

void Sys_mkdir (char *path)
{
    mkdir (path);
}

void Sys_DebugLog(char *file, char *fmt, ...)
{
    va_list argptr; 
    static char data[1024];
    FILE *fp;
    
    va_start(argptr, fmt);
    vsprintf(data, fmt, argptr);
    va_end(argptr);
    fp = fopen(file, "a");
    fwrite(data, strlen(data), 1, fp);
    fclose(fp);
}

double Sys_FloatTime (void)
{
	static int starttime = 0;

	if ( ! starttime )
		starttime = clock();

	return (clock()-starttime)*1.0/1024;
}

// =======================================================================
// Sleeps for microseconds
// =======================================================================

static volatile int oktogo;

void alarm_handler(int x)
{
	oktogo=1;
}

byte *Sys_ZoneBase (int *size)
{

	char *QUAKEOPT = NULL;

	*size = 0xc00000;
	if (QUAKEOPT)
	{
		while (*QUAKEOPT)
			if (tolower(*QUAKEOPT++) == 'm')
			{
				*size = atof(QUAKEOPT) * 1024*1024;
				break;
			}
	}
	return malloc (*size);

}

void Sys_LineRefresh(void)
{
}

void Sys_Sleep(void)
{
	Sleep(1);
}

void floating_point_exception_handler(int whatever)
{
#ifndef _XBOX
	Sys_Warn("floating point exception\n");
#endif
	signal(SIGFPE, floating_point_exception_handler);
}

void moncontrol(int x)
{
}
 
int main (int c, char **v)
{
	FILE *fp = NULL;

	char commandLine[1024];
	double		time, oldtime, newtime;
	quakeparms_t parms;
	extern int vcrFile;
	extern int recording;
	static int frame;

	int		n, d = 0, slcl;

	moncontrol(0);

	fp = fopen("T:\\quake.params", "rb");
	if (fp)
	{
		fread(commandLine, 1024, 1, fp);
		fclose(fp);
 
		// Count the arguments
		modmenu_argc = 1;
		for(n = 0; n < strlen(commandLine); n++)
		{
			if (commandLine[n] == ' ')
				 modmenu_argc++;
		}

		// Set up modmenu_argv
		modmenu_argv = (char **)malloc(sizeof(char **) * modmenu_argc);
		modmenu_argv[0] = commandLine;
		d = 1;
		slcl = strlen(commandLine);
		for(n = 0; n < slcl; n++)
		{
			if(commandLine[n] == ' ')
			{
				commandLine[n] = 0;
				modmenu_argv[d++] = commandLine + n + 1;
			}
		}
	}
  
	parms.memsize = 16*1024*1024;
	parms.membase = malloc (parms.memsize);
	parms.basedir = basedir;
	parms.cachedir = cachedir;

	COM_InitArgv(modmenu_argc, modmenu_argv);
	parms.argc = modmenu_argc;
	parms.argv = modmenu_argv;
 
	Sys_Init();

    Host_Init(&parms);

	Cvar_RegisterVariable (&sys_nostdout);

    oldtime = Sys_FloatTime () - 0.1;
    while (1)
    {
		// find time spent rendering last frame
        newtime = Sys_FloatTime ();
        time = newtime - oldtime;

        if (cls.state == ca_dedicated)
        {   // play vcrfiles at max speed
            if (time < sys_ticrate.value && (vcrFile == -1 || recording) )
            {
                Sleep(1);
                continue;       // not time to run a server only tic yet
            }
            time = sys_ticrate.value;
        }

        if (time > sys_ticrate.value*2)
            oldtime = newtime;
        else
            oldtime += time;

        if (++frame > 10)
            moncontrol(1);      // profile only while we do each Quake frame
        Host_Frame (time);
        moncontrol(0);

		// graphic debugging aids
        if (sys_linerefresh.value)
            Sys_LineRefresh ();
    }
}

/*
================
Sys_MakeCodeWriteable
================
*/
void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{

	DWORD  flOldProtect;

	if (!VirtualProtect((LPVOID)startaddr, length, PAGE_READWRITE, &flOldProtect))
   		Sys_Error("Protection change failed\n");
}

 