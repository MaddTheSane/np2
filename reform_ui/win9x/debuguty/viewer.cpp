/**
 * @file	viewer.cpp
 * @brief	DebugUty �p�r���[�� �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "resource.h"
#include "np2.h"
#include "viewer.h"
#include "viewcmn.h"
#include "cpucore.h"

// static	const TCHAR		np2viewclass[] = _T("NP2-ViewWindow");
		const TCHAR		np2viewfont[] = _T("�l�r �S�V�b�N");
static CDebugUtyView* g_np2view[NP2VIEW_MAX];

/**
 * �E�B���h�E������
 * @param[in] hWnd �E�B���h�E �n���h��
 * @return �C���X�^���X
 */
CDebugUtyView* CDebugUtyView::FromWnd(HWND hWnd)
{
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		CDebugUtyView* lpView = g_np2view[i];
		if ((lpView != NULL) && (lpView->m_hWnd == hWnd))
		{
			return lpView;
		}
	}
	return NULL;
}

/**
 * �R���X�g���N�^
 */
CDebugUtyView::CDebugUtyView()
	: hwnd(NULL)
	, pos(0)
	, maxline(0)
	, step(0)
	, mul(0)
	, type(0)
	, lock(0)
	, active(0)
	, seg(0)
	, off(0)
{
	ZeroMemory(&this->buf1, sizeof(this->buf1));
	ZeroMemory(&this->buf2, sizeof(this->buf2));
	ZeroMemory(&this->dmem, sizeof(this->dmem));
}

/**
 * �f�X�g���N�^
 */
CDebugUtyView::~CDebugUtyView()
{
	viewcmn_free(&this->buf1);
	viewcmn_free(&this->buf2);

	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		if (g_np2view[i] == this)
		{
			g_np2view[i] = NULL;
		}
	}
}

/**
 * �L���v�V�����̍X�V
 */
void CDebugUtyView::UpdateCaption()
{
	int nIndex = -1;
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		if (g_np2view[i] == this)
		{
			nIndex = i;
			break;
		}
	}
	LPCTSTR lpMode = (this->lock) ? TEXT("Locked") : TEXT("Realtime");


	TCHAR szTitle[256];
	wsprintf(szTitle, TEXT("%d.%s - NP2 Debug Utility"), nIndex + 1, lpMode);

	SetWindowText(szTitle);
}

// ----

static void vieweractive_renewal(void)
{
	np2break &= ~NP2BREAK_DEBUG;

	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		const NP2VIEW_T* view = g_np2view[i];
		if ((view != NULL) && (view->active))
		{
			np2break |= NP2BREAK_DEBUG;
			break;
		}
	}
	np2active_renewal();
}

/**
 * �E�B���h�E �v���V�[�W��
 * @param[in] message ���b�Z�[�W
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @return ���U���g �R�[�h
 */
LRESULT CDebugUtyView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	NP2VIEW_T* view = this;
	UINT msg = message;
	HWND hWnd = *this;

	switch (msg) {
		case WM_CREATE:
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDM_VIEWWINNEW:
					viewer_open(g_hInstance);
					break;

				case IDM_VIEWWINCLOSE:
					return WindowProc(WM_CLOSE, 0, 0);

				case IDM_VIEWWINALLCLOSE:
					viewer_allclose();
					break;

				case IDM_VIEWMODEREG:
					SetMode(VIEWMODE_REG);
					break;

				case IDM_VIEWMODESEG:
					SetMode(VIEWMODE_SEG);
					break;

				case IDM_VIEWMODE1MB:
					SetMode(VIEWMODE_1MB);
					break;

				case IDM_VIEWMODEASM:
					SetMode(VIEWMODE_ASM);
					break;

				case IDM_VIEWMODESND:
					SetMode(VIEWMODE_SND);
					break;

				default:
					return(viewcmn_dispat(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_PAINT:
			return(viewcmn_dispat(hWnd, msg, wParam, lParam));

		case WM_SIZE:
			{
				RECT rc;
				GetClientRect(hWnd, &rc);
				view->step = (UINT16)(rc.bottom / 16);
				viewcmn_setvscroll(hWnd, view);
			}
			break;

		case WM_VSCROLL:
			{
				UINT32 newpos = view->pos;
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
					Invalidate();
				}
			}
			break;

		case WM_ENTERMENULOOP:
			viewcmn_setmenuseg(hWnd);
			break;

		case WM_ACTIVATE:
			{
				if (LOWORD(wParam) != WA_INACTIVE) {
					view->active = 1;
					Invalidate();
				}
				else {
					view->active = 0;
				}
				vieweractive_renewal();
			}
			break;

		case WM_CLOSE:
			DestroyWindow();
			{
				delete view;
				vieweractive_renewal();
			}
			break;

		default:
			return DefWindowProc(msg, wParam, lParam);
	}
	return(0L);
}

