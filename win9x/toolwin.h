
enum {
	FDDLIST_DRV			= 2,
	FDDLIST_MAX			= 8
};

typedef struct {
	int		insert;
	char	name[FDDLIST_MAX][MAX_PATH];
} TOOLFDD;

typedef struct {
	int		posx;
	int		posy;
	TOOLFDD	fdd[FDDLIST_DRV];
} NP2TOOL;

extern	NP2TOOL		np2tool;

BOOL toolwin_initapp(HINSTANCE hInstance);
void toolwin_open(void);
void toolwin_close(void);

void toolwin_movingstart(void);
void toolwin_movingend(void);
void toolwin_setfdd(BYTE drv, const char *name);

#ifdef __cplusplus
extern "C" {
#endif
void toolwin_fddaccess(BYTE drv);
void toolwin_hddaccess(BYTE drv);
#ifdef __cplusplus
}
#endif
void toolwin_draw(BYTE frame);

void toolwin_readini(void);
void toolwin_writeini(void);

