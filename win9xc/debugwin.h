
#if defined(SUPPORT_MEMDBG32)

BOOL debugwin_initapp(HINSTANCE hInstance);
void debugwin_create(void);
void debugwin_destroy(void);
void debugwin_process(void);

#else

#define debugwin_initapp(i)		(TRUE)
#define	debugwin_create()
#define	debugwin_destroy()
#define	debugwin_process()

#endif

