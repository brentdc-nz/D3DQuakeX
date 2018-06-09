#ifndef H_SYSXBOXINPUT
#define H_SYSXBOXINPUT

//
// these are the key numbers that should be passed to Key_Event
//
#define	K_TAB			9
#define	K_ENTER			13
#define	K_ESCAPE		27
#define	K_SPACE			32

// normal keys should be passed as lowercased ascii

#define	K_BACKSPACE		127
#define	K_UPARROW		128
#define	K_DOWNARROW		129
#define	K_LEFTARROW		130
#define	K_RIGHTARROW	131

#define	K_ALT			132
#define	K_CTRL			133
#define	K_SHIFT			134
#define	K_F1			135
#define	K_F2			136
#define	K_F3			137
#define	K_F4			138
#define	K_F5			139
#define	K_F6			140
#define	K_F7			141
#define	K_F8			142
#define	K_F9			143
#define	K_F10			144
#define	K_F11			145
#define	K_F12			146
#define	K_INS			147
#define	K_DEL			148
#define	K_PGDN			149
#define	K_PGUP			150
#define	K_HOME			151
#define	K_END			152

#define K_PAUSE			255

//
// mouse buttons generate virtual keys
//
#define	K_MOUSE1		200
#define	K_MOUSE2		201
#define	K_MOUSE3		202

//
// joystick buttons
//
#define	K_JOY1			203
#define	K_JOY2			204
#define	K_JOY3			205
#define	K_JOY4			206
#define	K_AUX1			207
//
// aux keys are for multi-buttoned joysticks to generate so they can use
// the normal binding process
//
#define K_XBOX_A		207
#define K_XBOX_B		208
#define K_XBOX_X		209
#define K_XBOX_Y		210
#define K_XBOX_BACK		211
#define K_XBOX_START	212
#define K_DPAD_UP		213
#define K_DPAD_DOWN		214
#define K_DPAD_LEFT		215
#define K_DPAD_RIGHT	216
#define K_XBOX_LTRIG	217
#define K_XBOX_RTRIG	218
#define K_XBOX_QCOMBO	219
#define	K_AUX13			220
#define	K_AUX14			221
#define	K_AUX15			222
#define	K_AUX16			223
#define	K_AUX17			224
#define	K_AUX18			225
#define	K_AUX19			226
#define	K_AUX20			227
#define	K_AUX21			228
#define	K_AUX22			229
#define	K_AUX23			230
#define	K_AUX24			231
#define	K_AUX25			232
#define	K_AUX26			233
#define	K_AUX27			234
#define	K_AUX28			235
#define	K_AUX29			236
#define	K_AUX30			237
#define	K_AUX31			238
#define	K_AUX32			239

// JACK: Intellimouse(c) Mouse Wheel Support

#define K_MWHEELUP		239
#define K_MWHEELDOWN	240

//============================================

#ifdef __cplusplus
extern "C" 
{
#endif
		 
typedef struct XBGamepadInfo
{
	//Buttons
	int iKey;
	int iButtonDown;

	//Thumbsticks
	float fThumbLX;
	float fThumbLY;
	float fThumbRX;
	float fThumbRY;
} sXBGamepadInfo;

int UpdateGamepad(sXBGamepadInfo *pGamepadInfo);

#ifdef __cplusplus
}
#endif

#endif