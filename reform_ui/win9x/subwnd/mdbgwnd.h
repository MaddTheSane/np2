
#pragma once

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
BOOL mdbgwin_initialize(HINSTANCE hInstance);
void mdbgwin_create(HINSTANCE hInstance);
void mdbgwin_destroy(void);
void mdbgwin_process(void);
HWND mdbgwin_gethwnd(void);
void mdbgwin_readini(void);
void mdbgwin_writeini(void);
#else
#define mdbgwin_initialize(i)	(SUCCESS)
#define	mdbgwin_create(i)
#define	mdbgwin_destroy()
#define	mdbgwin_process()
#define	mdbgwin_gethwnd()		(NULL)
#define mdbgwin_readini()
#define mdbgwin_writeini()
#endif
