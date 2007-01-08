
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
BOOL mdbgwin_initialize(HINSTANCE hPreInst);
void mdbgwin_create(void);
void mdbgwin_destroy(void);
void mdbgwin_process(void);
HWND mdbgwin_gethwnd(void);
void mdbgwin_readini(void);
void mdbgwin_writeini(void);
#else
#define mdbgwin_initialize(i)	(SUCCESS)
#define	mdbgwin_create()
#define	mdbgwin_destroy()
#define	mdbgwin_process()
#define	mdbgwin_gethwnd()		(NULL)
#define mdbgwin_readini()
#define mdbgwin_writeini()
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

