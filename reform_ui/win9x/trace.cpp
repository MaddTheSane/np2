/**
 * @file	trace.cpp
 * @brief	�g���[�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include <stdarg.h>
#include "resource.h"
#include "misc\WndProc.h"
#include "strres.h"
#include "textfile.h"
#include "dosio.h"
#include "ini.h"
#include "menu.h"

#ifdef TRACE

#define	VIEW_BUFFERSIZE	4096
#define	VIEW_FGCOLOR	0x000000
#define	VIEW_BGCOLOR	0xffffff
#define	VIEW_TEXT		"�l�r �S�V�b�N"
#define	VIEW_SIZE		12

/**
 * @brief �g���[�X �E�B���h�E �N���X
 */
class CTraceWnd : public CWndProc
{
public:
	static CTraceWnd* GetInstance();

	CTraceWnd();
	void Initialize();
	void Deinitialize();
	bool IsTrace() const;
	bool IsVerbose() const;
	bool IsEnabled() const;
	void AddString(LPCTSTR lpString);

protected:
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSysCommand(UINT nID, LPARAM lParam);
	void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);

private:
	static CTraceWnd sm_instance;		/*!< �B��̃C���X�^���X�ł� */
	UINT8 m_nFlags;						/*!< �t���O */
	TEXTFILEH m_tfh;					/*!< �e�L�X�g �t�@�C�� �n���h�� */
	HBRUSH m_hBrush;					/*!< �u���V */
	HFONT m_hFont;						/*!< �t�H���g */
	CWndProc m_wndView;					/*!< �e�L�X�g �R���g���[�� */

private:
	int viewpos;
	int viewleng;
	TCHAR viewbuf[VIEW_BUFFERSIZE * 2];
	void View_ScrollToBottom();
	void View_ClrString();
	void View_AddString(const OEMCHAR *string);
};

struct TRACECFG
{
	int		posx;
	int		posy;
	int		width;
	int		height;
};

static const TCHAR s_szTitle[] = TEXT("console");
static const TCHAR s_szClassName[] = TEXT("TRACE-console");
static const TCHAR s_szViewFont[] = TEXT(VIEW_TEXT);

static const OEMCHAR crlf[] = OEMTEXT("\r\n");

static	TRACECFG	tracecfg;
static	int			devpos;
static	char		devstr[256];

static const OEMCHAR np2trace[] = OEMTEXT("np2trace.ini");
static const OEMCHAR inititle[] = OEMTEXT("TRACE");
static const PFTBL initbl[4] =
{
	PFVAL("posx",	PFTYPE_SINT32,	&tracecfg.posx),
	PFVAL("posy",	PFTYPE_SINT32,	&tracecfg.posy),
	PFVAL("width",	PFTYPE_SINT32,	&tracecfg.width),
	PFVAL("height",	PFTYPE_SINT32,	&tracecfg.height)
};


// ---- View

void CTraceWnd::View_ScrollToBottom()
{
	int MinPos;
	int MaxPos;

	GetScrollRange(m_wndView, SB_VERT, &MinPos, &MaxPos);
	m_wndView.PostMessage(EM_LINESCROLL, 0, MaxPos);
}

void CTraceWnd::View_ClrString(void)
{
	viewpos = 0;
	viewleng = 0;
	viewbuf[0] = '\0';
	m_wndView.SetWindowText(viewbuf);
}

void CTraceWnd::View_AddString(const OEMCHAR *string)
{
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
	m_wndView.SetWindowText(viewbuf + vpos);
	View_ScrollToBottom();
}


// ----

CTraceWnd CTraceWnd::sm_instance;

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline CTraceWnd* CTraceWnd::GetInstance()
{
	return &sm_instance;
}

/**
 * �R���X�g���N�^
 */
CTraceWnd::CTraceWnd()
	: m_nFlags(0)
	, m_tfh(NULL)
	, m_hBrush(NULL)
	, m_hFont(NULL)
{
}

/**
 * Trace �͗L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool CTraceWnd::IsTrace() const
{
	return ((m_nFlags & 1) != 0);
}

/**
 * Verbose �͗L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool CTraceWnd::IsVerbose() const
{
	return ((m_nFlags & 2) != 0);
}

/**
 * �L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool CTraceWnd::IsEnabled() const
{
	return ((m_nFlags & 4) || (m_tfh != NULL));
}

/**
 * ���O�ǉ�
 * @param[in] lpString ������
 */
void CTraceWnd::AddString(LPCTSTR lpString)
{
	if ((m_nFlags & 4) && (m_wndView.IsWindow()))
	{
		View_AddString(lpString);
	}
	if (m_tfh != NULL)
	{
		textfile_write(m_tfh, lpString);
		textfile_write(m_tfh, crlf);
	}
}

