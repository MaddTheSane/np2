
#ifndef __cplusplus
#error why called from C ?
#endif

typedef struct {
	int		winx;
	int		winy;
	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;

	BYTE	KEYBOARD;
	BYTE	F12COPY;

	BYTE	MOUSE_SW;
	BYTE	JOYPAD1;
	BYTE	JOYPAD2;
	BYTE	JOY1BTN[4];

	BYTE	I286SAVE;
} NP2OSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};


enum {
	IDM_MEMORYDUMP		= 20000,

	WM_NP2CMD			= (WM_USER + 200)
};

enum {
	NP2CMD_EXIT			= 0,
	NP2CMD_RESET		= 1,
	NP2CMD_EXIT2		= 0x0100,
	NP2CMD_DUMMY		= 0xffff
};

extern	const char	szAppCaption[];
extern	NP2OSCFG	np2oscfg;
extern	HWND		hWndMain;
extern	HINSTANCE	hInst;
extern	HINSTANCE	hPrev;
extern	char		modulefile[MAX_PATH];
extern	char		fddfolder[MAX_PATH];
extern	char		hddfolder[MAX_PATH];
extern	char		bmpfilefolder[MAX_PATH];
extern	char		mimpideffile[MAX_PATH];

