#include	"compiler.h"
#include	<stdarg.h>
#include	"resource.h"
#include	"strres.h"
#include	"textfile.h"
#include	"dosio.h"
#include	"ini.h"
#include	"menu.h"

#ifdef TRACE

#ifdef STRICT
#define	SUBCLASSPROC	WNDPROC
#else
#define	SUBCLASSPROC	FARPROC
#endif

#define	IDC_VIEW		(WM_USER + 100)
#define	VIEW_BUFFERSIZE	4096
#define	VIEW_FGCOLOR	0x000000
#define	VIEW_BGCOLOR	0xffffff
#define	VIEW_TEXT		"�l�r �S�V�b�N"
#define	VIEW_SIZE		12

typedef struct
{
	UINT8		en;
	TEXTFILEH	tf;
	HWND		hwnd;
} TRACEWIN;

typedef struct {
	int		posx;
	int		posy;
	int		width;
	int		height;
} TRACECFG;

extern	HINSTANCE	g_hInstance;

static const TCHAR ProgTitle[] = _T("console");
static const TCHAR ClassName[] = _T("TRACE-console");
static const TCHAR ClassEdit[] = _T("EDIT");
static const TCHAR viewfont[] = _T(VIEW_TEXT);

static const OEMCHAR crlf[] = OEMTEXT("\r\n");

static	TRACEWIN	tracewin;
static	HWND		s_hView = NULL;
static	HFONT		s_hfView = NULL;
static	HBRUSH		s_hBrush = NULL;
static	int			viewpos;
static	int			viewleng;
static	TCHAR		viewbuf[VIEW_BUFFERSIZE * 2];
static	TRACECFG	tracecfg;
static	int			devpos;
static	char		devstr[256];

static const OEMCHAR np2trace[] = OEMTEXT("np2trace.ini");
static const OEMCHAR inititle[] = OEMTEXT("TRACE");
static const PFTBL initbl[4] = {
			PFVAL("posx",	PFTYPE_SINT32,	&tracecfg.posx),
			PFVAL("posy",	PFTYPE_SINT32,	&tracecfg.posy),
			PFVAL("width",	PFTYPE_SINT32,	&tracecfg.width),
			PFVAL("height",	PFTYPE_SINT32,	&tracecfg.height)};

static void View_ScrollToBottom(HWND hWnd) {

	int		MinPos;
	int		MaxPos;

	GetScrollRange(hWnd, SB_VERT, &MinPos, &MaxPos);
	PostMessage(hWnd, EM_LINESCROLL, 0, MaxPos);
}

static void View_ClrString(void) {

	viewpos = 0;
	viewleng = 0;
	viewbuf[0] = '\0';
	SetWindowText(s_hView, viewbuf);
}

static void View_AddString(const OEMCHAR *string) {

	int		slen;
	int		vpos;
	int		vlen;
	TCHAR	c;

	slen = lstrlen(string);
	if ((slen == 0) || ((slen + 3) > VIEW_BUFFERSIZE)) {
		return;
	}
	vpos = viewpos;
	vlen = viewleng;
	if ((vpos + vlen + slen + 3) > (VIEW_BUFFERSIZE * 2)) {
		while(vlen > 0) {
			vlen--;
			c = viewbuf[vpos++];
			if ((c == 0x0a) && ((vlen + slen + 3) <= VIEW_BUFFERSIZE)) {
				break;
			}
		}
		if (vpos >= VIEW_BUFFERSIZE) {
			if (vlen) {
				CopyMemory(viewbuf, viewbuf + vpos, vlen * sizeof(TCHAR));
			}
			vpos = 0;
			viewpos = 0;
		}
	}
	CopyMemory(viewbuf + vpos + vlen, string, slen * sizeof(TCHAR));
	vlen += slen;
	viewbuf[vpos + vlen + 0] = '\r';
	viewbuf[vpos + vlen + 1] = '\n';
	viewbuf[vpos + vlen + 2] = '\0';
	viewleng = vlen + 2;
	SetWindowText(s_hView, viewbuf + vpos);
	View_ScrollToBottom(s_hView);
}


// ----

