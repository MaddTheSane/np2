
enum {
	FONT_ANK8		= 0x01,
	FONT_ANK16a		= 0x02,
	FONT_ANK16b		= 0x04,
	FONT_KNJ1		= 0x08,
	FONT_KNJ2		= 0x10,
	FONT_KNJ3		= 0x20,

	FONTLOAD_KNJ	= (FONT_KNJ1 | FONT_KNJ2 | FONT_KNJ3),
	FONTLOAD_ANK	= (FONT_ANK16a | FONT_ANK16b),
	FONTLOAD_16		= (FONTLOAD_ANK | FONTLOAD_KNJ),
	FONTLOAD_ALL	= (FONT_ANK8 | FONTLOAD_16)
};

enum {
	FONTTYPE_NONE	= 0,
	FONTTYPE_PC98,
	FONTTYPE_V98,
	FONTTYPE_PC88,
	FONTTYPE_FM7,
	FONTTYPE_X1,
	FONTTYPE_X68
};


#ifdef __cplusplus
extern "C" {
#endif

extern const char pc88ankname[];
extern const char pc88knj1name[];
extern const char pc88knj2name[];
extern const char pc98fontname[];
extern const char v98fontname[];
extern const char fm7ankname[];
extern const char fm7knjname[];
extern const char x1ank1name[];
extern const char x1ank2name[];
extern const char x1knjname[];
extern const char x68kfontname[];

extern const BYTE fontdata_8[256*8];
extern const BYTE fontdata_16[3*32*16];
extern const BYTE fontdata_29[94*16];
extern const BYTE fontdata_2a[94*16];
extern const BYTE fontdata_2b[94*16];
extern const BYTE fontdata_2c[76*16*2];

void fontdata_patch16a(void);
void fontdata_patch16b(void);
void fontdata_patchjis(void);

BYTE fontpc88_read(const char *filename, BYTE loading);
BYTE fontpc98_read(const char *filename, BYTE loading);
BYTE fontv98_read(const char *filename, BYTE loading);
BYTE fontfm7_read(const char *filename, BYTE loading);
BYTE fontx1_read(const char *filename, BYTE loading);
BYTE fontx68k_read(const char *filename, BYTE loading);

#ifdef __cplusplus
}
#endif

