
typedef struct {
	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;
	BYTE	DISPCLK;
	BYTE	F12COPY;
	BYTE	MOUSE_SW;
    
	BYTE	comfirm;
	BYTE	resume;													// ver0.30
	//BYTE	statsave;
    
	BYTE	I286SAVE;
} NP2OSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};


extern	WindowPtr	hWndMain;
extern	BOOL		np2running;
extern	NP2OSCFG	np2oscfg;

void toggleMenubar(void);