#if defined(SUPPORT_KEYDISP)

BOOL kdispwin_initialize(void);
void kdispwin_create(void);
void kdispwin_destroy(void);
void kdispwin_draw(BYTE cnt);
void kdispwin_readini(void);
void kdispwin_writeini(void);

#else

#define kdispwin_initialize()
#define kdispwin_create()
#define kdispwin_destroy()
#define kdispwin_draw(a)
#define kdispwin_readini()
#define kdispwin_writeini()

#endif