static LRESULT onCreate(HWND hWnd)
{
	HMENU	hMenu;
	RECT	rc;

	hMenu = GetSystemMenu(hWnd, FALSE);
	menu_addmenures(hMenu, 0, IDR_TRACE, FALSE);

	CheckMenuItem(hMenu, IDM_TRACE_TRACE,
								(tracewin.en & 1)?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_TRACE_VERBOSE,
								(tracewin.en & 2)?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_TRACE_ENABLE,
								(tracewin.en & 4)?MF_CHECKED:MF_UNCHECKED);

	GetClientRect(hWnd, &rc);
	s_hView = CreateWindowEx(WS_EX_CLIENTEDGE,
							ClassEdit, NULL,
							WS_CHILD | WS_VISIBLE | ES_READONLY | ES_LEFT |
							ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL,
							0, 0, rc.right, rc.bottom,
							hWnd, (HMENU)IDC_VIEW, g_hInstance, NULL);
	SendMessage(s_hView, EM_SETLIMITTEXT, (WPARAM)VIEW_BUFFERSIZE, 0);

	s_hfView = CreateFont(VIEW_SIZE, 0, 0, 0, 0, 0, 0, 0, 
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, viewfont);
	if (s_hfView)
	{
		SendMessage(s_hView, WM_SETFONT, (WPARAM)s_hfView,
												MAKELPARAM(TRUE, 0));
	}

	s_hBrush = CreateSolidBrush(VIEW_BGCOLOR);
	SetFocus(s_hView);

	return TRUE;
}

static LRESULT onSysCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HMENU	hMenu;

	hMenu = GetSystemMenu(hWnd, FALSE);
	switch(wParam)
	{
		case IDM_TRACE_TRACE:
			tracewin.en ^= 1;
			CheckMenuItem(hMenu, IDM_TRACE_TRACE,
								(tracewin.en & 1)?MF_CHECKED:MF_UNCHECKED);
			break;

		case IDM_TRACE_VERBOSE:
			tracewin.en ^= 2;
			CheckMenuItem(hMenu, IDM_TRACE_VERBOSE,
								(tracewin.en & 2)?MF_CHECKED:MF_UNCHECKED);
			break;

		case IDM_TRACE_ENABLE:
			tracewin.en ^= 4;
			CheckMenuItem(hMenu, IDM_TRACE_ENABLE,
								(tracewin.en & 4)?MF_CHECKED:MF_UNCHECKED);
			break;

		case IDM_TRACE_FILEOUT:
			if (tracewin.tf != NULL)
			{
				textfile_close(tracewin.tf);
				tracewin.tf = NULL;
			}
			else
			{
				tracewin.tf = textfile_create(OEMTEXT("traceout.txt"),
																0x800);
			}
			CheckMenuItem(hMenu, IDM_TRACE_FILEOUT,
									(tracewin.tf)?MF_CHECKED:MF_UNCHECKED);
			break;

		case IDM_TRACE_CLEAR:
			View_ClrString();
			break;

		default:
			return DefWindowProc(hWnd, WM_SYSCOMMAND, wParam, lParam);
	}
	return FALSE;
}

static LRESULT CALLBACK traceproc(HWND hWnd, UINT uMsg,
												WPARAM wParam, LPARAM lParam)
{
	RECT	rc;

	switch(uMsg)
	{
		case WM_CREATE:
			return onCreate(hWnd);

		case WM_SYSCOMMAND:
			return onSysCommand(hWnd, wParam, lParam);

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
											(WS_MAXIMIZE | WS_MINIMIZE)))
			{
				GetWindowRect(hWnd, &rc);
				tracecfg.posx = rc.left;
				tracecfg.posy = rc.top;
			}
			break;

		case WM_SIZE:							// window resize
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
										(WS_MAXIMIZE | WS_MINIMIZE)))
			{
				GetWindowRect(hWnd, &rc);
				tracecfg.width = rc.right - rc.left;
				tracecfg.height = rc.bottom - rc.top;
			}
			MoveWindow(s_hView, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			View_ScrollToBottom(s_hView);
			break;

		case WM_SETFOCUS:
			SetFocus(s_hView);
			return(0L);

		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
			SetTextColor((HDC)wParam, VIEW_FGCOLOR);
			SetBkColor((HDC)wParam, VIEW_BGCOLOR);
			return((LRESULT)s_hBrush);

		case WM_CLOSE:
			break;

		case WM_DESTROY:
			if (s_hBrush)
			{
				DeleteObject(s_hBrush);
			}
			if (s_hfView)
			{
				DeleteObject(s_hfView);
			}
			break;
#if 0
		case WM_ENTERSIZEMOVE:
			winloc_movingstart();
			break;

		case WM_MOVING:
			winloc_movingproc((RECT *)lParam);
			break;

		case WM_ERASEBKGND:
			return FALSE;
#endif
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return FALSE;
}


