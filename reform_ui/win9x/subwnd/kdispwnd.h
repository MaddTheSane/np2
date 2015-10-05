
#pragma once

#if defined(SUPPORT_KEYDISP)
BOOL kdispwin_initialize(HINSTANCE hInstance);
void kdispwin_create(HINSTANCE hInstance);
void kdispwin_destroy(void);
HWND kdispwin_gethwnd(void);
void kdispwin_draw(UINT8 cnt);
void kdispwin_readini(void);
void kdispwin_writeini(void);
#else
#define kdispwin_initialize(i)	(SUCCESS)
#define	kdispwin_create(i)
#define	kdispwin_destroy()
#define	kdispwin_gethwnd()		(NULL)
#define	kdispwin_draw(c)
#define	kdispwin_readini()
#define	kdispwin_writeini()
#endif
