
enum {
	LIO_SUCCESS			= 0,
	LIO_ILLEGALFUNC		= 5,
	LIO_OUTOFMEMORY		= 7
};


typedef struct {
	UINT8	mode;
	UINT8	sw;
	UINT8	act;
	UINT8	disp;
} LIOGSCREEN;

typedef struct {
	SINT16	x1;
	SINT16	y1;
	SINT16	x2;
	SINT16	y2;
	UINT8	vdraw_bg;
	UINT8	vdraw_ln;
} LIOGVIEW;

typedef struct {
	UINT8	palmax;
	UINT8	bgcolor;
	UINT8	bdcolor;
	UINT8	fgcolor;
	UINT8	palmode;
} LIOGCOLOR1;


// ----

typedef struct {
	UINT16	top;
	UINT16	lines;
	UINT8	bank;
	UINT8	plane;
	UINT8	disp;
	UINT8	dbit;
} LIO_SCRN;

typedef struct {
	SINT16	x1;
	SINT16	y1;
	SINT16	x2;
	SINT16	y2;
} LIORANGE;


typedef struct {
	LIO_SCRN	scrn;
	LIOGSCREEN	gscreen;
	LIOGVIEW	gview;
	LIOGCOLOR1	gcolor1;
	UINT8		degcol[8];

	LIORANGE	range;

} _LIOWORK, *LIOWORK;


#ifdef __cplusplus
extern "C" {
#endif

extern const UINT32 lioplaneadrs[4];

void lio_initialize(void);
void bios_lio(REG8 cmd);

void lio_updaterange(LIOWORK lio);
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