// ----

void trace_init(void) {

	HWND	hwnd;

	ZeroMemory(&tracewin, sizeof(tracewin));
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = traceproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClassName;
	if (!RegisterClass(&wc)) {
		return;
	}

#if 1
	tracewin.en = 4;
#else
	tracewin.en = 1;
	tracewin.tf = textfile_create(OEMTEXT("traceout.txt"), 0x800);
#endif

	tracecfg.posx = CW_USEDEFAULT;
	tracecfg.posy = CW_USEDEFAULT;
	tracecfg.width = CW_USEDEFAULT;
	tracecfg.height = CW_USEDEFAULT;
	ini_read(file_getcd(np2trace), inititle, initbl, NELEMENTS(initbl));

	hwnd = CreateWindowEx(WS_EX_CONTROLPARENT,
							ClassName, ProgTitle,
							WS_OVERLAPPEDWINDOW,
							tracecfg.posx, tracecfg.posy,
							tracecfg.width, tracecfg.height,
							NULL, NULL, g_hInstance, NULL);
	tracewin.hwnd = hwnd;
	if (hwnd == NULL) {
		return;
	}
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

void trace_term(void) {

	if (tracewin.tf != NULL) {
		textfile_close(tracewin.tf);
		tracewin.tf = NULL;
	}
	if (tracewin.hwnd) {
		DestroyWindow(tracewin.hwnd);
		tracewin.hwnd = NULL;
		ini_write(file_getcd(np2trace), inititle, initbl, NELEMENTS(initbl));
	}
}

void trace_fmt(const char *fmt, ...) {

	BOOL	en;
	va_list	ap;
	OEMCHAR	buf[0x1000];

	en = (tracewin.en & 1) &&
		((tracewin.en & 4) || (tracewin.tf != NULL));
	if (en) {
		va_start(ap, fmt);
#if defined(OSLANG_UCS2)
		OEMCHAR cnvfmt[0x800];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fmt, -1,
												cnvfmt, NELEMENTS(cnvfmt));
		vswprintf(buf, cnvfmt, ap);
#else
		vsprintf(buf, fmt, ap);
#endif
		va_end(ap);
		if ((tracewin.en & 4) && (s_hView)) {
			View_AddString(buf);
		}
		if (tracewin.tf != NULL) {
			textfile_write(tracewin.tf, buf);
			textfile_write(tracewin.tf, crlf);
		}
	}
}

void trace_fmt2(const char *fmt, ...) {

	BOOL	en;
	va_list	ap;
	OEMCHAR	buf[0x1000];

	en = (tracewin.en & 2) &&
		((tracewin.en & 4) || (tracewin.tf != NULL));
	if (en) {
		va_start(ap, fmt);
#if defined(OSLANG_UCS2)
		OEMCHAR cnvfmt[0x800];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fmt, -1,
												cnvfmt, NELEMENTS(cnvfmt));
		vswprintf(buf, cnvfmt, ap);
#else
		vsprintf(buf, fmt, ap);
#endif
		va_end(ap);
		if ((tracewin.en & 4) && (s_hView)) {
			View_AddString(buf);
		}
		if (tracewin.tf != FILEH_INVALID) {
			textfile_write(tracewin.tf, buf);
			textfile_write(tracewin.tf, crlf);
		}
	}
}

void trace_char(char c) {

	if ((c == 0x0a) || (c == 0x0d)) {
		if (devpos) {
			devstr[devpos] = '\0';
#if defined(OSLANG_UCS2)
			TCHAR pdevstr[0x800];
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, devstr, -1,
												pdevstr, NELEMENTS(pdevstr));
#else
			const OEMCHAR *pdevstr = devstr;
#endif
			if ((tracewin.en & 4) && (s_hView))
			{
				View_AddString(pdevstr);
			}
			if (tracewin.tf != NULL) {
				textfile_write(tracewin.tf, pdevstr);
				textfile_write(tracewin.tf, crlf);
			}
			devpos = 0;
		}
	}
	else {
		if (devpos < (sizeof(devstr) - 1)) {
			devstr[devpos++] = c;
		}
	}
}
#endif

