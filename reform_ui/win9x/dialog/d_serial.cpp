/**
 * @file	d_serial.cpp
 * @brief	Serial configure dialog procedure
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include <vector>
#include "c_combodata.h"
#include "c_dipsw.h"
#include "dialogs.h"
#include "np2class.h"
#include "commng.h"
#include "dosio.h"
#include "np2.h"
#include "sysmng.h"
#include "misc\PropProc.h"
#include "misc\tstring.h"
#include "pccore.h"
#include "iocore.h"
#include "cbus\pc9861k.h"
#include "common\strres.h"
#include "generic\dipswbmp.h"

#if !defined(__GNUC__)
#pragma comment(lib, "comctl32.lib")
#endif	// !defined(__GNUC__)

static const CBPARAM cpPort[] =
{
	{MAKEINTRESOURCE(IDS_NONCONNECT),	COMPORT_NONE},
	{MAKEINTRESOURCE(IDS_COM1),			COMPORT_COM1},
	{MAKEINTRESOURCE(IDS_COM2),			COMPORT_COM2},
	{MAKEINTRESOURCE(IDS_COM3),			COMPORT_COM3},
	{MAKEINTRESOURCE(IDS_COM4),			COMPORT_COM4},
	{MAKEINTRESOURCE(IDS_MIDI),			COMPORT_MIDI},
};

static const CBNPARAM cpChars[] =
{
	{5,	0x00},
	{6,	0x04},
	{7,	0x08},
	{8,	0x0c},
};

static const CBPARAM cpParity[] =
{
    {MAKEINTRESOURCE(IDS_PARITY_NONE),	0x00},
    {MAKEINTRESOURCE(IDS_PARITY_ODD),	0x20},
	{MAKEINTRESOURCE(IDS_PARITY_EVEN),	0x30},
};

static const CBPARAM cpSBit[] =
{
    {MAKEINTRESOURCE(IDS_1),			0x40},
    {MAKEINTRESOURCE(IDS_1HALF),		0x80},
	{MAKEINTRESOURCE(IDS_2),			0xc0},
};


#ifdef __cplusplus
extern "C" {
#endif
extern	COMMNG	cm_rs232c;
extern	COMMNG	cm_pc9861ch1;
extern	COMMNG	cm_pc9861ch2;
#ifdef __cplusplus
}
#endif


enum {
	ID_PORT		= 0,

	ID_SPEED,
	ID_CHARS,
	ID_PARITY,
	ID_SBIT,

	ID_MMAP,
	ID_MMDL,
	ID_DEFE,
	ID_DEFF,
	ID_DEFB,

	ID_MAX
};

typedef struct {
	UINT16		idc[ID_MAX];
	COMMNG		*cm;
	COMCFG		*cfg;
const UINT16	*com_item;
const UINT16	*midi_item;
	UINT		update;
} DLGCOM_P;


static const UINT16 com1serial[8] = {
			IDC_COM1STR00, IDC_COM1STR01, IDC_COM1STR02, IDC_COM1STR03,
			IDC_COM1STR04, IDC_COM1STR05, IDC_COM1STR06, IDC_COM1STR07};

static const UINT16 com1rsmidi[3] = {
			IDC_COM1STR10, IDC_COM1STR11, IDC_COM1STR12};

static const DLGCOM_P res_com1 =
		{{	IDC_COM1PORT,
			IDC_COM1SPEED, IDC_COM1CHARSIZE, IDC_COM1PARITY, IDC_COM1STOPBIT,
			IDC_COM1MMAP, IDC_COM1MMDL,
			IDC_COM1DEFE, IDC_COM1DEFF, IDC_COM1DEFB},
			&cm_rs232c, &np2oscfg.com1,
			com1serial, com1rsmidi, SYS_UPDATESERIAL1};


static const UINT16 com2serial[8] = {
			IDC_COM2STR00, IDC_COM2STR01, IDC_COM2STR02, IDC_COM2STR03,
			IDC_COM2STR04, IDC_COM2STR05, IDC_COM2STR06, IDC_COM2STR07};

static const UINT16 com2rsmidi[3] = {
			IDC_COM2STR10, IDC_COM2STR11, IDC_COM2STR12};

static const DLGCOM_P res_com2 =
		{{	IDC_COM2PORT,
			IDC_COM2SPEED, IDC_COM2CHARSIZE, IDC_COM2PARITY, IDC_COM2STOPBIT,
			IDC_COM2MMAP, IDC_COM2MMDL,
			IDC_COM2DEFE, IDC_COM2DEFF, IDC_COM2DEFB},
			&cm_pc9861ch1, &np2oscfg.com2,
			com2serial, com2rsmidi, SYS_UPDATESERIAL1};


static const UINT16 com3serial[8] = {
			IDC_COM3STR00, IDC_COM3STR01, IDC_COM3STR02, IDC_COM3STR03,
			IDC_COM3STR04, IDC_COM3STR05, IDC_COM3STR06, IDC_COM3STR07};

static const UINT16 com3rsmidi[3] = {
			IDC_COM3STR10, IDC_COM3STR11, IDC_COM3STR12};

static const DLGCOM_P res_com3 =
		{{	IDC_COM3PORT,
			IDC_COM3SPEED, IDC_COM3CHARSIZE, IDC_COM3PARITY, IDC_COM3STOPBIT,
			IDC_COM3MMAP, IDC_COM3MMDL,
			IDC_COM3DEFE, IDC_COM3DEFF, IDC_COM3DEFB},
			&cm_pc9861ch2, &np2oscfg.com3,
			com3serial, com3rsmidi, SYS_UPDATESERIAL1};


static void dlgcom_show(HWND hWnd, int ncmd, const UINT16 *item, int items) {

	while(items--) {
		ShowWindow(GetDlgItem(hWnd, *item++), ncmd);
	}
}

static void dlgcom_items(HWND hWnd, const DLGCOM_P *m, UINT r) {

	int		ncmd;

	ncmd = (((r >= 1) && (r <= 4))?SW_SHOW:SW_HIDE);
	dlgcom_show(hWnd, ncmd, m->com_item, 8);
	dlgcom_show(hWnd, ncmd, m->idc + 1, 4);

	ncmd = ((r == 5)?SW_SHOW:SW_HIDE);
	dlgcom_show(hWnd, ncmd, m->midi_item, 3);
	dlgcom_show(hWnd, ncmd, m->idc + 5, 5);
}


static void setChars(HWND hWnd, UINT uID, UINT8 cValue)
{
	dlgs_setcbcur(hWnd, uID, cValue & 0x0c);
}

static UINT8 getChars(HWND hWnd, UINT uID, UINT8 cDefault)
{
	return dlgs_getcbcur(hWnd, uID, cDefault & 0x0c);
}


static void setParity(HWND hWnd, UINT uID, UINT8 cValue)
{
	cValue = cValue & 0x30;
	if (!(cValue & 0x20))
	{
		cValue = 0;
	}
	dlgs_setcbcur(hWnd, uID, cValue);
}

static UINT8 getParity(HWND hWnd, UINT uID, UINT8 cDefault)
{
	return dlgs_getcbcur(hWnd, uID, cDefault & 0x30);
}


static void setStopBit(HWND hWnd, UINT uID, UINT8 cValue)
{
	cValue = cValue & 0xc0;
	if (!cValue)
	{
		cValue = 0x40;
	}
	dlgs_setcbcur(hWnd, uID, cValue);
}

static UINT8 getStopBit(HWND hWnd, UINT uID, UINT8 cDefault)
{
	return dlgs_getcbcur(hWnd, uID, cDefault & 0xc0);
}






static LRESULT CALLBACK dlgitem_proc(HWND hWnd, UINT msg,
								WPARAM wp, LPARAM lp, const DLGCOM_P *m) {

	UINT32	d;
	UINT8	b;
	LRESULT	r;
	union {
		TCHAR	mmap[MAXPNAMELEN];
		TCHAR	mmdl[64];
		TCHAR	mdef[MAX_PATH];
	} str;
	COMCFG	*cfg;
	UINT	update;
	COMMNG	cm;

	switch (msg) {
		case WM_INITDIALOG:
			cfg = m->cfg;
			dlgs_setcbitem(hWnd, m->idc[ID_PORT], cpPort, NELEMENTS(cpPort));
			SETLISTUINT32(hWnd, m->idc[ID_SPEED], cmserial_speed);
			dlgs_setcbnumber(hWnd, m->idc[ID_CHARS],
											cpChars, NELEMENTS(cpChars));
			dlgs_setcbitem(hWnd, m->idc[ID_PARITY],
											cpParity, NELEMENTS(cpParity));
			dlgs_setcbitem(hWnd, m->idc[ID_SBIT], cpSBit, NELEMENTS(cpSBit));
			for (d=0; d<(NELEMENTS(cmserial_speed) - 1); d++) {
				if (cmserial_speed[d] >= cfg->speed) {
					break;
				}
			}
			SendDlgItemMessage(hWnd, m->idc[ID_SPEED],
										CB_SETCURSEL, (WPARAM)d, (LPARAM)0);

			b = cfg->param;
			d = (b >> 2) & 3;

			setChars(hWnd, m->idc[ID_CHARS], b);
			setParity(hWnd, m->idc[ID_PARITY], b);
			setStopBit(hWnd, m->idc[ID_SBIT], b);

			dlgs_setlistmidiout(hWnd, m->idc[ID_MMAP], cfg->mout);
			SETLISTSTR(hWnd, m->idc[ID_MMDL], cmmidi_mdlname);
			SetDlgItemText(hWnd, m->idc[ID_MMDL], cfg->mdl);
			SetDlgItemCheck(hWnd, m->idc[ID_DEFE], cfg->def_en);
			SetDlgItemText(hWnd, m->idc[ID_DEFF], cfg->def);

			d = cfg->port;
			if (d >= NELEMENTS(cpPort))
			{
				d = 0;
			}
			dlgs_setcbcur(hWnd, m->idc[ID_PORT], d);
			dlgcom_items(hWnd, m, d);
			return(TRUE);

		case WM_COMMAND:
			if (LOWORD(wp) == m->idc[ID_PORT])
			{
				dlgcom_items(hWnd, m,
						dlgs_getcbcur(hWnd, m->idc[ID_PORT], COMPORT_NONE));
			}
			else if (LOWORD(wp) == m->idc[ID_DEFB])
			{
				dlgs_browsemimpidef(hWnd, m->idc[ID_DEFF]);
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY)
			{
				cfg = m->cfg;
				update = 0;
				r = dlgs_getcbcur(hWnd, m->idc[ID_PORT], COMPORT_NONE);
				if (cfg->port != (UINT8)r)
				{
					cfg->port = (UINT8)r;
					update |= SYS_UPDATEOSCFG;
					update |= m->update;
				}
				r = SendDlgItemMessage(hWnd, m->idc[ID_SPEED],
										CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if (r != CB_ERR) {
					if ((UINT)r >= NELEMENTS(cmserial_speed)) {
						r = 0;
					}
					if (cfg->speed != cmserial_speed[r]) {
						cfg->speed = cmserial_speed[r];
						update |= SYS_UPDATEOSCFG;
						update |= m->update;
					}
				}

				b = 0;
				b |= getChars(hWnd, m->idc[ID_CHARS], cfg->param);
				b |= getParity(hWnd, m->idc[ID_PARITY], cfg->param);
				b |= getStopBit(hWnd, m->idc[ID_SBIT], cfg->param);

				if (cfg->param != b) {
					cfg->param = b;
					update |= SYS_UPDATEOSCFG;
					update |= m->update;
				}

				GetDlgItemText(hWnd, m->idc[ID_MMAP],
											str.mmap, NELEMENTS(str.mmap));
				if (milstr_cmp(cfg->mout, str.mmap)) {
					milstr_ncpy(cfg->mout, str.mmap, NELEMENTS(cfg->mout));
					update |= SYS_UPDATEOSCFG;
					update |= m->update;
				}
				GetDlgItemText(hWnd, m->idc[ID_MMDL],
											str.mmdl, NELEMENTS(str.mmdl));
				if (milstr_cmp(cfg->mdl, str.mmdl)) {
					milstr_ncpy(cfg->mdl, str.mmdl, NELEMENTS(cfg->mdl));
					update |= SYS_UPDATEOSCFG;
					update |= m->update;
				}

				cfg->def_en = (UINT8)GetDlgItemCheck(hWnd, m->idc[ID_DEFE]);
				cm = *m->cm;
				if (cm) {
					cm->msg(cm, COMMSG_MIMPIDEFEN, cfg->def_en);
				}
				GetDlgItemText(hWnd, m->idc[ID_DEFF],
											str.mdef, NELEMENTS(str.mdef));
				if (milstr_cmp(cfg->def, str.mdef)) {
					milstr_ncpy(cfg->def, str.mdef, NELEMENTS(cfg->def));
					update |= SYS_UPDATEOSCFG;
					if (cm) {
						cm->msg(cm, COMMSG_MIMPIDEFFILE, (INTPTR)str.mdef);
					}
				}
				sysmng_update(update);
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}


// ---------------------------------------------------------------------------

static LRESULT CALLBACK Com1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	return(dlgitem_proc(hWnd, msg, wp, lp, &res_com1));
}

static LRESULT CALLBACK Com2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	return(dlgitem_proc(hWnd, msg, wp, lp, &res_com2));
}

static LRESULT CALLBACK Com3Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	return(dlgitem_proc(hWnd, msg, wp, lp, &res_com3));
}



// ----

/**
 * @brief 61 ページ
 */
