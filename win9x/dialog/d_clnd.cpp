#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"timemng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"calendar.h"


static	BYTE	cbuf[8];

typedef struct {
	UINT16	res;
	BYTE	min;
	BYTE	max;
} VIRCAL_T;

static const VIRCAL_T vircal[6] = {	{IDC_VIRYEAR,	0x00, 0x99},
									{IDC_VIRMONTH,	0x01, 0x12},
									{IDC_VIRDAY,	0x01, 0x31},
									{IDC_VIRHOUR,	0x00, 0x23},
									{IDC_VIRMINUTE,	0x00, 0x59},
									{IDC_VIRSECOND,	0x00, 0x59}};


static void set_cal2dlg(HWND hWnd, const BYTE *cbuf) {

	int		i;
	char	work[8];

	for (i=0; i<6; i++) {
		if (i != 1) {
			wsprintf(work, str_2x, cbuf[i]);
		}
		else {
			wsprintf(work, str_2d, cbuf[1] >> 4);
		}
		SetDlgItemText(hWnd, vircal[i].res, work);
	}
}

static void vircalendar(HWND hWnd, BOOL disp) {

	int		i;

	for (i=0; i<6; i++) {
		EnableWindow(GetDlgItem(hWnd, vircal[i].res), disp);
	}
	EnableWindow(GetDlgItem(hWnd, IDC_SETNOW), disp);
}

static DWORD getbcd(char *str, int len) {

	DWORD	ret;
	BYTE	c;

	if (!(*str)) {
		return(0xff);
	}
	ret = 0;
	while(len--) {
		c = *str++;
		if (!c) {
			break;
		}
		if ((c < '0') || (c > '9')) {
			return(0xff);
		}
		ret <<= 4;
		ret |= (BYTE)(c - '0');
	}
	return(ret);
}

LRESULT CALLBACK ClndDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	char	work[32];
	BYTE	b;
	int		i;
	HWND	subwnd;

	switch (msg) {
		case WM_INITDIALOG:
			// ���Ԃ��Z�b�g�B
			calendar_getvir(cbuf);
			set_cal2dlg(hWnd, cbuf);
			if (np2cfg.calendar) {
				vircalendar(hWnd, 0);
				subwnd = GetDlgItem(hWnd, IDC_CLNDREAL);
			}
			else {
				vircalendar(hWnd, 1);
				subwnd = GetDlgItem(hWnd, IDC_CLNDVIR);
			}
			SendMessage(subwnd, BM_SETCHECK, TRUE, 0);
			SetFocus(subwnd);
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					b = GetDlgItemCheck(hWnd, IDC_CLNDREAL);
					if (np2cfg.calendar != b) {
						np2cfg.calendar = b;
						sysmng_update(SYS_UPDATECFG);
					}
					for (i=0; i<6; i++) {
						GetDlgItemText(hWnd, vircal[i].res,
														work, sizeof(work));
						b = (BYTE)getbcd(work, 2);
						if ((b >= vircal[i].min) && (b <= vircal[i].max)) {
							if (i == 1) {
								b = ((b & 0x10) * 10) + (b << 4);
							}
							cbuf[i] = b;
						}
					}
					calendar_set(cbuf);
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

				case IDC_CLNDVIR:
					vircalendar(hWnd, 1);
					return(FALSE);

				case IDC_CLNDREAL:
					vircalendar(hWnd, 0);
					return(FALSE);

				case IDC_SETNOW:
					calendar_getreal(cbuf);
					set_cal2dlg(hWnd, cbuf);
					return(FALSE);

				default:
					return(FALSE);
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}

