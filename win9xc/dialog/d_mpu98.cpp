#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"commng.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"dipswbmp.h"


#ifdef __cplusplus
extern "C" {
#endif

extern	COMMNG	cm_mpu98;

#ifdef __cplusplus
}
#endif

static const char *mpuinterrupt[4] = {str_int0, str_int1, str_int2, str_int5};

static	BYTE	mpu = 0;


static void setmpuiopara(HWND hWnd, WORD res, BYTE value) {

	SendDlgItemMessage(hWnd, res, CB_SETCURSEL,
									(WPARAM)((value >> 4) & 15), (LPARAM)0);
}

static BYTE getmpuio(HWND hWnd, WORD res) {

	char	work[8];

	GetDlgItemText(hWnd, res, work, sizeof(work));
	return((milstr_solveHEX(work) >> 6) & 0xf0);
}

static void setmpuintpara(HWND hWnd, WORD res, BYTE value) {

	SendDlgItemMessage(hWnd, res, CB_SETCURSEL,
									(WPARAM)(value & 3), (LPARAM)0);
}

static BYTE getmpuint(HWND hWnd, WORD res) {

	char	work[8];
	BYTE	ret;

	GetDlgItemText(hWnd, res, work, sizeof(work));
	ret = work[3] - '0';
	if (ret >= 3) {
		ret = 3;
	}
	return(ret);
}

static void setmpujmp(HWND hWnd, BYTE value, BYTE bit) {

	if ((mpu ^ value) & bit) {
		mpu &= ~bit;
		mpu |= value;
		InvalidateRect(GetDlgItem(hWnd, IDC_MPUDIP), NULL, TRUE);
	}
}


// ----

static void mpucreate(HWND hWnd) {

	UINT	i;
	char	buf[8];
	HWND	sub;

	mpu = np2cfg.mpuopt;
	for (i=0; i<16; i++) {
		wsprintf(buf, str_4X, 0xC0D0 + (i << 10));
		SendDlgItemMessage(hWnd, IDC_MPUIO,
									CB_INSERTSTRING, (WPARAM)i, (LPARAM)buf);
	}
	setmpuiopara(hWnd, IDC_MPUIO, mpu);
	SETLISTSTR(hWnd, IDC_MPUINT, mpuinterrupt);
	setmpuintpara(hWnd, IDC_MPUINT, mpu);

	// SS_OWNERDRAW�ɂ���� IDE�ŕs�s�����o��̂Łc
	sub = GetDlgItem(hWnd, IDC_MPUDIP);
	SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));

	SetFocus(GetDlgItem(hWnd, IDC_MPUIO));
}

static void mpuupdate(HWND hWnd) {

	UINT	update;

	update = 0;
	if (np2cfg.mpuopt != mpu) {
		np2cfg.mpuopt = mpu;
		update |= SYS_UPDATECFG | SYS_UPDATEMIDI;
	}
	sysmng_update(update);
}

LRESULT CALLBACK MidiDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	BYTE	b, bit;
	RECT	rect1;
	RECT	rect2;
	POINT	p;

	switch(msg) {
		case WM_INITDIALOG:
			mpucreate(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDOK:
					mpuupdate(hWnd);
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

				case IDC_MPUIO:
					setmpujmp(hWnd, getmpuio(hWnd, IDC_MPUIO), 0xf0);
					return(FALSE);

				case IDC_MPUINT:
					setmpujmp(hWnd, getmpuint(hWnd, IDC_MPUINT), 0x03);
					return(FALSE);

				case IDC_MPUDEF:
					mpu = 0x82;
					setmpuiopara(hWnd, IDC_MPUIO, mpu);
					setmpuintpara(hWnd, IDC_MPUINT, mpu);
					InvalidateRect(GetDlgItem(hWnd, IDC_MPUDIP), NULL, TRUE);
					return(FALSE);

				case IDC_MPUDIP:
					GetWindowRect(GetDlgItem(hWnd, IDC_MPUDIP), &rect1);
					GetClientRect(GetDlgItem(hWnd, IDC_MPUDIP), &rect2);
					GetCursorPos(&p);
					p.x += rect2.left - rect1.left;
					p.y += rect2.top - rect1.top;
					p.x /= 9;
					p.y /= 9;
					if ((p.y < 1) || (p.y >= 3)) {
						return(FALSE);
					}
					if ((p.x >= 2) && (p.x < 6)) {
						bit = 0x80 >> (p.x - 2);
						mpu ^= bit;
						setmpuiopara(hWnd, IDC_MPUIO, mpu);
						InvalidateRect(GetDlgItem(hWnd, IDC_MPUDIP),
															NULL, TRUE);
					}
					else if ((p.x >= 9) && (p.x < 13)) {
						b = (BYTE)(12 - p.x);
						if ((mpu ^ b) & 3) {
							mpu &= ~0x3;
							mpu |= b;
							setmpuintpara(hWnd, IDC_MPUINT, mpu);
							InvalidateRect(GetDlgItem(hWnd, IDC_MPUDIP),
															NULL, TRUE);
						}
					}
					return(FALSE);
			}
			break;

		case WM_DRAWITEM:
			if (LOWORD(wp) == IDC_MPUDIP) {
				dlgs_drawbmp(((LPDRAWITEMSTRUCT)lp)->hDC,
													dipswbmp_getmpu(mpu));
			}
			return(FALSE);

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}

