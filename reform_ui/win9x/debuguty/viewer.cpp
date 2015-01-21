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

static	const TCHAR		np2viewclass[] = TEXT("NP2-ViewWindow");
		const TCHAR		np2viewfont[] = _T("�l�r �S�V�b�N");
static CDebugUtyView* g_np2view[NP2VIEW_MAX];

//! �`�F�b�N �}�N��
#define MFCHECK(bChecked) ((bChecked) ? MF_CHECKED : MF_UNCHECKED)

static void vieweractive_renewal(void);

/**
 * �R���X�g���N�^
 */
CDebugUtyView::CDebugUtyView()
	: m_nVPos(0)
	, m_nVLines(0)
	, m_nVPage(0)
	, m_nVMultiple(1)
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
			vieweractive_renewal();
			break;
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
			nIndex = static_cast<int>(i);
			break;
		}
	}
	LPCTSTR lpMode = (this->lock) ? TEXT("Locked") : TEXT("Realtime");

	TCHAR szTitle[256];
	wsprintf(szTitle, TEXT("%d.%s - NP2 Debug Utility"), nIndex + 1, lpMode);

	SetWindowText(szTitle);
}

/**
 * V �X�N���[���ʒu�̐ݒ�
 * @param[in] nPos �V�����ʒu
 */
void CDebugUtyView::SetVScrollPos(UINT nPos)
{
	if (m_nVPos != nPos)
	{
		m_nVPos = nPos;
		UpdateVScroll();
		Invalidate();
	}
}

/**
 * V �X�N���[���̐ݒ�
 * @param[in] nPos �V�����ʒu
 * @param[in] nLines ���C����
 */
void CDebugUtyView::SetVScroll(UINT nPos, UINT nLines)
{
	if ((m_nVPos != nPos) || (m_nVLines != nLines))
	{
		m_nVPos = nPos;
		m_nVLines = nLines;
		m_nVMultiple = ((nLines - 1) / 0xFFFF) + 1;
		UpdateVScroll();
		Invalidate();
	}
}

/**
 * V �X�N���[���o�[�̍X�V
 */
void CDebugUtyView::UpdateVScroll()
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = ((m_nVLines + m_nVMultiple - 1) / m_nVMultiple) - 1;
	si.nPos = m_nVPos / m_nVMultiple;
	si.nPage = m_nVPage / m_nVMultiple;
	SetScrollInfo(SB_VERT, &si, TRUE);
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
			if (!OnCommand(wParam, lParam))
			{
				return viewcmn_dispat(this, msg, wParam, lParam);
			}
			break;

		case WM_PAINT:
			return viewcmn_dispat(this, msg, wParam, lParam);

		case WM_SIZE:
			{
				RECT rc;
				GetClientRect(&rc);
				m_nVPage = rc.bottom / 16;
				UpdateVScroll();
			}
			break;

		case WM_VSCROLL:
			OnVScroll(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));
			break;

		case WM_ENTERMENULOOP:
			OnEnterMenuLoop(static_cast<BOOL>(wParam));
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
			break;

		default:
			return CWndProc::WindowProc(msg, wParam, lParam);
	}
	return(0L);
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL CDebugUtyView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDM_VIEWWINNEW:
			viewer_open(g_hInstance);
			break;

		case IDM_VIEWWINCLOSE:
			DestroyWindow();
			break;

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
			return FALSE;
	}
	return TRUE;
}

/**
 * �t���[�����[�N�́A���[�U�[���E�B���h�E�ɐ����X�N���[�� �o�[���N���b�N����ƁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] nSBCode���[�U�[�̗v���������X�N���[�� �o�[ �R�[�h���w�肵�܂�
 * @param[in] nPos ���݂̃X�N���[�� �{�b�N�X�̈ʒu
 * @param[in] hwndScrollBar �X�N���[�� �o�[ �R���g���[��
 */
void CDebugUtyView::OnVScroll(UINT nSBCode, UINT nPos, HWND hwndScrollBar)
{
	UINT32 nNewPos = m_nVPos;
	switch (nSBCode)
	{
		case SB_LINEUP:
			if (nNewPos)
			{
				nNewPos--;
			}
			break;

		case SB_LINEDOWN:
			if ((nNewPos + m_nVPage) < m_nVLines)
			{
				nNewPos++;
			}
			break;

		case SB_PAGEUP:
			if (nNewPos > m_nVPage)
			{
				nNewPos -= m_nVPage;
			}
			else
			{
				nNewPos = 0;
			}
			break;

		case SB_PAGEDOWN:
			nNewPos += m_nVPage;
			if (nNewPos > (m_nVLines - m_nVPage))
			{
				nNewPos = m_nVLines - m_nVPage;
			}
			break;

		case SB_THUMBTRACK:
			nNewPos = nPos * m_nVMultiple;
			break;
	}
	SetVScrollPos(nNewPos);
}

