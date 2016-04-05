/**
 * @file	d_network.cpp
 * @brief	Network configure dialog procedure
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

#if defined(SUPPORT_NET)

/**
 * @brief �l�b�g���[�N��{�ݒ�y�[�W
 * @param[in] hwndParent �e�E�B���h�E
 */
class CNetworkPage : public CPropPageProc
{
public:
	CNetworkPage();
	virtual ~CNetworkPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	TCHAR m_tap[300];			//!< TAP��
	CComboData m_cmbtap;		//!< TAP NAME
	void SetNetWorkDeviceNames();
};

/**
 * �R���X�g���N�^
 */
CNetworkPage::CNetworkPage()
	: CPropPageProc(IDD_NETWORK)
{
}
/**
 * �f�X�g���N�^
 */
CNetworkPage::~CNetworkPage()
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CNetworkPage::OnInitDialog()
{
	_tcscpy(m_tap, np2cfg.np2nettap);

	m_cmbtap.SubclassDlgItem(IDC_NETTAP, this);
	SetNetWorkDeviceNames();

	m_cmbtap.SetFocus();

	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CNetworkPage::OnOK()
{
	UINT update = 0;

	if (_tcscmp(np2cfg.np2nettap, m_tap)!=0)
	{
		_tcscpy(np2cfg.np2nettap, m_tap);
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
BOOL CNetworkPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_NETTAP:
			m_cmbtap.GetWindowTextW(m_tap, NELEMENTS(m_tap));
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
LRESULT CNetworkPage::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	//switch (nMsg)
	//{
	//}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * �l�b�g���[�N�f�o�C�X�\�������R���{�{�b�N�X�f�[�^�ɕ��荞��
 * �Q�l����: http://dsas.blog.klab.org/archives/51012690.html
 */
void CNetworkPage::SetNetWorkDeviceNames()
{
	CONST TCHAR *SUBKEY = _T("SYSTEM\\CurrentControlSet\\Control\\Network");
 
#define BUFSZ 256
	int index = 0;
	int indexsel = -1;
	HKEY hKey1, hKey2, hKey3;
	LONG nResult;
	DWORD dwIdx1, dwIdx2;
	TCHAR szData[64], *pKeyName1, *pKeyName2, *pKeyName3, *pKeyName4; 
	DWORD dwSize, dwType = REG_SZ;
	BOOL bDone = FALSE;
	FILETIME ft;

	hKey1 = hKey2 = hKey3 = NULL;
	pKeyName1 = pKeyName2 = pKeyName3 = pKeyName4 = NULL;
 
	// ��L�[�̃I�[�v��
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SUBKEY, 0, KEY_READ, &hKey1);
	if (nResult != ERROR_SUCCESS) {
		return;
	}
	pKeyName1 = (TCHAR*)malloc(sizeof(TCHAR)*BUFSZ);
	pKeyName2 = (TCHAR*)malloc(sizeof(TCHAR)*BUFSZ);
	pKeyName3 = (TCHAR*)malloc(sizeof(TCHAR)*BUFSZ);
	pKeyName4 = (TCHAR*)malloc(sizeof(TCHAR)*BUFSZ);
 
	dwIdx1 = 0;
	while (bDone != TRUE) { // {id1} ��񋓂��郋�[�v
 
	dwSize = BUFSZ;
	nResult = RegEnumKeyEx(hKey1, dwIdx1++, pKeyName1,
							&dwSize, NULL, NULL, NULL, &ft);
	if (nResult == ERROR_NO_MORE_ITEMS) {
		break;
	}
 
	// SUBKEY\{id1} �L�[���I�[�v��
	_stprintf(pKeyName2, _T("%s\\%s"), SUBKEY, pKeyName1);
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pKeyName2,
							0, KEY_READ, &hKey2);
	if (nResult != ERROR_SUCCESS) {
		continue;
	}
		dwIdx2 = 0;
		while (1) { // {id2} ��񋓂��郋�[�v
			dwSize = BUFSZ;
			nResult = RegEnumKeyEx(hKey2, dwIdx2++, pKeyName3,
								&dwSize, NULL, NULL, NULL, &ft);
			if (nResult == ERROR_NO_MORE_ITEMS) {
				break;
			}
 
			if (nResult != ERROR_SUCCESS) {
				continue;
			}
 
			// SUBKEY\{id1}\{id2]\Connection �L�[���I�[�v��
			_stprintf(pKeyName4, _T("%s\\%s\\%s"),
							pKeyName2, pKeyName3, _T("Connection"));
			nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
							pKeyName4, 0, KEY_READ, &hKey3);
			if (nResult != ERROR_SUCCESS) {
				continue;
			}
 
			// SUBKEY\{id1}\{id2]\Connection\PnpInstanceID �l���擾
			dwSize = sizeof(szData);
			nResult = RegQueryValueEx(hKey3, _T("PnpInstanceID"),
							0, &dwType, (LPBYTE)szData, &dwSize);
 
			if (nResult == ERROR_SUCCESS) {
				if(_tcslen(szData)>0){
					// SUBKEY\{id1}\{id2]\Connection\Name �l���擾
					dwSize = sizeof(szData);
					nResult = RegQueryValueEx(hKey3, _T("Name"),
									0, &dwType, (LPBYTE)szData, &dwSize);
 
					if (nResult == ERROR_SUCCESS) {
						if(_tcslen(szData)>0){
							m_cmbtap.Add(szData, index);
							if(_tcscmp(szData, m_tap)==0){
								m_cmbtap.SetCurSel(index);
								indexsel = index;
							}
							index++;
						}
					}
				}
			}
			RegCloseKey(hKey3);
			hKey3 = NULL;
		}
		RegCloseKey(hKey2);
		hKey2 = NULL;
	}
 
	if (hKey1) { RegCloseKey(hKey1); }
	if (hKey2) { RegCloseKey(hKey2); }
	if (hKey3) { RegCloseKey(hKey3); }
 
	if (pKeyName1) { free(pKeyName1); }
	if (pKeyName2) { free(pKeyName2); }
	if (pKeyName3) { free(pKeyName3); }
	if (pKeyName4) { free(pKeyName4); }

	if(indexsel == -1){
		// �J�X�^���ڑ���
		m_cmbtap.SetWindowTextW(m_tap);
	}
 
	return;
}

