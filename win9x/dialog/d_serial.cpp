#include	"compiler.h"
#include	<prsht.h>
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"commng.h"
#include	"sysmng.h"
#include	"np2class.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"pc9861k.h"
#include	"dipswbmp.h"


static const TCHAR str_none[] = _T("NONE");
static const TCHAR str_com1[] = _T("COM1");
static const TCHAR str_com2[] = _T("COM2");
static const TCHAR str_com3[] = _T("COM3");
static const TCHAR str_com4[] = _T("COM4");
static const TCHAR str_midi[] = _T("MIDI");
static const TCHAR str_odd[] = _T("ODD");
static const TCHAR str_even[] = _T("EVEN");
static const TCHAR str_one[] = _T("1");
static const TCHAR str_onehalf[] = _T("1.5");
static const TCHAR str_two[] = _T("2");

static const TCHAR *rsport[] = {str_none, str_com1, str_com2, str_com3,
									str_com4, str_midi};
static const UINT32 rscharsize[] = {5, 6, 7, 8};
static const TCHAR *rsparity[] = {str_none, str_odd, str_even};
static const TCHAR *rsstopbit[] = {str_one, str_onehalf, str_two};

static const TCHAR str_seropt[] = _T("Serial option");


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


static LRESULT CALLBACK dlgitem_proc(HWND hWnd, UINT msg,
								WPARAM wp, LPARAM lp, const DLGCOM_P *m) {

	UINT32	d;
	UINT8	b;
	LRESULT	r;
	union {
		OEMCHAR	mmap[MAXPNAMELEN];
		OEMCHAR	mmdl[64];
		OEMCHAR	mdef[MAX_PATH];
	} str;
	COMCFG	*cfg;
	UINT	update;
	COMMNG	cm;

	switch (msg) {
		case WM_INITDIALOG:
			cfg = m->cfg;
			SETLISTSTR(hWnd, m->idc[ID_PORT], rsport);
			SETLISTUINT32(hWnd, m->idc[ID_SPEED], cmserial_speed);
			SETLISTUINT32(hWnd, m->idc[ID_CHARS], rscharsize);
			SETLISTSTR(hWnd, m->idc[ID_PARITY], rsparity);
			SETLISTSTR(hWnd, m->idc[ID_SBIT], rsstopbit);
			for (d=0; d<(NELEMENTS(cmserial_speed) - 1); d++) {
				if (cmserial_speed[d] >= cfg->speed) {
					break;
				}
			}
			SendDlgItemMessage(hWnd, m->idc[ID_SPEED],
										CB_SETCURSEL, (WPARAM)d, (LPARAM)0);

			b = cfg->param;
			d = (b >> 2) & 3;
			SendDlgItemMessage(hWnd, m->idc[ID_CHARS],
										CB_SETCURSEL, (WPARAM)d, (LPARAM)0);
			if (b & 0x10) {
				d = ((b >> 5) & 1) + 1;
			}
			else {
				d = 0;
			}
			SendDlgItemMessage(hWnd, m->idc[ID_PARITY],
										CB_SETCURSEL, (WPARAM)d, (LPARAM)0);
			d = (b >> 6) & 3;
			if (d) {
				d--;
			}
			SendDlgItemMessage(hWnd, m->idc[ID_SBIT],
										CB_SETCURSEL, (WPARAM)d, (LPARAM)0);

			dlgs_setlistmidiout(hWnd, m->idc[ID_MMAP], cfg->mout);
			SETLISTSTR(hWnd, m->idc[ID_MMDL], cmmidi_mdlname);
			SetDlgItemText(hWnd, m->idc[ID_MMDL], cfg->mdl);
			SetDlgItemCheck(hWnd, m->idc[ID_DEFE], cfg->def_en);
			SetDlgItemText(hWnd, m->idc[ID_DEFF], cfg->def);

			d = cfg->port;
			if (d >= NELEMENTS(rsport)) {
				d = 0;
			}
			SendDlgItemMessage(hWnd, m->idc[ID_PORT],
								CB_SETCURSEL, (WPARAM)d, (LPARAM)0);

			dlgcom_items(hWnd, m, d);
			return(TRUE);

		case WM_COMMAND:
			if (LOWORD(wp) == m->idc[ID_PORT]) {
				r = SendDlgItemMessage(hWnd, m->idc[ID_PORT],
										CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if (r != CB_ERR) {
					dlgcom_items(hWnd, m, (UINT)r);
				}
			}
			else if (LOWORD(wp) == m->idc[ID_DEFB]) {
				dlgs_browsemimpidef(hWnd, m->idc[ID_DEFF]);
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				cfg = m->cfg;
				update = 0;
				r = SendDlgItemMessage(hWnd, m->idc[ID_PORT],
										CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if (r != CB_ERR) {
					if ((UINT)r >= NELEMENTS(rsport)) {
						r = 0;
					}
					if (cfg->port != (UINT8)r) {
						cfg->port = (UINT8)r;
						update |= SYS_UPDATEOSCFG;
						update |= m->update;
					}
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
				r = SendDlgItemMessage(hWnd, m->idc[ID_CHARS],
										CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if (r != CB_ERR) {
					b |= (UINT8)(((UINT)r & 3) << 2);
				}
				r = SendDlgItemMessage(hWnd, m->idc[ID_PARITY],
										CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if (r != CB_ERR) {
					if ((UINT)r) {
						b |= 0x10;
						b |= (UINT8)((((UINT)r - 1) & 1) << 5);
					}
				}
				r = SendDlgItemMessage(hWnd, m->idc[ID_SBIT],
										CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if (r != CB_ERR) {
					b |= (UINT8)((((UINT)r + 1) & 3) << 6);
				}
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
						cm->msg(cm, COMMSG_MIMPIDEFFILE, (long)str.mdef);
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



// --------------------------------------------------------------------

static	UINT8	pc9861_s[3];
static	UINT8	pc9861_j[6];

typedef struct {
	int		idc_speed;
	int		idc_int;
	int		idc_mode;
	UINT8	*dip_mode;
	UINT8	*dip_int;
	UINT8	sft_int;
} PC9861MODE_T;

static const PC9861MODE_T pc9861mode[2] = {
			{IDC_CH1SPEED, IDC_CH1INT, IDC_CH1MODE,
								&pc9861_s[0], &pc9861_s[1], 0},
			{IDC_CH2SPEED, IDC_CH2INT, IDC_CH2MODE,
								&pc9861_s[2], &pc9861_s[1], 2}};

enum {
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

static const UINT32 pc9861kint1[] = {0, 1, 2, 3};
static const UINT32 pc9861kint2[] = {0, 4, 5, 6};

static const TCHAR sync0[] = _T("Start-Stop");
static const TCHAR sync1[] = _T("ST1");
static const TCHAR sync2[] = _T("ST2");
static const TCHAR sync3[] = _T("RD-Sync");
static const TCHAR *pc9861sync[] = {sync0, sync1, sync2, sync3};

static const UINT pc9861d2sync[] = {1, 2, 3, 0};
static const UINT pc9861d2int[] = {0, 2, 1, 3};


static void pc9861getspeed(HWND hWnd, const PC9861MODE_T *m) {

	LRESULT	r;
	UINT8	mode;

	mode = *(m->dip_mode);
	r = SendDlgItemMessage(hWnd, m->idc_speed, CB_GETCURSEL, 0, 0);
	if (r != CB_ERR) {
		UINT speed = r;
		if (speed > (NELEMENTS(pc9861k_speed) - 1)) {
			speed = NELEMENTS(pc9861k_speed) - 1;
		}
		if (mode & 2) {
			speed += 3;
		}
		else {
			if (speed) {
				speed--;
			}
		}
		mode &= 3;
		mode |= ((~speed) & 0x0f) << 2;
		*(m->dip_mode) = mode;
	}
}

static void pc9861getint(HWND hWnd, const PC9861MODE_T *m) {

	LRESULT	r;
	UINT	i;

	r = SendDlgItemMessage(hWnd, m->idc_int, CB_GETCURSEL, 0, 0);
	if (r != CB_ERR) {
		for (i=0; i<NELEMENTS(pc9861d2int); i++) {
			if (pc9861d2int[i] == (UINT)(r & 3)) {
				*(m->dip_int) &= ~(0x03 << (m->sft_int));
				*(m->dip_int) |= (UINT8)(i << (m->sft_int));
				break;
			}
		}
	}
}

static void pc9861getmode(HWND hWnd, const PC9861MODE_T *m) {

	LRESULT	r;
	UINT8	i;

	r = SendDlgItemMessage(hWnd, m->idc_mode, CB_GETCURSEL, 0, 0);
	if (r != CB_ERR) {
		for (i=0; i<NELEMENTS(pc9861d2sync); i++) {
			if (pc9861d2sync[i] == (UINT)(r & 3)) {
				*(m->dip_mode) &= (~3);
				*(m->dip_mode) |= i;
				break;
			}
		}
	}
}

static void pc9861setmode(HWND hWnd, const PC9861MODE_T *m) {

	UINT	speed;
	UINT	mode;
	UINT	intnum;
	UINT8	modedip;

	modedip = *(m->dip_mode);
	speed = (((~modedip) >> 2) & 0x0f) + 1;
	if (modedip & 0x02) {
		if (speed > 4) {
			speed -= 4;
		}
		else {
			speed = 0;
		}
	}
	if (speed > (NELEMENTS(pc9861k_speed) - 1)) {
		speed = NELEMENTS(pc9861k_speed) - 1;
	}

	SendDlgItemMessage(hWnd, m->idc_speed,
								CB_SETCURSEL, (WPARAM)speed, (LPARAM)0);

	mode = pc9861d2sync[modedip & 3];
	SendDlgItemMessage(hWnd, m->idc_mode,
								CB_SETCURSEL, (WPARAM)mode, (LPARAM)0);

	intnum = pc9861d2int[((*(m->dip_int)) >> (m->sft_int)) & 3];
	SendDlgItemMessage(hWnd, m->idc_int,
								CB_SETCURSEL, (WPARAM)intnum, (LPARAM)0);
}

static void pc9861cmddipsw(HWND hWnd) {

	RECT	rect1;
	RECT	rect2;
	POINT	p;
	UINT8	bit;

	GetWindowRect(GetDlgItem(hWnd, IDC_PC9861DIP), &rect1);
	GetClientRect(GetDlgItem(hWnd, IDC_PC9861DIP), &rect2);
	GetCursorPos(&p);
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;
	if ((p.y >= 1) && (p.y < 3)) {					// 1’i–Ú
		if ((p.x >= 1) && (p.x < 7)) {				// S1
			pc9861_s[0] ^= (1 << (p.x - 1));
			pc9861setmode(hWnd, pc9861mode);
		}
		else if ((p.x >= 10) && (p.x < 14)) {		// S2
			pc9861_s[1] ^= (1 << (p.x - 10));
			pc9861setmode(hWnd, pc9861mode);
			pc9861setmode(hWnd, pc9861mode+1);
		}
		else if ((p.x >= 17) && (p.x < 23)) {		// S3
			pc9861_s[2] ^= (1 << (p.x - 17));
			pc9861setmode(hWnd, pc9861mode+1);
		}
	}
	else if ((p.y >= 4) && (p.y < 6)) {				// 2’i–Ú
		if ((p.x >= 1) && (p.x < 7)) {				// J1
			pc9861_j[0] ^= (1 << (p.x - 1));
		}
		else if ((p.x >= 9) && (p.x < 15)) {		// J2
			pc9861_j[1] ^= (1 << (p.x - 9));
		}
		else if ((p.x >= 17) && (p.x < 19)) {		// J3
			pc9861_j[2] = (1 << (p.x - 17));
		}
	}
	else if ((p.y >= 7) && (p.y < 9)) {				// 3’i–Ú
		if ((p.x >= 1) && (p.x < 9)) {				// J4
			bit = (1 << (p.x - 1));
			if (pc9861_j[3] == bit) {
				bit = 0;
			}
			pc9861_j[3] = bit;
		}
		else if ((p.x >= 11) && (p.x < 17)) {		// J5
			pc9861_j[4] ^= (1 << (p.x - 11));
		}
		else if ((p.x >= 19) && (p.x < 25)) {		// J6
			pc9861_j[5] ^= (1 << (p.x - 19));
		}
	}
}

static LRESULT CALLBACK pc9861mainProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	HWND	sub;
	UINT8	r;
	UINT	update;

	switch (msg) {
		case WM_INITDIALOG:
			CopyMemory(pc9861_s, np2cfg.pc9861sw, 3);
			CopyMemory(pc9861_j, np2cfg.pc9861jmp, 6);
			SETLISTUINT32(hWnd, IDC_CH1SPEED, pc9861k_speed);
			SETLISTUINT32(hWnd, IDC_CH2SPEED, pc9861k_speed);
			SETLISTUINT32(hWnd, IDC_CH1INT, pc9861kint1);
			SETLISTUINT32(hWnd, IDC_CH2INT, pc9861kint2);
			SETLISTSTR(hWnd, IDC_CH1MODE, pc9861sync);
			SETLISTSTR(hWnd, IDC_CH2MODE, pc9861sync);

			SendDlgItemMessage(hWnd, IDC_PC9861E, BM_GETCHECK,
												np2cfg.pc9861enable & 1, 0);
			pc9861setmode(hWnd, pc9861mode);
			pc9861setmode(hWnd, pc9861mode+1);

			sub = GetDlgItem(hWnd, IDC_PC9861DIP);
			SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
			return(TRUE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDC_CH1SPEED:
					pc9861getspeed(hWnd, pc9861mode);
					pc9861setmode(hWnd, pc9861mode);
					break;

				case IDC_CH1INT:
					pc9861getint(hWnd, pc9861mode);
					pc9861setmode(hWnd, pc9861mode);
					break;

				case IDC_CH1MODE:
					pc9861getmode(hWnd, pc9861mode);
					pc9861setmode(hWnd, pc9861mode);
					break;

				case IDC_CH2SPEED:
					pc9861getspeed(hWnd, pc9861mode+1);
					pc9861setmode(hWnd, pc9861mode+1);
					break;

				case IDC_CH2INT:
					pc9861getint(hWnd, pc9861mode+1);
					pc9861setmode(hWnd, pc9861mode+1);
					break;

				case IDC_CH2MODE:
					pc9861getmode(hWnd, pc9861mode+1);
					pc9861setmode(hWnd, pc9861mode+1);
					break;

				case IDC_PC9861DIP:
					pc9861cmddipsw(hWnd);
					break;

				default:
					return(FALSE);
			}
			InvalidateRect(GetDlgItem(hWnd, IDC_PC9861DIP), NULL, TRUE);
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				update = 0;
				r = GetDlgItemCheck(hWnd, IDC_PC9861E);
				if (np2cfg.pc9861enable != r) {
					np2cfg.pc9861enable = r;
					update |= SYS_UPDATECFG;
				}
				if (memcmp(np2cfg.pc9861sw, pc9861_s, 3)) {
					CopyMemory(np2cfg.pc9861sw, pc9861_s, 3);
					update |= SYS_UPDATECFG;
				}
				if (memcmp(np2cfg.pc9861jmp, pc9861_j, 6)) {
					CopyMemory(np2cfg.pc9861jmp, pc9861_j, 6);
					update |= SYS_UPDATECFG;
				}
				sysmng_update(update);
				return(TRUE);
			}
			break;

		case WM_DRAWITEM:
			if (LOWORD(wp) == IDC_PC9861DIP) {
				dlgs_drawbmp(((LPDRAWITEMSTRUCT)lp)->hDC,
										dipswbmp_get9861(pc9861_s, pc9861_j));
			}
			return(FALSE);
	}
	return(FALSE);
}


// --------------------------------------------------------------------------

void dialog_serial(HWND hWnd) {

	HINSTANCE		hinst;
	PROPSHEETPAGE	psp;
	PROPSHEETHEADER	psh;
	HPROPSHEETPAGE	hpsp[4];

	hinst = GetWindowInst(hWnd);

	ZeroMemory(&psp, sizeof(psp));
	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.dwFlags = PSP_DEFAULT;
	psp.hInstance = hinst;

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SERIAL1);
	psp.pfnDlgProc = (DLGPROC)Com1Proc;
	hpsp[0] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_PC9861A);
	psp.pfnDlgProc = (DLGPROC)pc9861mainProc;
	hpsp[1] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_PC9861B);
	psp.pfnDlgProc = (DLGPROC)Com2Proc;
	hpsp[2] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_PC9861C);
	psp.pfnDlgProc = (DLGPROC)Com3Proc;
	hpsp[3] = CreatePropertySheetPage(&psp);

	ZeroMemory(&psh, sizeof(psh));
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	psh.hwndParent = hWnd;
	psh.hInstance = hinst;
	psh.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_ICON2));
	psh.nPages = 4;
	psh.phpage = hpsp;
	psh.pszCaption = str_seropt;
	psh.pfnCallback = np2class_propetysheet;
	PropertySheet(&psh);
	InvalidateRect(hWnd, NULL, TRUE);
}

