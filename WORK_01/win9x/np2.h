
typedef struct {
	UINT8	port;
	UINT8	def_en;
	UINT8	param;
	UINT32	speed;
	OEMCHAR	mout[MAXPNAMELEN];
	OEMCHAR	min[MAXPNAMELEN];
	OEMCHAR	mdl[64];
	OEMCHAR	def[MAX_PATH];
} COMCFG;

typedef struct {
	OEMCHAR	titles[256];
	OEMCHAR	winid[4];

	int		winx;
	int		winy;
	UINT	paddingx;
	UINT	paddingy;
	UINT8	force400;
	UINT8	WINSNAP;
	UINT8	NOWAIT;
	UINT8	DRAW_SKIP;

	UINT8	background;
	UINT8	DISPCLK;
	UINT8	KEYBOARD;
	UINT8	F12COPY;

	UINT8	MOUSE_SW;
	UINT8	JOYPAD1;
	UINT8	JOYPAD2;
	UINT8	JOY1BTN[4];

	COMCFG	mpu;
	COMCFG	com1;
	COMCFG	com2;
	COMCFG	com3;

	UINT32	clk_color1;
	UINT32	clk_color2;
	UINT8	clk_x;
	UINT8	clk_fnt;

	UINT8	comfirm;
	UINT8	shortcut;												// ver0.30

	UINT8	sstp;
	UINT16	sstpport;												// ver0.30

	UINT8	resume;													// ver0.30
	UINT8	statsave;
	UINT8	disablemmx;
	UINT8	wintype;
	UINT8	toolwin;
	UINT8	keydisp;
	UINT8	I286SAVE;
	UINT8	hostdrv_write;
	UINT8	jastsnd;
	UINT8	useromeo;
	UINT8	thickframe;
	UINT8	xrollkey;
	UINT8	fscrnbpp;
} NP2OSCFG;


enum {
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
	IDM_SOFTKBD			= 20002,
	IDM_MEMDBG32		= 20003,
	IDM_SCREENCENTER	= 20004,
	IDM_SNAPENABLE		= 20005,
	IDM_BACKGROUND		= 20006,
	IDM_BGSOUND			= 20007,
	IDM_TRACEONOFF		= 20008,
	IDM_MEMORYDUMP		= 20009,
	IDM_DEBUGUTY		= 20010,
	IDM_VIEWER			= 20011,

	IDM_SCRNMUL			= 20050,
	IDM_SCRNMUL4		= (IDM_SCRNMUL + 4),
	IDM_SCRNMUL6		= (IDM_SCRNMUL + 6),
	IDM_SCRNMUL8		= (IDM_SCRNMUL + 8),
	IDM_SCRNMUL10		= (IDM_SCRNMUL + 10),
	IDM_SCRNMUL12		= (IDM_SCRNMUL + 12),
	IDM_SCRNMUL16		= (IDM_SCRNMUL + 16),

	IDM_FLAGSAVE		= 20100,
	IDM_FLAGLOAD		= 20150,

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
extern	UINT8		np2break;
extern	BOOL		winui_en;

extern	OEMCHAR		modulefile[MAX_PATH];
extern	OEMCHAR		fddfolder[MAX_PATH];
extern	OEMCHAR		hddfolder[MAX_PATH];
extern	OEMCHAR		bmpfilefolder[MAX_PATH];

void np2active_renewal(void);