#if defined(SUPPORT_LGY98)

/**
 * @brief LGY-98 �ݒ�y�[�W
 * @param[in] hwndParent �e�E�B���h�E
 */
class CLgy98Page : public CPropPageProc
{
public:
	CLgy98Page();
	virtual ~CLgy98Page();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	UINT8 m_enabled;			//!< �L���t���O
	UINT8 m_port;				//!< PORT�ݒ�l
	UINT8 m_int;				//!< INT�ݒ�l
	CWndProc m_chkenabled;		//!< ENABLED
	CComboData m_cmbport;		//!< IO
	CComboData m_cmbint;		//!< INT
	CWndProc m_btnreset;		//!< RESET
	UINT8 ConvertIrq2Int(UINT8 cValue);
	UINT8 ConvertInt2Irq(UINT8 cValue);
	void SetPort(UINT8 cValue);
	UINT8 GetPort() const;
	void SetInt(UINT8 cValue);
	UINT8 GetInt() const;
};

/**
 * �|�[�g���X�g
 */
static const CComboData::Entry s_port[] =
{
	{MAKEINTRESOURCE(IDS_00D0),		0x00},
	{MAKEINTRESOURCE(IDS_10D0),		0x10},
	{MAKEINTRESOURCE(IDS_20D0),		0x20},
	{MAKEINTRESOURCE(IDS_30D0),		0x30},
	{MAKEINTRESOURCE(IDS_40D0),		0x40},
	{MAKEINTRESOURCE(IDS_50D0),		0x50},
	{MAKEINTRESOURCE(IDS_60D0),		0x60},
	{MAKEINTRESOURCE(IDS_70D0),		0x70},
};

/**
 * ���荞�݃��X�g
 */
static const CComboData::Entry s_int[] =
{
	{MAKEINTRESOURCE(IDS_INT0),		0},
	{MAKEINTRESOURCE(IDS_INT1),		1},
	{MAKEINTRESOURCE(IDS_INT2),		2},
	{MAKEINTRESOURCE(IDS_INT5),		5},
};

/**
 * �R���X�g���N�^
 */
CLgy98Page::CLgy98Page()
	: CPropPageProc(IDD_LGY98)
	, m_port((UINT8)IDS_10D0), m_int((UINT8)IDS_INT1)
{
}
/**
 * �f�X�g���N�^
 */
