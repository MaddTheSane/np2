
typedef struct {
	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;
	BYTE	F12KEY;
	BYTE	resume;

#if !defined(GX_DLL)
	int		winx;
	int		winy;
#endif
#if defined(WIN32_PLATFORM_PSPC)
	BYTE	bindcur;
	BYTE	bindbtn;
#endif
} NP2OSCFG;


#if defined(SIZE_QVGA)
#if defined(SUPPORT_SOFTKBD)
enum {
	FULLSCREEN_WIDTH	= 320,
	FULLSCREEN_HEIGHT	= 240
};
#else
enum {
	FULLSCREEN_WIDTH	= 320,
	FULLSCREEN_HEIGHT	= 200
};
#endif
#else
enum {
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 400
};
#endif

enum {
	WM_NP2CMD			= (WM_USER + 200)
};

enum {
	NP2CMD_EXIT			= 0,
	NP2CMD_RESET		= 1,
	NP2CMD_EXIT2		= 0x0100,
	NP2CMD_DUMMY		= 0xffff
};


#ifdef __cplusplus
extern "C" {
#endif
extern	NP2OSCFG	np2oscfg;
#ifdef __cplusplus
}
#endif
extern	HWND		hWndMain;
extern	HINSTANCE	hInst;
extern	HINSTANCE	hPrev;
extern	char		modulefile[MAX_PATH];