/**
 * �t���[�����[�N�́AWindows �̃E�B���h�E�� [�쐬] �܂��� CreateEx �̃����o�[�֐����Ăяo�����Ƃɂ���č쐬���ꂽ�A�v���P�[�V�������K�v�Ƃ���ƁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] lpCreateStruct �쐬���ꂽ�I�u�W�F�N�g�Ɋւ����񂪊܂܂�Ă��܂��B
 * @retval 0 ����
 * @retval -1 ���s
 */
int CTraceWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HMENU hMenu = GetSystemMenu(FALSE);
	menu_addmenures(hMenu, 0, IDR_TRACE, FALSE);

	m_hBrush = ::CreateSolidBrush(VIEW_BGCOLOR);

	RECT rc;
	GetClientRect(&rc);
	if (m_wndView.CreateEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | ES_READONLY | ES_LEFT | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL, 0, 0, rc.right, rc.bottom, m_hWnd, NULL))
	{
		m_wndView.SendMessage(EM_SETLIMITTEXT, VIEW_BUFFERSIZE, 0);

		m_hFont = ::CreateFont(VIEW_SIZE, 0, 0, 0, 0, 0, 0, 0,  SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, s_szViewFont);
		if (m_hFont)
		{
			m_wndView.SetFont(m_hFont);
		}
		m_wndView.SetFocus();
	}

	return 0;
}

/**
 * �t���[�����[�N�́A���[�U�[���R���g���[�� ���j���[����R�}���h��I�������Ƃ��A�܂��͍ő剻�܂��͍ŏ����{�^����I������ƁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] nID �K�v�ȃV�X�e�� �R�}���h�̎�ނ��w�肵�܂�
 * @param[in] lParam �J�[�\���̍��W
 */
void CTraceWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID)
	{
		case IDM_TRACE_TRACE:
			m_nFlags ^= 1;
			break;

		case IDM_TRACE_VERBOSE:
			m_nFlags ^= 2;
			break;

		case IDM_TRACE_ENABLE:
			m_nFlags ^= 4;
			break;

		case IDM_TRACE_FILEOUT:
			if (m_tfh != NULL)
			{
				textfile_close(m_tfh);
				m_tfh = NULL;
			}
			else
			{
				m_tfh = textfile_create(OEMTEXT("traceout.txt"), 0x800);
			}
			break;

		case IDM_TRACE_CLEAR:
			View_ClrString();
			break;
	}
}

/**
 * Windows �v���V�[�W�� (WindowProc) ��񋟂��܂�
 * @param[in] message ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ɏg�p����ǉ�����񋟂��܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ɏg�p����ǉ�����񋟂��܂�
 * @return �߂�l�́A���b�Z�[�W�ɂ���ĈقȂ�܂�
 */
LRESULT CTraceWnd::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_CREATE:
			return OnCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));

		case WM_SYSCOMMAND:
			OnSysCommand(wParam, lParam);
			return DefWindowProc(nMsg, wParam, lParam);

		case WM_ENTERMENULOOP:
			OnEnterMenuLoop(wParam);
			break;

		case WM_MOVE:
			if (!(GetStyle() & (WS_MAXIMIZE | WS_MINIMIZE)))
			{
				RECT rc;
				GetWindowRect(&rc);
				tracecfg.posx = rc.left;
				tracecfg.posy = rc.top;
			}
			break;

		case WM_SIZE:							// window resize
			if (!(GetStyle() & (WS_MAXIMIZE | WS_MINIMIZE)))
			{
				RECT rc;
				GetWindowRect(&rc);
				tracecfg.width = rc.right - rc.left;
				tracecfg.height = rc.bottom - rc.top;
			}
			m_wndView.MoveWindow(0, 0, LOWORD(lParam), HIWORD(lParam));
			View_ScrollToBottom();
			break;

		case WM_SETFOCUS:
			m_wndView.SetFocus();
			break;

		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
			SetTextColor((HDC)wParam, VIEW_FGCOLOR);
			SetBkColor((HDC)wParam, VIEW_BGCOLOR);
			return reinterpret_cast<LRESULT>(m_hBrush);

		case WM_CLOSE:
			break;

		case WM_DESTROY:
			if (m_hBrush)
			{
				::DeleteObject(m_hBrush);
				m_hBrush = NULL;
			}
			if (m_hFont)
			{
				::DeleteObject(m_hFont);
				m_hFont = NULL;
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
			break;
#endif
		default:
			return DefWindowProc(nMsg, wParam, lParam);
	}
	return FALSE;
}