class SndOpt61Page : public CPropPageProc
{
public:
	SndOpt61Page();
	virtual ~SndOpt61Page();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	UINT8 m_sw[3];				//!< スイッチ
	UINT8 m_jmp[6];				//!< ジャンパ
	CComboData m_speed[2];		//!< Speed
	CComboData m_int[2];		//!< INT
	CComboData m_sync[2];		//!< Mode
	CStaticDipSw m_dipsw;		//!< DIPSW
	void Set(const UINT8* sw, const UINT8* jmp);
	UINT8 GetMode(UINT nIndex, UINT8 cMode);
	void SetMode(UINT nIndex, UINT8 cMode);
	void UpdateMode(UINT nIndex, UINT8& cMode);
	void OnDipSw();
};

enum
{
	PC9861S1_X		= 1,
	PC9861S2_X		= 10,
	PC9861S3_X		= 17,
	PC9861S_Y		= 1,

	PC9861J1_X		= 1,
	PC9861J2_X		= 9,
	PC9861J3_X		= 17,
	PC9861J4_X		= 1,
	PC9861J5_X		= 11,
	PC9861J6_X		= 19,
	PC9861J1_Y		= 4,
	PC9861J4_Y		= 7
};

//! INT1
static const CComboData::Value s_int1[] =
{
	{0,	0x00},
	{1,	0x02},
	{2,	0x01},
	{3,	0x03},
};

