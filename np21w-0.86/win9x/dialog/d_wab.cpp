/**
 * @file	d_wab.cpp
 * @brief	Window Accelerator Board configure dialog procedure
 *
 * @author	$Author: SimK $
 * @date	$Date: 2016/03/11 $
 */

#include "compiler.h"
#include "resource.h"
#include "strres.h"
#include "dialog.h"
#include "c_combodata.h"
#include "np2class.h"
#include "dosio.h"
#include "joymng.h"
#include "np2.h"
#include "sysmng.h"
#include "misc\PropProc.h"
#include "pccore.h"
#include "iocore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif


/**
 * @brief CL-GD5430 �ݒ�y�[�W
 * @param[in] hwndParent �e�E�B���h�E
 */
class CGD5430Page : public CPropPageProc
{
public:
	CGD5430Page();
	virtual ~CGD5430Page();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	UINT8 m_enabled;			//!< �L���t���O
	UINT8 m_type;				//!< �@��ݒ�
	CWndProc m_chkenabled;		//!< ENABLED
	CComboData m_cmbtype;		//!< �@��
	CWndProc m_btnreset;		//!< RESET
	void SetWABType(UINT8 cValue);
	UINT8 GetWABType() const;
};

/**
 * �R���X�g���N�^
 */
CGD5430Page::CGD5430Page()
	: CPropPageProc(IDD_GD5430)
{
}
/**
 * �f�X�g���N�^
 */
CGD5430Page::~CGD5430Page()
{
}

/**
 * �@�탊�X�g
 */