/**
 * ���[�h �ύX
 * @param[in] type �^�C�v
 */
void CDebugUtyView::SetMode(UINT8 type)
{
	if (this->type != type)
	{
		viewcmn_setmode(this, this, type);
		this->dmem.Update();
		viewcmn_setvscroll(hwnd, this);
		Invalidate();
	}
}


// -----------------------------------------------------------------------

BOOL viewer_init(HINSTANCE hInstance)
{
	ZeroMemory(g_np2view, sizeof(g_np2view));

#if 0
	WNDCLASS np2vc;
	np2vc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	np2vc.lpfnWndProc = ViewProc;
	np2vc.cbClsExtra = 0;
	np2vc.cbWndExtra = 0;
	np2vc.hInstance = hInstance;
	np2vc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	np2vc.hCursor = LoadCursor(NULL, IDC_ARROW);
	np2vc.hbrBackground = (HBRUSH)0;
	np2vc.lpszMenuName = MAKEINTRESOURCE(IDR_VIEW);
	np2vc.lpszClassName = np2viewclass;
	if (!RegisterClass(&np2vc)) {
		return(FAILURE);
	}
#endif
	return(SUCCESS);
}


void viewer_term(void) {

}


void viewer_open(HINSTANCE hInstance)
{
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		NP2VIEW_T* view = g_np2view[i];
		if (view == NULL)
		{
			view = new CDebugUtyView;
			g_np2view[i] = view;

			view->Create(NULL,
							WS_OVERLAPPEDWINDOW | WS_VSCROLL,
							CW_USEDEFAULT, CW_USEDEFAULT,
							CW_USEDEFAULT, CW_USEDEFAULT,
							NULL, ::LoadMenu(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(IDR_VIEW)));
			view->hwnd = *view;
			viewcmn_setmode(view, NULL, VIEWMODE_REG);
			view->UpdateCaption();
			view->ShowWindow(SW_SHOWNORMAL);
			view->UpdateWindow();
			break;
		}
	}
}

void viewer_allclose(void)
{
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		NP2VIEW_T* view = g_np2view[i];
		if (view != NULL)
		{
			DestroyWindow(view->hwnd);
			delete view;
		}
	}
	vieweractive_renewal();
}


void viewer_allreload(BOOL force) {

static UINT32	last = 0;

	UINT32 now = GetTickCount();
	if ((force) || ((now - last) >= 200))
	{
		last = now;

		for (size_t i = 0; i < _countof(g_np2view); i++)
		{
			NP2VIEW_T* view = g_np2view[i];
			if ((view != NULL) && (!view->lock))
			{
				if (view->type == VIEWMODE_ASM)
				{
					view->seg = CPU_CS;
					view->off = CPU_IP;
					view->pos = 0;
					viewcmn_setvscroll(view->hwnd, view);
				}
				view->dmem.Update();
				view->Invalidate();
			}
		}
	}
}