//! INT2
static const CComboData::Value s_int2[] =
{
	{0,	0x00},
	{4,	0x08},
	{5,	0x04},
	{6,	0x0c},
};

//! 同期方法
static const CComboData::Entry s_sync[] =
{
	{MAKEINTRESOURCE(IDS_SYNC),		0x03},
	{MAKEINTRESOURCE(IDS_ASYNC),	0x00},
	{MAKEINTRESOURCE(IDS_ASYNC16X),	0x01},
	{MAKEINTRESOURCE(IDS_ASYNC64X),	0x02},
};

/**
 * コンストラクタ
 */
SndOpt61Page::SndOpt61Page()
	: CPropPageProc(IDD_PC9861A)
{
	ZeroMemory(m_sw, sizeof(m_sw));
	ZeroMemory(m_jmp, sizeof(m_jmp));
}

/**
 * デストラクタ
 */
SndOpt61Page::~SndOpt61Page()
{
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL SndOpt61Page::OnInitDialog()
{
	CheckDlgButton(IDC_PC9861E, (np2cfg.pc9861enable) ? BST_CHECKED : BST_UNCHECKED);

	m_speed[0].SubclassDlgItem(IDC_CH1SPEED, this);
	m_speed[0].Add(pc9861k_speed, _countof(pc9861k_speed));
	m_speed[1].SubclassDlgItem(IDC_CH2SPEED, this);
	m_speed[1].Add(pc9861k_speed, _countof(pc9861k_speed));

	m_int[0].SubclassDlgItem(IDC_CH1INT, this);
	m_int[0].Add(s_int1, _countof(s_int1));
	m_int[1].SubclassDlgItem(IDC_CH2INT, this);
	m_int[1].Add(s_int2, _countof(s_int2));

	m_sync[0].SubclassDlgItem(IDC_CH1MODE, this);
	m_sync[0].Add(s_sync, _countof(s_sync));
	m_sync[1].SubclassDlgItem(IDC_CH2MODE, this);
	m_sync[1].Add(s_sync, _countof(s_sync));

	Set(np2cfg.pc9861sw, np2cfg.pc9861jmp);

	m_dipsw.SubclassDlgItem(IDC_PC9861DIP, this);

	return TRUE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void SndOpt61Page::OnOK()
{
	bool bUpdated = false;

	const UINT8 cEnabled = (IsDlgButtonChecked(IDC_PC9861E) != BST_UNCHECKED) ? 1 : 0;
	if (np2cfg.pc9861enable != cEnabled)
	{
		np2cfg.pc9861enable = cEnabled;
		bUpdated = true;
	}

	if (memcmp(np2cfg.pc9861sw, m_sw, 3))
	{
		CopyMemory(np2cfg.pc9861sw, m_sw, 3);
		bUpdated = true;
	}
	if (memcmp(np2cfg.pc9861jmp, m_jmp, 6))
	{
		CopyMemory(np2cfg.pc9861jmp, m_jmp, 6);
		bUpdated = true;
	}

	if (bUpdated)
	{
		::sysmng_update(SYS_UPDATECFG);
	}
}

/**
 * ユーザーがメニューの項目を選択したときに、フレームワークによって呼び出されます
 * @param[in] wParam パラメタ
 * @param[in] lParam パラメタ
 * @retval TRUE アプリケーションがこのメッセージを処理した
 */
BOOL SndOpt61Page::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_CH1SPEED:
		case IDC_CH1MODE:
			UpdateMode(0, m_sw[0]);
			break;

		case IDC_CH2SPEED:
		case IDC_CH2MODE:
			UpdateMode(1, m_sw[2]);
			break;

		case IDC_CH1INT:
		case IDC_CH2INT:
			{
				UINT8 cMode = m_sw[1] & 0xf0;
				cMode |= m_int[0].GetCurItemData(m_sw[1] & 0x03);
				cMode |= m_int[1].GetCurItemData(m_sw[1] & 0x0c);
				if (m_sw[1] != cMode)
				{
					m_sw[1] = cMode;
					m_dipsw.Invalidate();
				}
			}
			break;

		case IDC_PC9861DIP:
			OnDipSw();
			break;
	}
	return FALSE;
}