CLgy98Page::~CLgy98Page()
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CLgy98Page::OnInitDialog()
{
	m_enabled = np2cfg.uselgy98;
	m_port = (UINT8)(np2cfg.lgy98io>>8);
	m_int = ConvertIrq2Int(np2cfg.lgy98irq);

	m_chkenabled.SubclassDlgItem(IDC_LGY98ENABLED, this);
	if(m_enabled)
		m_chkenabled.SendMessage(BM_SETCHECK , BST_CHECKED , 0);
	else
		m_chkenabled.SendMessage(BM_SETCHECK , BST_UNCHECKED , 0);

	m_cmbport.SubclassDlgItem(IDC_LGY98IO, this);
	m_cmbport.Add(s_port, _countof(s_port));
	SetPort(m_port);
	
	m_cmbint.SubclassDlgItem(IDC_LGY98INT, this);
	m_cmbint.Add(s_int, _countof(s_int));
	SetInt(m_int);
	
	m_cmbport.SetFocus();

	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CLgy98Page::OnOK()
{
	UINT update = 0;

	if (np2cfg.uselgy98 != m_enabled 
		|| (np2cfg.lgy98io>>8) != m_port 
		|| np2cfg.lgy98irq != ConvertInt2Irq(m_int))
	{
		np2cfg.uselgy98 = m_enabled;
		np2cfg.lgy98io = (m_port<<8)|0xD0;
		np2cfg.lgy98irq = ConvertInt2Irq(m_int);
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
BOOL CLgy98Page::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_LGY98ENABLED:
			m_enabled = (m_chkenabled.SendMessage(BM_GETCHECK , 0 , 0) ? 1 : 0);
			return TRUE;

		case IDC_LGY98IO:
			m_port = GetPort();
			return TRUE;

		case IDC_LGY98INT:
			m_int = GetInt();
			return TRUE;
			
		case IDC_LGY98DEF:
			m_port = 0x10;
			m_int = 1;
			SetPort(m_port);
			SetInt(m_int);
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
LRESULT CLgy98Page::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	//switch (nMsg)
	//{
	//}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * IRQ -> INT�ϊ�
 * @return INT
 */
UINT8 CLgy98Page::ConvertIrq2Int(UINT8 cValue) 
{
	switch(cValue){
	case 3:
		return 0;
	case 5:
		return 1;
	case 6:
		return 2;
	case 12:
		return 5;
	}
	return 0;
}
/**
 * INT -> IRQ�ϊ�
 * @return IRQ
 */
UINT8 CLgy98Page::ConvertInt2Irq(UINT8 cValue) 
{
	switch(cValue){
	case 0:
		return 3;
	case 1:
		return 5;
	case 2:
		return 6;
	case 5:
		return 12;
	}
	return 0;
}

/**
 * I/O ��ݒ�
 * @param[in] cValue �ݒ�
 */
void CLgy98Page::SetPort(UINT8 cValue)
{
	m_cmbport.SetCurItemData(cValue);
}

/**
 * I/O ���擾
 * @return I/O
 */
UINT8 CLgy98Page::GetPort() const
{
	return m_cmbport.GetCurItemData(0x01);
}

/**
 * INT ��ݒ�
 * @param[in] cValue �ݒ�
 */
void CLgy98Page::SetInt(UINT8 cValue)
{
	m_cmbint.SetCurItemData(cValue);
}

/**
 * INT ���擾
 * @return INT
 */
UINT8 CLgy98Page::GetInt() const
{
	return m_cmbint.GetCurItemData(0x01);
}

#endif

/**
 * �R���t�B�O �_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
 */
void dialog_netopt(HWND hwndParent)
{
	CPropSheetProc prop(IDS_NETWORKOPTION, hwndParent);
	
	CNetworkPage network;
	prop.AddPage(&network);
	
#if defined(SUPPORT_LGY98)
	CLgy98Page lgy98;
	prop.AddPage(&lgy98);
#endif
	
	prop.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	prop.m_psh.hIcon = LoadIcon(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(IDI_ICON2));
	prop.m_psh.pfnCallback = np2class_propetysheet;
	prop.DoModal();

	InvalidateRect(hwndParent, NULL, TRUE);
}

#endif