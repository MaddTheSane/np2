
#if defined(SUPPORT_KEYDISP)
BOOL kdispwin_initialize(HINSTANCE hPreInst);
void kdispwin_create(void);
void kdispwin_destroy(void);
HWND kdispwin_gethwnd(void);
void kdispwin_draw(BYTE cnt);
void kdispwin_readini(void);
void kdispwin_writeini(void);
#else
#define kdispwin_initialize(i)	(SUCCESS)
#define	kdispwin_create()
#define	kdispwin_destroy()
#define	kdispwin_gethwnd()		(NULL)
#define	kdispwin_draw(c)
#define	kdispwin_readini()
#define	kdispwin_writeini()
#endif

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
BOOL memdbg_initialize(HINSTANCE hInstance);
void memdbg_create(void);
void memdbg_destroy(void);
void memdbg_process(void);
HWND memdbg_gethwnd(void);
void memdbg_readini(void);
void memdbg_writeini(void);
#else
#define memdbg_initialize(i)	(SUCCESS)
#define	memdbg_create()
#define	memdbg_destroy()
#define	memdbg_process()
#define	memdbg_gethwnd()		(NULL)
#define memdbg_readini()
#define memdbg_writeini()
#endif

#if defined(SUPPORT_SOFTKBD)
BOOL skbdwin_initialize(HINSTANCE hPreInst);
void skbdwin_deinitialize(void);
void skbdwin_create(void);
void skbdwin_destroy(void);
HWND skbdwin_gethwnd(void);
void skbdwin_process(void);
void skbdwin_readini(void);
void skbdwin_writeini(void);
#else
#define	skbdwin_initialize(i)	(SUCCESS)
#define	skbdwin_deinitialize()
#define	skbdwin_create()
#define	skbdwin_destroy()
#define	skbdwin_gethwnd()		(NULL)
#define	skbdwin_process()
#define	skbdwin_readini()
#define	skbdwin_writeini()
#endif