/**
 * CWndProc オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] nMsg 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT SndOpt61Page::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_DRAWITEM:
			if (LOWORD(wParam) == IDC_PC9861DIP)
			{
				UINT8* pBitmap = dipswbmp_get9861(m_sw, m_jmp);
				m_dipsw.Draw((reinterpret_cast<LPDRAWITEMSTRUCT>(lParam))->hDC, pBitmap);
				_MFREE(pBitmap);
			}
			return FALSE;
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * コントロール設定
 * @param[in] sw 設定値
 * @param[in] jmp 設定値
 */
void SndOpt61Page::Set(const UINT8* sw, const UINT8* jmp)
{
	CopyMemory(m_sw, sw, sizeof(m_sw));
	CopyMemory(m_jmp, jmp, sizeof(m_jmp));

	SetMode(0, sw[0]);
	SetMode(1, sw[2]);
	m_int[0].SetCurItemData(sw[1] & 0x03);
	m_int[1].SetCurItemData(sw[1] & 0x0c);
}

/**
 * モードを得る
 * @param[in] nIndex ポート
 * @param[in] cMode デフォルト値
 * @return モード
 */
UINT8 SndOpt61Page::GetMode(UINT nIndex, UINT8 cMode)
{
	const UINT8 cSync = m_sync[nIndex].GetCurItemData(cMode & 0x03);

	UINT nSpeed = m_speed[nIndex].GetCurSel();
	if (nSpeed > (_countof(pc9861k_speed) - 1))
	{
		nSpeed = _countof(pc9861k_speed) - 1;
	}
	if (cSync & 2)
	{
		nSpeed += 3;
	}
	else
	{
		if (nSpeed)
		{
			nSpeed--;
		}
	}
	return (((~nSpeed) & 0x0f) << 2) | cSync;
}

