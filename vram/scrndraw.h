
#ifndef SCRNCALL
#define	SCRNCALL
#endif

enum {
	SURFACE_WIDTH		= 640,
	SURFACE_HEIGHT		= 480,
	SURFACE_SIZE		= (SURFACE_WIDTH * SURFACE_HEIGHT),

	START_PALORG		= 0x0a,
	START_PAL			= 0x10
};


#ifdef __cplusplus
extern "C" {
#endif

extern	BYTE	renewal_line[SURFACE_HEIGHT];
extern	BYTE	np2_tram[SURFACE_SIZE];
extern	BYTE	np2_vram[2][SURFACE_SIZE];

void scrndraw_initialize(void);
void scrndraw_changepalette(void);
BYTE scrndraw_draw(BYTE update);
void scrndraw_redraw(void);

#ifdef __cplusplus
}
#endif