/**
 * The framework calls this member function when a menu modal loop has been entered
 * @param[in] bIsTrackPopupMenu Specifies whether the menu involved is a popup menu
 */
void CDebugUtyView::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	HMENU hMenu = GetMenu();
	if (hMenu == NULL)
	{
		return;
	}
	::CheckMenuItem(hMenu, IDM_VIEWMODELOCK, MF_BYCOMMAND | MFCHECK(this->lock));
	::CheckMenuItem(hMenu, IDM_VIEWMODEREG, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_REG));
	::CheckMenuItem(hMenu, IDM_VIEWMODESEG, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_SEG));
	::CheckMenuItem(hMenu, IDM_VIEWMODE1MB, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_1MB));
	::CheckMenuItem(hMenu, IDM_VIEWMODEASM, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_ASM));
	::CheckMenuItem(hMenu, IDM_VIEWMODESND, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_SND));

	HMENU hSubMenu = ::GetSubMenu(hMenu, 2);
	if (hSubMenu)
	{
		SetSegmentItem(hSubMenu, IDM_SEGCS, TEXT("CS"), CPU_CS);
		SetSegmentItem(hSubMenu, IDM_SEGDS, TEXT("DS"), CPU_DS);
		SetSegmentItem(hSubMenu, IDM_SEGES, TEXT("ES"), CPU_ES);
		SetSegmentItem(hSubMenu, IDM_SEGSS, TEXT("SS"), CPU_SS);
		DrawMenuBar();
	}
}

/**
 * Called by the default OnNcDestroy member function after the window has been destroyed.
 */
void CDebugUtyView::PostNcDestroy()
{
	delete this;
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
		UpdateVScroll();
		Invalidate();
	}
}

/**
 * ���j���[ �A�C�e�����X�V
 * @param[in] hMenu ���j���[ �n���h��
 * @param[in] hId ���j���[ ID
 * @param[in] lpSegment �Z�O�����g��
 * @param[in] nSegment �Z�O�����g�l
 */
void CDebugUtyView::SetSegmentItem(HMENU hMenu, int nId, LPCTSTR lpSegment, UINT nSegment)
{
	TCHAR szString[32];
	wsprintf(szString, _T("Seg = &%s [%04x]"), lpSegment, nSegment);
	::ModifyMenu(hMenu, nId, MF_BYCOMMAND | MF_STRING, nId, szString);
}

// -----------------------------------------------------------------------

BOOL viewer_init(HINSTANCE hInstance)
{
	ZeroMemory(g_np2view, sizeof(g_np2view));

	WNDCLASS np2vc;
	np2vc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	np2vc.lpfnWndProc = DefWindowProc;
	np2vc.cbClsExtra = 0;
	np2vc.cbWndExtra = 0;
	np2vc.hInstance = hInstance;
	np2vc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	np2vc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	np2vc.hbrBackground = static_cast<HBRUSH>(NULL);
	np2vc.lpszMenuName = MAKEINTRESOURCE(IDR_VIEW);
	np2vc.lpszClassName = np2viewclass;
	if (!RegisterClass(&np2vc))
	{
		return FAILURE;
	}
	return SUCCESS;
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
			if (view->CreateEx(0, np2viewclass, NULL,
							WS_OVERLAPPEDWINDOW | WS_VSCROLL,
							CW_USEDEFAULT, CW_USEDEFAULT,
							CW_USEDEFAULT, CW_USEDEFAULT,
							NULL, 0))
			{
				g_np2view[i] = view;
				viewcmn_setmode(view, NULL, VIEWMODE_REG);
				view->UpdateCaption();
				view->ShowWindow(SW_SHOWNORMAL);
				view->UpdateWindow();
			}
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
			view->DestroyWindow();
		}
	}
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
					view->m_nVPos = 0;
					view->UpdateVScroll();
				}
				view->dmem.Update();
				view->Invalidate();
			}
		}
	}
}