/**
 * モードを設定
 * @param[in] nIndex ポート
 * @param[in] cMode モード
 */
void SndOpt61Page::SetMode(UINT nIndex, UINT8 cMode)
{
	UINT nSpeed = (((~cMode) >> 2) & 0x0f) + 1;
	if (cMode)
	{
		if (nSpeed > 4)
		{
			nSpeed -= 4;
		}
		else
		{
			nSpeed = 0;
		}
	}
	if (nSpeed > (_countof(pc9861k_speed) - 1))
	{
		nSpeed = _countof(pc9861k_speed) - 1;
	}
	m_speed[nIndex].SetCurSel(nSpeed);
	m_sync[nIndex].SetCurItemData(cMode & 0x03);
}

/**
 * 更新
 * @param[in] nIndex ポート
 * @param[out] cMode モード
 */
void SndOpt61Page::UpdateMode(UINT nIndex, UINT8& cMode)
{
	const UINT8 cValue = GetMode(nIndex, cMode);
	if (cMode != cValue)
	{
		cMode = cValue;
		SetMode(nIndex, cMode);
		m_dipsw.Invalidate();
	}
}

/**
 * DIPSW をタップした
 */
void SndOpt61Page::OnDipSw()
{
	RECT rect1;
	m_dipsw.GetWindowRect(&rect1);

	RECT rect2;
	m_dipsw.GetClientRect(&rect2);

	POINT p;
	::GetCursorPos(&p);
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;

	UINT8 sw[3];
	UINT8 jmp[6];
	CopyMemory(sw, m_sw, sizeof(sw));
	CopyMemory(jmp, m_jmp, sizeof(jmp));

	if ((p.y >= 1) && (p.y < 3))					// 1段目
	{
		if ((p.x >= 1) && (p.x < 7))				// S1
		{
			sw[0] ^= (1 << (p.x - 1));
		}
		else if ((p.x >= 10) && (p.x < 14))			// S2
		{
			sw[1] ^= (1 << (p.x - 10));
		}
		else if ((p.x >= 17) && (p.x < 23))			// S3
		{
			sw[2] ^= (1 << (p.x - 17));
		}
	}
	else if ((p.y >= 4) && (p.y < 6))				// 2段目
	{
		if ((p.x >= 1) && (p.x < 7))				// J1
		{
			jmp[0] ^= (1 << (p.x - 1));
		}
		else if ((p.x >= 9) && (p.x < 15))			// J2
		{
			jmp[1] ^= (1 << (p.x - 9));
		}
		else if ((p.x >= 17) && (p.x < 19))			// J3
		{
			jmp[2] = (1 << (p.x - 17));
		}
	}
	else if ((p.y >= 7) && (p.y < 9))				// 3段目
	{
		if ((p.x >= 1) && (p.x < 9))				// J4
		{
			const UINT8 cBit = (1 << (p.x - 1));
			jmp[3] = (jmp[3] != cBit) ? cBit : 0;
		}
		else if ((p.x >= 11) && (p.x < 17))			// J5
		{
			jmp[4] ^= (1 << (p.x - 11));
		}
		else if ((p.x >= 19) && (p.x < 25))			// J6
		{
			jmp[5] ^= (1 << (p.x - 19));
		}
	}

	if ((memcmp(m_sw, sw, sizeof(sw)) != 0) || (memcmp(m_jmp, jmp, sizeof(jmp)) != 0))
	{
		Set(sw, jmp);
		m_dipsw.Invalidate();
	}
}



