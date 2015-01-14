/**
 * @file	WndProc.cpp
 * @brief	�v���V�[�W�� �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "WndProc.h"
#include "..\resource.h"

//! �C���X�^���X
HINSTANCE CWndProc::sm_hInstance;
//! ���\�[�X
HINSTANCE CWndProc::sm_hResource;

//! �N���X��
static const TCHAR s_szClassName[] = TEXT("WndProc");

/**
 * ������
 * @param[in] hInstance �C���X�^���X
 */
void CWndProc::Initialize(HINSTANCE hInstance)
{
	sm_hInstance = hInstance;
	sm_hResource = hInstance;

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(NULL_BRUSH));
	wc.lpszClassName = s_szClassName;
	::RegisterClass(&wc);
}

/**
 * �R���X�g���N�^
 */
CWndProc::CWndProc()
	: CWndBase(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CWndProc::~CWndProc()
{
	DestroyWindow();
}

/**
 * dwExStyle �Ŏw�肵���g���X�^�C���ŁA�I�[�o�[���b�v �E�B���h�E�A�|�b�v�A�b�v �E�B���h�E�A�܂��͎q�E�B���h�E���쐬���܂�
 * @param[in] lpszWindowName �E�B���h�E�������� NULL �ŏI��镶����ւ̃|�C���^
 * @param[in] dwStyle �E�B���h�E�̃X�^�C���������w�肵�܂�
 * @param[in] x CWndProc �E�B���h�E�̏��� x �ʒu���w�肵�܂�
 * @param[in] y CWndProc �E�B���h�E�̏��� y �ʒu���w�肵�܂�
 * @param[in] nWidth CWndProc �E�B���h�E�̕��� (�f�o�C�X�P�ʂ�) �w�肵�܂�
 * @param[in] nHeight CWndProc �E�B���h�E�̍����� (�f�o�C�X�P�ʂ�) �w�肵�܂�
 * @param[in] hwndParent �쐬����� CWnd �E�B���h�E�̐e�E�B���h�E�܂��̓I�[�i�[ �E�B���h�E���w�肵�܂��BNULL ���w�肷��ƁA�g�b�v���x���̃E�B���h�E�ɂȂ�܂��B
 * @param[in] nIDorHMenu ���j���[�܂��͎q���j���[�̎��ʎq���w�肵�܂��B�E�B���h�E�̃X�^�C���ɂ���ĈӖ����قȂ�܂�
 * @return ����I�������ꍇ�� 0 �ȊO��Ԃ��܂��B����ȊO�̏ꍇ�� 0 ��Ԃ��܂�
 */
BOOL CWndProc::Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	const HWND hWnd = ::CreateWindow(s_szClassName, lpszWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu, sm_hInstance, this);
	return (hWnd != NULL) ? TRUE : FALSE;
}

/**
 * CWndProc �I�u�W�F�N�g�Ɋ֘A�t����ꂽ Windows �̃E�B���h�E��j�����܂�
 * @return �E�B���h�E���j�����ꂽ�ꍇ�� 0 �ȊO��Ԃ��܂��B����ȊO�̏ꍇ�� 0 ��Ԃ��܂�
 */
BOOL CWndProc::DestroyWindow()
{
	if (!m_hWnd)
	{
		return FALSE;
	}
	return ::DestroyWindow(m_hWnd);
}

/**
 * �E�B���h�E �v���V�[�W��
 * @param[in] hWnd �E�B���h�E �n���h��
 * @param[in] message ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CALLBACK CWndProc::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CWndProc* pWnd = NULL;
	if (message == WM_CREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pWnd = static_cast<CWndProc*>(pCreate->lpCreateParams);
		pWnd->m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
	}
	else
	{
		pWnd = reinterpret_cast<CWndProc*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pWnd == NULL)
	{
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	LRESULT lResult = pWnd->WindowProc(message, wParam, lParam);

	if (message == WM_NCDESTROY)
	{
		pWnd->m_hWnd = NULL;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
	}

	return lResult;
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] message ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CWndProc::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(message, wParam, lParam);
}

/**
 * ����̃E�B���h�E �v���V�[�W�����Ăяo���܂�
 * @param[in] message ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�ˑ��̒ǉ������w�肵�܂�
 * @param[in] lParam ���b�Z�[�W�ˑ��̒ǉ������w�肵�܂�
 * @return ����ꂽ���b�Z�[�W�Ɉˑ����܂�
 */
LRESULT CWndProc::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(m_hWnd, message, wParam, lParam);
}
