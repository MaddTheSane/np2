
enum {
	LIO_SUCCESS			= 0,
	LIO_ILLEGALFUNC		= 5,
	LIO_OUTOFMEMORY		= 7
};


typedef struct {
	UINT8	scrnmode;
	UINT8	pos;
	UINT8	plane;
	UINT8	fgcolor;
	UINT8	bgcolor;
	UINT8	padding;
	UINT8	color[8];
	BYTE	viewx1[2];
	BYTE	viewy1[2];
	BYTE	viewx2[2];
	BYTE	viewy2[2];
	UINT8	disp;
	UINT8	access;
} LIOMEM;

enum {
	LIODRAW_PMASK	= 0x03,
	LIODRAW_MONO	= 0x04,
	LIODRAW_UPPER	= 0x20,
	LIODRAW_4BPP	= 0x40
};

typedef struct {
	SINT16	x1;
	SINT16	y1;
	SINT16	x2;
	SINT16	y2;
	UINT32	base;
	UINT8	flag;
	UINT8	palmax;
	UINT8	bank;
	UINT8	sbit;
} LIODRAW;


typedef struct {
	LIOMEM	mem;
	UINT8	palmode;

	// ---- work
	UINT32	wait;
	LIODRAW	draw;
} _LIOWORK, *LIOWORK;


#ifdef __cplusplus
extern "C" {
#endif

extern const UINT32 lioplaneadrs[4];

void lio_initialize(void);
void bios_lio(REG8 cmd);

void lio_updatedraw(LIOWORK lio);
void lio_pset(const _LIOWORK *lio, SINT16 x, SINT16 y, REG8 pal);
void lio_line(const _LIOWORK *lio, SINT16 x1, SINT16 x2, SINT16 y, REG8 pal);

REG8 lio_ginit(LIOWORK lio);
REG8 lio_gscreen(LIOWORK lio);
REG8 lio_gview(LIOWORK lio);
REG8 lio_gcolor1(LIOWORK lio);
REG8 lio_gcolor2(LIOWORK lio);
REG8 lio_gcls(LIOWORK lio);
REG8 lio_gpset(LIOWORK lio);
REG8 lio_gline(LIOWORK lio);
REG8 lio_gcircle(LIOWORK lio);
REG8 lio_gpaint1(LIOWORK lio);
REG8 lio_gpaint2(LIOWORK lio);
REG8 lio_gget(LIOWORK lio);
REG8 lio_gput1(LIOWORK lio);
REG8 lio_gput2(LIOWORK lio);
REG8 lio_groll(LIOWORK lio);
REG8 lio_gpoint2(LIOWORK lio);
REG8 lio_gcopy(LIOWORK lio);

#ifdef __cplusplus
}
#endif