// ----

/**
 * シリアル設定
 * @param[in] hwndParent 親ウィンドウ
 */
void dialog_serial(HWND hwndParent)
{
	std::vector<HPROPSHEETPAGE> hpsp;

	PROPSHEETPAGE psp;
	ZeroMemory(&psp, sizeof(psp));
	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.dwFlags = PSP_DEFAULT;
	psp.hInstance = CWndProc::GetResourceHandle();

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SERIAL1);
	psp.pfnDlgProc = (DLGPROC)Com1Proc;
	hpsp.push_back(::CreatePropertySheetPage(&psp));

	SndOpt61Page pc9861;
	hpsp.push_back(::CreatePropertySheetPage(&pc9861.m_psp));

	psp.pszTemplate = MAKEINTRESOURCE(IDD_PC9861B);
	psp.pfnDlgProc = (DLGPROC)Com2Proc;
	hpsp.push_back(::CreatePropertySheetPage(&psp));

	psp.pszTemplate = MAKEINTRESOURCE(IDD_PC9861C);
	psp.pfnDlgProc = (DLGPROC)Com3Proc;
	hpsp.push_back(::CreatePropertySheetPage(&psp));

	std::tstring rTitle(LoadTString(IDS_SERIALOPTION));

	PROPSHEETHEADER psh;
	ZeroMemory(&psh, sizeof(psh));
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	psh.hwndParent = hwndParent;
	psh.hInstance = CWndProc::GetResourceHandle();
	psh.hIcon = LoadIcon(psh.hInstance, MAKEINTRESOURCE(IDI_ICON2));
	psh.nPages = hpsp.size();
	psh.phpage = &hpsp.at(0);
	psh.pszCaption = rTitle.c_str();
	psh.pfnCallback = np2class_propetysheet;
	::PropertySheet(&psh);
	::InvalidateRect(hwndParent, NULL, TRUE);
}
