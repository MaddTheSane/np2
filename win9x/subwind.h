
#if !defined(SUPPORT_KEYDISP)
#define kdispwin_initialize(i)	(TRUE)
#define	kdispwin_create()
#define	kdispwin_destroy()
#define	kdispwin_gethwnd()		(NULL)
#define	kdispwin_draw(c)
#define	kdispwin_readini()
#define	kdispwin_writeini()
#else
BOOL kdispwin_initialize(HINSTANCE hPreInst);
void kdispwin_create(void);
void kdispwin_destroy(void);
HWND kdispwin_gethwnd(void);
void kdispwin_draw(BYTE cnt);
void kdispwin_readini(void);
void kdispwin_writeini(void);
#endif

#if !defined(SUPPORT_SOFTKBD)
#define	skbdwin_initialize(i)	(TRUE)
#define	skbdwin_deinitialize()
#define	skbdwin_create()
#define	skbdwin_destroy()
#define	skbdwin_gethwnd()		(NULL)
#define	skbdwin_process()
#define	skbdwin_readini()
#define	skbdwin_writeini()
#else
BOOL skbdwin_initialize(HINSTANCE hPreInst);
void skbdwin_deinitialize(void);
void skbdwin_create(void);
void skbdwin_destroy(void);
HWND skbdwin_gethwnd(void);
void skbdwin_process(void);
void skbdwin_readini(void);
void skbdwin_writeini(void);
#endif