static const CComboData::Entry s_type[] =
{
	{MAKEINTRESOURCE(IDS_GD5430_XE),		0x58},
	{MAKEINTRESOURCE(IDS_GD5430_CB),		0x59},
	{MAKEINTRESOURCE(IDS_GD5430_CF),		0x5A},
	{MAKEINTRESOURCE(IDS_GD5430_XE10),		0x5B},
	{MAKEINTRESOURCE(IDS_GD5430_CB2),		0x5C},
	{MAKEINTRESOURCE(IDS_GD5430_CX2),		0x5D},
};

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CGD5430Page::OnInitDialog()
{
	m_enabled = np2cfg.usegd5430;
	m_type = np2cfg.gd5430type;

	m_chkenabled.SubclassDlgItem(IDC_GD5430ENABLED, this);
	if(m_enabled)
		m_chkenabled.SendMessage(BM_SETCHECK , BST_CHECKED , 0);
	else
		m_chkenabled.SendMessage(BM_SETCHECK , BST_UNCHECKED , 0);
	
	m_cmbtype.SubclassDlgItem(IDC_GD5430TYPE, this);
	m_cmbtype.Add(s_type, _countof(s_type));
	SetWABType(m_type);
	
	m_cmbtype.SetFocus();

	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CGD5430Page::OnOK()
{
	UINT update = 0;

	if (np2cfg.usegd5430 != m_enabled
		|| np2cfg.gd5430type != m_type)
	{
		np2cfg.usegd5430 = m_enabled;
		np2cfg.gd5430type = m_type;
		update |= SYS_UPDATECFG;
	}
	::sysmng_update(update);
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL CGD5430Page::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_GD5430ENABLED:
			m_enabled = (m_chkenabled.SendMessage(BM_GETCHECK , 0 , 0) ? 1 : 0);
			return TRUE;

		case IDC_GD5430TYPE:
			m_type = GetWABType();
			return TRUE;
			
		case IDC_GD5430DEF:
			m_type = 0x5B;
			SetWABType(m_type);
			return TRUE;
	}
	return FALSE;
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CGD5430Page::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	//switch (nMsg)
	//{
	//}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * I/O ��ݒ�
 * @param[in] cValue �ݒ�
 */
void CGD5430Page::SetWABType(UINT8 cValue)
{
	m_cmbtype.SetCurItemData(cValue);
}

/**
 * I/O ���擾
 * @return I/O
 */
UINT8 CGD5430Page::GetWABType() const
{
	return m_cmbtype.GetCurItemData(0x5B);
}


/**
 * �R���t�B�O �_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
 */
void dialog_wabopt(HWND hwndParent)
{
	CPropSheetProc prop(IDS_WABOPTION, hwndParent);

	CGD5430Page gd5430;
	prop.AddPage(&gd5430);

	prop.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	prop.m_psh.hIcon = LoadIcon(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(IDI_ICON2));
	prop.m_psh.pfnCallback = np2class_propetysheet;
	prop.DoModal();

	InvalidateRect(hwndParent, NULL, TRUE);
}


//#include "compiler.h"
//#include <commctrl.h>
//#include <prsht.h>
//#include "strres.h"
//#include "resource.h"
//#include "np2.h"
//#include "dosio.h"
//#include "misc\tstring.h"
//#include "joymng.h"
//#include "sysmng.h"
//#include "np2class.h"
//#include "dialog.h"
//#include "dialogs.h"
//#include "pccore.h"
//#include "iocore.h"
//#include "video/video.h"
//
//#if !defined(__GNUC__)
//#pragma comment(lib, "comctl32.lib")
//#endif	// !defined(__GNUC__)
//
//static LRESULT CALLBACK CLGD5430optDlgProc(HWND hWnd, UINT msg,
//													WPARAM wp, LPARAM lp) {
//
//	//HWND	sub;
//	HWND	hItem;
//	//int		cur;
//#if defined(SUPPORT_CL_GD5430)
//
//	switch(msg) {
//		case WM_INITDIALOG:
//			//SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
//			//				(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
//			
//			hItem = GetDlgItem(hWnd, IDC_GD5430ENABLED);
//			if(np2cfg.usegd5430)
//				SendMessage(hItem , BM_SETCHECK , BST_CHECKED , 0);
//			else
//				SendMessage(hItem , BM_SETCHECK , BST_UNCHECKED , 0);
//			return(TRUE);
//
//		case WM_COMMAND:
//			switch(LOWORD(wp)) {
//				case IDC_GD5430ENABLED:
//					hItem = GetDlgItem(hWnd, IDC_GD5430ENABLED);
//					np2cfg.usegd5430 = (SendMessage(hItem , BM_GETCHECK , 0 , 0) ? 1 : 0);
//					break;
//			}
//			break;
//
//		case WM_NOTIFY:
//			break;
//	}
//#endif
//	return(FALSE);
//}
//
//void dialog_wabopt(HWND hWnd)
//{
//	HINSTANCE		hInstance;
//	PROPSHEETPAGE	psp;
//	PROPSHEETHEADER	psh;
//	HPROPSHEETPAGE	hpsp[1];
//	
//#if defined(SUPPORT_LGY98)
//	hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
//
//	ZeroMemory(&psp, sizeof(psp));
//	psp.dwSize = sizeof(PROPSHEETPAGE);
//	psp.dwFlags = PSP_DEFAULT;
//	psp.hInstance = hInstance;
//
//	psp.pszTemplate = MAKEINTRESOURCE(IDD_GD5430);
//	psp.pfnDlgProc = (DLGPROC)CLGD5430optDlgProc;
//	hpsp[0] = CreatePropertySheetPage(&psp);
//
//	std::tstring rTitle(LoadTString(IDS_WABOPTION));
//
//	ZeroMemory(&psh, sizeof(psh));
//	psh.dwSize = sizeof(PROPSHEETHEADER);
//	psh.dwFlags = PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
//	psh.hwndParent = hWnd;
//	psh.hInstance = hInstance;
//	psh.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
//	psh.nPages = 1;
//	psh.phpage = hpsp;
//	psh.pszCaption = rTitle.c_str();
//	psh.pfnCallback = np2class_propetysheet;
//	PropertySheet(&psh);
//	InvalidateRect(hWnd, NULL, TRUE);
//#endif
//}
