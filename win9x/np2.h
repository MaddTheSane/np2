
typedef struct {
	BYTE	port;
	BYTE	def_en;
	BYTE	param;
	UINT32	speed;
	char	mout[MAXPNAMELEN];
	char	min[MAXPNAMELEN];
	char	mdl[64];
	char	def[MAX_PATH];
} COMCFG;

typedef struct {
	char	titles[256];
	char	winid[4];

	int		winx;
	int		winy;
	UINT	paddingx;
	UINT	paddingy;
	BYTE	force400;
	BYTE	WINSNAP;
	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;

	BYTE	background;
	BYTE	DISPCLK;

	BYTE	KEYBOARD;
	BYTE	F12COPY;

	BYTE	MOUSE_SW;
	BYTE	JOYPAD1;
	BYTE	JOYPAD2;
	BYTE	JOY1BTN[4];

	COMCFG	mpu;
	COMCFG	com1;
	COMCFG	com2;
	COMCFG	com3;

	UINT32	clk_color1;
	UINT32	clk_color2;
	BYTE	clk_x;
	BYTE	clk_fnt;

	BYTE	comfirm;
	BYTE	shortcut;												// ver0.30

	BYTE	sstp;
	UINT16	sstpport;												// ver0.30

	BYTE	resume;													// ver0.30
	BYTE	statsave;
	BYTE	disablemmx;
	BYTE	toolwin;
	BYTE	keydisp;
	BYTE	I286SAVE;
	BYTE	hostdrv_write;
} NP2OSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};

enum {
	NP2BREAK_MAIN		= 0x01,
	NP2BREAK_DEBUG		= 0x02
};

enum {
	IDM_TOOLWIN			= 20000,
	IDM_KEYDISP			= 20001,
	IDM_SCREENCENTER	= 20002,
	IDM_SNAPENABLE		= 20003,
	IDM_BACKGROUND		= 20004,
	IDM_BGSOUND			= 20005,
	IDM_TRACEONOFF		= 20006,
	IDM_MEMORYDUMP		= 20007,
	IDM_DEBUGUTY		= 20008,
	IDM_VIEWER			= 20009,

	IDM_FLAGSAVE		= 20100,
	IDM_FLAGLOAD		= 20150,

	IDM_SCRNMUL			= 20200,					// ver0.26
	IDM_SCRNMUL4		= (IDM_SCRNMUL + 4),
	IDM_SCRNMUL6		= (IDM_SCRNMUL + 6),
	IDM_SCRNMUL8		= (IDM_SCRNMUL + 8),
	IDM_SCRNMUL10		= (IDM_SCRNMUL + 10),
	IDM_SCRNMUL12		= (IDM_SCRNMUL + 12),
	IDM_SCRNMUL16		= (IDM_SCRNMUL + 16),

	WM_NP2CMD			= (WM_USER + 200),
	WM_SSTP				= (WM_USER + 201)
};

enum {
	NP2CMD_EXIT			= 0,
	NP2CMD_RESET		= 1,
	NP2CMD_EXIT2		= 0x0100,
	NP2CMD_DUMMY		= 0xffff
};

enum {
	MMXFLAG_DISABLE		= 1,
	MMXFLAG_NOTSUPPORT	= 2
};


extern	NP2OSCFG	np2oscfg;
extern	HWND		hWndMain;
extern	HINSTANCE	hInst;
extern	HINSTANCE	hPrev;
extern	int			mmxflag;
extern	BYTE		np2break;
extern	char		modulefile[MAX_PATH];
extern	char		fddfolder[MAX_PATH];
extern	char		hddfolder[MAX_PATH];
extern	char		bmpfilefolder[MAX_PATH];

void np2active_renewal(void);

