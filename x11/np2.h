#ifndef	NP2_X11_NP2_H__
#define	NP2_X11_NP2_H__

typedef struct {
	BYTE	port;
	BYTE	def_en;
	BYTE	param;
	UINT32	speed;
	char	mout[MAX_PATH];
	char	min[MAX_PATH];
	char	mdl[64];
	char	def[MAX_PATH];
} COMCFG;

typedef struct {
	char	titles[256];

	UINT	paddingx;
	UINT	paddingy;

	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;

	BYTE	DISPCLK;

	BYTE	KEYBOARD;
	BYTE	F12KEY;

	BYTE	MOUSE_SW;
	BYTE	JOYPAD1;
	BYTE	JOYPAD2;
	BYTE	JOY1BTN[4];

	COMCFG	mpu;

	BYTE	confirm;

	BYTE	resume;						// ver0.30

	BYTE	statsave;
	BYTE	toolwin;
	BYTE	keydisp;
	BYTE	hostdrv_write;
	BYTE	jastsnd;
	BYTE	I286SAVE;

	BYTE	snddrv;
	char	audiodev[MAX_PATH];
	char	MIDIDEV[2][MAX_PATH];
	UINT32	MIDIWAIT;

	BYTE	mouse_move_ratio;

	char	toolkit[32];

	BYTE	disablemmx;
	BYTE	shared_pixmap;
} NP2OSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};

enum {
	MMXFLAG_DISABLE		= 1,
	MMXFLAG_NOTSUPPORT	= 2
};

#ifdef __cplusplus
extern "C" {
#endif

/* np2.c */
extern BOOL np2running;
extern NP2OSCFG np2oscfg;
extern BYTE scrnmode;

extern UINT framecnt;
extern UINT waitcnt;
extern UINT framemax;

extern BOOL s98logging;
extern int s98log_count;

extern BOOL use_shared_pixmap;

extern char hddfolder[MAX_PATH];
extern char fddfolder[MAX_PATH];
extern char bmpfilefolder[MAX_PATH];
extern char modulefile[MAX_PATH];
extern char statpath[MAX_PATH];
extern char fontname[1024];
extern char fontfilename[MAX_PATH];

int flagload(const char* ext, const char* title, BOOL force);
int flagsave(const char* ext);
void flagdelete(const char* ext);

void changescreen(BYTE newmode);
void framereset(UINT cnt);
void processwait(UINT cnt);
int mainloop(void *);

#if defined(__GNUC__) && (defined(i386) || defined(__i386__))
extern int mmxflag;
int havemmx(void);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_NP2_H__ */
