#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"i286.h"


static	char		np2viewclass[] = "NP2-ViewWindow";
		NP2VIEW_T	np2view[NP2VIEW_MAX];
extern	HINSTANCE	hInst;


static void viewer_segmode(HWND hwnd, BYTE type) {

	NP2VIEW_T	*view;

	view = viewcmn_find(hwnd);
	if ((view) && (view->type != type)) {
		viewcmn_setmode(view, view, type);
		viewcmn_setbank(view);
		viewcmn_setvscroll(hwnd, view);
		InvalidateRect(hwnd, NULL, TRUE);
	}
}


static void vieweractive_renewal(void) {

	int			i;
	NP2VIEW_T	*view;

	view = np2view;
	np2break &= ~NP2BREAK_DEBUG;
	for (i=0; i<NP2VIEW_MAX; i++, view++) {
		if ((view->alive) && (view->active)) {
			np2break |= NP2BREAK_DEBUG;
			break;
		}
	}
	np2active_renewal();
}


static void viewer_close(NP2VIEW_T *view) {

	view->alive = FALSE;
	viewcmn_free(&view->buf1);
	viewcmn_free(&view->buf2);
}


LRESULT CALLBACK ViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	NP2VIEW_T *view;

	switch (msg) {
		case WM_CREATE:
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDM_VIEWWINNEW:
					viewer_open();
					break;

				case IDM_VIEWWINCLOSE:
					return(ViewProc(hWnd, WM_CLOSE, 0, 0));
					break;

				case IDM_VIEWWINALLCLOSE:
					viewer_allclose();
					break;

				case IDM_VIEWMODEREG:
					viewer_segmode(hWnd, VIEWMODE_REG);
					break;

				case IDM_VIEWMODESEG:
					viewer_segmode(hWnd, VIEWMODE_SEG);
					break;

				case IDM_VIEWMODE1MB:
					viewer_segmode(hWnd, VIEWMODE_1MB);
					break;

				case IDM_VIEWMODEASM:
					viewer_segmode(hWnd, VIEWMODE_ASM);
					break;

				case IDM_VIEWMODESND:
					viewer_segmode(hWnd, VIEWMODE_SND);
					break;

				default:
					return(viewcmn_dispat(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_PAINT:
			return(viewcmn_dispat(hWnd, msg, wParam, lParam));

		case WM_SIZE:
			view = viewcmn_find(hWnd);
			if (view) {
				RECT	rc;
				GetClientRect(hWnd, &rc);
				view->step = rc.bottom / 16;
				viewcmn_setvscroll(hWnd, view);
			}
			break;

		case WM_VSCROLL:
			view = viewcmn_find(hWnd);
			if (view) {
				DWORD newpos = view->pos;
				switch(LOWORD(wParam)) {
					case SB_LINEUP:
						if (newpos) {
							newpos--;
						}
						break;
					case SB_LINEDOWN:
						if (newpos < (view->maxline - view->step)) {
							newpos++;
						}
						break;
					case SB_PAGEUP:
						if (newpos > view->step) {
							newpos -= view->step;
						}
						else {
							newpos = 0;
						}
						break;
					case SB_PAGEDOWN:
						newpos += view->step;
						if (newpos > (view->maxline - view->step)) {
							newpos = view->maxline - view->step;
						}
						break;
					case SB_THUMBTRACK:
						newpos = HIWORD(wParam) * (view->mul);
						break;
				}
				if (view->pos != newpos) {
					view->pos = newpos;
					viewcmn_setvscroll(hWnd, view);
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			break;

		case WM_ENTERMENULOOP:
			viewcmn_setmenuseg(hWnd);
			break;

		case WM_ACTIVATE:
			view = viewcmn_find(hWnd);
			if (view) {
				if (LOWORD(wParam) != WA_INACTIVE) {
					view->active = 1;
					InvalidateRect(hWnd, NULL, TRUE);
				}
				else {
					view->active = 0;
				}
				vieweractive_renewal();
			}
			break;

		case WM_CLOSE:
			view = viewcmn_find(hWnd);
			DestroyWindow(hWnd);
			if (view) {
				viewer_close(view);
				vieweractive_renewal();
			}
			break;

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}


// -----------------------------------------------------------------------

BOOL viewer_init(HINSTANCE hPreInst) {

	WNDCLASS	np2vc;

	ZeroMemory(np2view, sizeof(np2view));

	if (!hPreInst) {
		np2vc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
		np2vc.lpfnWndProc = ViewProc;
		np2vc.cbClsExtra = 0;
		np2vc.cbWndExtra = 0;
		np2vc.hInstance = hInst;
		np2vc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));
		np2vc.hCursor = LoadCursor(NULL, IDC_ARROW);
		np2vc.hbrBackground = (HBRUSH)0;
		np2vc.lpszMenuName = MAKEINTRESOURCE(IDM_VIEW);
		np2vc.lpszClassName = np2viewclass;
		if (!RegisterClass(&np2vc)) {
			return(FAILURE);
		}
	}
	return(SUCCESS);
}


void viewer_term(void) {

}


void viewer_open(void) {

	int			i;
	NP2VIEW_T	*view;

	view = np2view;
	for (i=0; i<NP2VIEW_MAX; i++, view++) {
		if (!view->alive) {
			char buf[256];
			viewcmn_caption(view, buf);
			ZeroMemory(view, sizeof(NP2VIEW_T));
			view->alive = TRUE;
			view->hwnd = CreateWindowEx(0,
							np2viewclass, buf,
							WS_OVERLAPPEDWINDOW | WS_VSCROLL,
							CW_USEDEFAULT, CW_USEDEFAULT,
							CW_USEDEFAULT, CW_USEDEFAULT,
							NULL, NULL, hInst, NULL);
			viewcmn_setmode(view, NULL, VIEWMODE_REG);
			ShowWindow(view->hwnd, SW_SHOWNORMAL);
			UpdateWindow(view->hwnd);
			break;
		}
	}
}


void viewer_allclose(void) {

	int			i;
	NP2VIEW_T	*view;

	view = np2view;
	for (i=0; i<NP2VIEW_MAX; i++, view++) {
		if (view->alive) {
			DestroyWindow(view->hwnd);
			viewer_close(view);
		}
	}
	vieweractive_renewal();
}


void viewer_allreload(BOOL force) {

static	DWORD	last = 0;
		DWORD	now;

	now = GetTickCount();
	if ((force) || ((now - last) >= 200)) {
		int			i;
		NP2VIEW_T	*view;

		last = now;
		view = np2view;
		for (i=0; i<NP2VIEW_MAX; i++, view++) {
			if ((view->alive) && (!view->lock)) {
				if (view->type == VIEWMODE_ASM) {
					view->seg = I286_CS;
					view->off = I286_IP;
					view->pos = 0;
					viewcmn_setvscroll(view->hwnd, view);
				}
				viewcmn_setbank(view);
				InvalidateRect(view->hwnd, NULL, TRUE);
			}
		}
	}
}

