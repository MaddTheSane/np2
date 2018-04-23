
typedef struct {
	UINT8	port;
	UINT8	def_en;
	UINT8	param;
	UINT32	speed;
	char	mout[MAX_PATH];
	char	min[MAX_PATH];
	char	mdl[64];
	char	def[MAX_PATH];
} COMCFG;

typedef struct {
	char	titles[256];
	
	int		winx;
	int		winy;
	UINT8	NOWAIT;
	UINT8	DRAW_SKIP;
	UINT8	DISPCLK;
	UINT8	F11KEY;
	UINT8	F12KEY;
    
	UINT8	MOUSE_SW;
	UINT8	JOYPAD1;
    
	COMCFG	mpu;

	UINT8	comfirm;
	UINT8	resume;
    
	UINT8	toolwin;
#if defined(SUPPORT_KEYDISP)
	UINT8	keydisp;
#endif
#if defined(SUPPORT_SOFTKBD)
	UINT8	softkey;
#endif
	UINT8	jastsnd;
	UINT8	I286SAVE;
	
} NP2OSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};


//extern	WindowPtr	hWndMain;
extern	BOOL		np2running;
extern	NP2OSCFG	np2oscfg;
extern	UINT8		scrnmode;
