
#define	GDCCMD_MAX	32

typedef struct {
	BYTE	para[256];
	UINT16	fifo[GDCCMD_MAX];
	UINT16	cnt;
	BYTE	ptr;
	BYTE	rcv;
	BYTE	snd;
	BYTE	cmd;				// ver0.29
	BYTE	paracb;				// ver0.29
	BYTE	reserved[3];
} _GDCDATA, *GDCDATA;

typedef struct {
	_GDCDATA	m;
	_GDCDATA	s;
	BYTE		mode1;
	BYTE		mode2;
	BYTE		vsync;
	BYTE		vsyncint;
	int			analog;
	int			palnum;
	BYTE		degpal[4];
	RGB32		anapal[16];
	BYTE		clock;
	BYTE		display;
	BYTE		bitac;
	BYTE		m_drawing;
	BYTE		s_drawing;
	BYTE		reserved[3];
} _GDC, *GDC;

typedef struct {
	BYTE	access;
	BYTE	disp;
	BYTE	textdisp;
	BYTE	msw_accessable;
	BYTE	grphdisp;
	BYTE	palchange;
	BYTE	mode2;
} _GDCS, *GDCS;

enum {
	GDC_DEGBBIT			= 0x01,
	GDC_DEGRBIT			= 0x02,
	GDC_DEGGBIT			= 0x04,

	GDCSCRN_ENABLE		= 0x80,
	GDCSCRN_EXT			= 0x40,
	GDCSCRN_ATR			= 0x10,
	GDCSCRN_ALLDRAW		= 0x04,
	GDCSCRN_REDRAW		= 0x01,
	GDCSCRN_ALLDRAW2	= 0x0c,
	GDCSCRN_REDRAW2		= 0x03,
	GDCSCRN_MAKE		= (GDCSCRN_ALLDRAW | GDCSCRN_REDRAW),

	GDCWORK_MASTER		= 0,
	GDCWORK_SLAVE		= 1
};


#ifdef __cplusplus
extern "C" {
#endif

void gdc_reset(void);
void gdc_bind(void);

void gdc_work(int id);
void gdc_forceready(GDCDATA item);
void gdc_paletteinit(void);

void gdc_setdegitalpal(int color, BYTE value);
void gdc_setanalogpal(int color, int rgb, BYTE value);
void gdc_setdegpalpack(int color, BYTE value);

void gdc_restorekacmode(void);

#ifdef __cplusplus
}
#endif

