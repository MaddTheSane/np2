
#ifdef STRICT
#define	SUBCLASSPROC	WNDPROC
#else
#define	SUBCLASSPROC	FARPROC
#endif

#if defined(_WIN64)
#define GetWindowInst(h)	(HINSTANCE)GetWindowLongPtr((h), GWLP_HINSTANCE)
#define GetWindowProc(h)	(SUBCLASSPROC)GetWindowLongPtr((h), GWLP_WNDPROC)
#define SetWindowProc(h, f)	SetWindowLongPtr((h), GWLP_WNDPROC, (LONG)(f))
#else
#define GetWindowInst(h)	(HINSTANCE)GetWindowLong((h), GWL_HINSTANCE)
#define GetWindowProc(h)	(SUBCLASSPROC)GetWindowLong((h), GWL_WNDPROC)
#define SetWindowProc(h, f)	SetWindowLong((h), GWL_WNDPROC, (LONG)(f))
#endif


#ifdef __cplusplus
extern "C" {
#endif

void __msgbox(const char *title, const char *msg);

#ifdef __cplusplus
}
#endif