/**
 * �t���[�����[�N�́A���j���[ ���[�v�J�n���ɁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] bIsTrackPopupMenu TrackPopupMenu �֐��𗘗p�����ꍇ TRUE
 */
void CTraceWnd::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	HMENU hMenu = GetSystemMenu(FALSE);
	::CheckMenuItem(hMenu, IDM_TRACE_TRACE, (m_nFlags & 1) ? MF_CHECKED : MF_UNCHECKED);
	::CheckMenuItem(hMenu, IDM_TRACE_VERBOSE, (m_nFlags & 2) ? MF_CHECKED : MF_UNCHECKED);
	::CheckMenuItem(hMenu, IDM_TRACE_ENABLE, (m_nFlags & 4) ? MF_CHECKED:MF_UNCHECKED);
	::CheckMenuItem(hMenu, IDM_TRACE_FILEOUT, (m_tfh != NULL) ? MF_CHECKED : MF_UNCHECKED);
}



// ----

void trace_init(void)
{
	CTraceWnd::GetInstance()->Initialize();
}

/**
 * ������
 */
void CTraceWnd::Initialize()
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hInstance = GetInstanceHandle();
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wc.lpszClassName = s_szClassName;
	if (!::RegisterClass(&wc))
	{
		return;
	}

#if 1
	m_nFlags = 4;
#else
	m_nFlags = 1;
	m_tfh = textfile_create(OEMTEXT("traceout.txt"), 0x800);
#endif

	tracecfg.posx = CW_USEDEFAULT;
	tracecfg.posy = CW_USEDEFAULT;
	tracecfg.width = CW_USEDEFAULT;
	tracecfg.height = CW_USEDEFAULT;
	ini_read(file_getcd(np2trace), inititle, initbl, NELEMENTS(initbl));

	if (!CreateEx(WS_EX_CONTROLPARENT, s_szClassName, s_szTitle, WS_OVERLAPPEDWINDOW, tracecfg.posx, tracecfg.posy, tracecfg.width, tracecfg.height, NULL, NULL))
	{
		return;
	}
	ShowWindow(SW_SHOW);
	UpdateWindow();
}

void trace_term(void)
{
	CTraceWnd::GetInstance()->Deinitialize();
}

/**
 * ���
 */
void CTraceWnd::Deinitialize()
{
	if (m_tfh != NULL)
	{
		textfile_close(m_tfh);
		m_tfh = NULL;
	}

	DestroyWindow();
	ini_write(file_getcd(np2trace), inititle, initbl, NELEMENTS(initbl));
}

void trace_fmt(const char *fmt, ...)
{
	CTraceWnd* pWnd = CTraceWnd::GetInstance();

	if ((pWnd->IsTrace()) && (pWnd->IsEnabled()))
	{
		va_list ap;
		va_start(ap, fmt);
#if defined(OSLANG_UCS2)
		OEMCHAR cnvfmt[0x800];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fmt, -1, cnvfmt, NELEMENTS(cnvfmt));
		OEMCHAR buf[0x1000];
		vswprintf(buf, cnvfmt, ap);
#else
		OEMCHAR buf[0x1000];
		vsprintf(buf, fmt, ap);
#endif
		va_end(ap);
		pWnd->AddString(buf);
	}
}

void trace_fmt2(const char *fmt, ...)
{
	CTraceWnd* pWnd = CTraceWnd::GetInstance();

	if ((pWnd->IsVerbose()) && (pWnd->IsEnabled()))
	{
		va_list ap;
		va_start(ap, fmt);
#if defined(OSLANG_UCS2)
		OEMCHAR cnvfmt[0x800];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fmt, -1, cnvfmt, NELEMENTS(cnvfmt));

		OEMCHAR buf[0x1000];
		vswprintf(buf, cnvfmt, ap);
#else
		OEMCHAR buf[0x1000];
		vsprintf(buf, fmt, ap);
#endif
		va_end(ap);
		pWnd->AddString(buf);
	}
}

void trace_char(char c)
{
	if ((c == 0x0a) || (c == 0x0d))
	{
		if (devpos)
		{
			devstr[devpos] = '\0';
#if defined(OSLANG_UCS2)
			TCHAR pdevstr[0x800];
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, devstr, -1, pdevstr, NELEMENTS(pdevstr));
#else
			const OEMCHAR *pdevstr = devstr;
#endif
			CTraceWnd::GetInstance()->AddString(pdevstr);
			devpos = 0;
		}
	}
	else
	{
		if (devpos < (sizeof(devstr) - 1))
		{
			devstr[devpos++] = c;
		}
	}
}

#endif
