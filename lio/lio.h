
typedef struct {
	BYTE	mode;
	BYTE	sw;
	BYTE	act;
	BYTE	disp;
} LIOGSCREEN;

typedef struct {
	SINT16	x1;
	SINT16	y1;
	SINT16	x2;
	SINT16	y2;
	BYTE	vdraw_bg;
	BYTE	vdraw_ln;
} LIOGVIEW;

typedef struct {
	BYTE	palmax;					// command dummy
	BYTE	bgcolor;
	BYTE	bdcolor;
	BYTE	fgcolor;
	BYTE	palmode;
} LIOGCOLOR1;


// ----

typedef struct {
	UINT16	top;
	UINT16	lines;
	BYTE	bank;
	BYTE	plane;
	BYTE	disp;
	BYTE	dbit;
} LIO_SCRN;


typedef struct {
	SINT16	x1;
	SINT16	y1;
	SINT16	x2;
	SINT16	y2;
	BYTE	degcol[8];

	LIO_SCRN	scrn;
	LIOGSCREEN	gscreen;
	LIOGVIEW	gview;
	LIOGCOLOR1	gcolor1;
} LIO_TABLE;


#ifdef __cplusplus
extern "C" {
#endif

extern LIO_TABLE lio;

void lio_init(void);
void bios_lio(BYTE cmd);

void lio_makeviewmask(void);
void lio_pset(SINT16 x, SINT16 y, BYTE pal);
void lio_line(SINT16 x1, SINT16 x2, SINT16 y, BYTE pal);

BYTE lio_ginit(void);
BYTE lio_gscreen(void);
BYTE lio_gview(void);
BYTE lio_gcolor1(void);
BYTE lio_gcolor2(void);
BYTE lio_gcls(void);
BYTE lio_gpset(void);
BYTE lio_gline(void);
BYTE lio_gcircle(void);
BYTE lio_gpaint1(void);
BYTE lio_gpaint2(void);
BYTE lio_gget(void);
BYTE lio_gput1(void);
BYTE lio_gput2(void);
BYTE lio_groll(void);
BYTE lio_gpoint2(void);
BYTE lio_gcopy(void);

#ifdef __cplusplus
}
#endif

