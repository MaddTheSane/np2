
typedef struct {
	BYTE	*ptr;
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	UINT	bpp;
	int		extend;
} SCRNSURF;


#ifdef __cplusplus
extern "C" {
#endif

void scrnmng_initialize(void);
BOOL scrnmng_create(HWND hWnd, LONG width, LONG height);
void scrnmng_destroy(void);

void scrnmng_setwidth(int posx, int width);
#define scrnmng_setextend(e)
void scrnmng_setheight(int posy, int height);
const SCRNSURF *scrnmng_surflock(void);
void scrnmng_surfunlock(const SCRNSURF *surf);
void scrnmng_update(void);

#define	scrnmng_haveextend()	(0)
#define	scrnmng_getbpp()		(16)
#define	scrnmng_allflash()		
#define	scrnmng_palchanged()	

RGB16 scrnmng_makepal16(RGB32 pal32);

#ifdef __cplusplus
}
#endif














#if 0
typedef struct {
	BYTE	*ptr;
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	int		bit;
const void	*arg;
} DEST_SURFACE;


#ifdef __cplusplus
extern "C" {
#endif

void gdraws_init(void);
BOOL gdraws_InitDirectDraw(HWND hWnd, LONG width, LONG height);
void gdraws_TermDirectDraw(void);
BOOL gdraws_draws(void (*cb)(DEST_SURFACE *ds), const void *arg);
void gdraws_enable(BOOL enable);
BOOL gdraws_mousepos(LPARAM *lp);
void gdraws_keybinds(struct GXKeyList *gxkey);
long gdraws_getyalign(void);

#ifdef __cplusplus
}
#endif
#endif

