#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"commng.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"bit2res.h"
#include	"pccore.h"


#ifdef __cplusplus
extern "C" {
#endif
extern	COMMNG	cm_mpu98;
#ifdef __cplusplus
}
#endif

static const char *mpuinterrupt[4] = {str_int0, str_int1, str_int2, str_int5};

static	BYTE			mpu = 0;
static	SUBCLASSPROC	oldidc_mpujmp = NULL;


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

static void setmpuiodip(BYTE *image, int px, int py, int align, BYTE v) {

	int		i, j, y;

	px *= 9;
	px++;
	py *= 9;
	for (i=0; i<4; i++, px+=9, v<<=1) {
		y = py + ((v&0x80)?5:9);
		for (j=0; j<3; j++) {
			dlgs_linex(image, px, y+j, 7, align, 2);
		}
	}
}

static void setmpuintdip(BYTE *image, int px, int py, int align, BYTE v) {

	dlgs_setjumpery(image, px + 3 - (mpu & 3), py, align);
}

static LRESULT CALLBACK mpujmp(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	PAINTSTRUCT	ps;
	HDC			hdc;
	HBITMAP		hBitmap;
	HDC			hMemDC;
	BYTE		*image;
	HANDLE		hwork;
	BITMAPINFO	*work;
	BYTE		*imgbtm;
	int			align;

	switch(msg) {
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if ((hwork = GlobalAlloc(GPTR, bit2res_getsize(&mpudip)))
																== NULL) {
				break;
			}
			if ((work = (BITMAPINFO *)GlobalLock(hwork)) == NULL) {
				GlobalFree(hwork);
				break;
			}
			bit2res_sethead(work, &mpudip);
			hBitmap = CreateDIBSection(hdc, work, DIB_RGB_COLORS,
												(void **)&image, NULL, 0);
			bit2res_setdata(image, &mpudip);
			align = ((mpudip.x + 7) / 2) & ~3;
			imgbtm = image + align * (mpudip.y - 1);
			setmpuiodip(imgbtm, 2, 1, align, mpu);
			setmpuintdip(imgbtm, 9, 1, align, mpu);
			if ((hMemDC = CreateCompatibleDC(hdc)) != NULL) {
				SelectObject(hMemDC, hBitmap);
				StretchBlt(hdc, 0, 0, mpudip.x, mpudip.y, hMemDC,
									0, 0, mpudip.x, mpudip.y, SRCCOPY);
				DeleteDC(hMemDC);
			}
			DeleteObject(hBitmap);
			EndPaint(hWnd, &ps);
			GlobalUnlock(hwork);
			GlobalFree(hwork);
			break;
		default:
			return(CallWindowProc(oldidc_mpujmp, hWnd, msg, wp, lp));
	}
	return(FALSE);
}

static void updatempu(HWND hWnd) {

	union {
		char	mmap[MAXPNAMELEN];
		char	mmdl[64];
		char	mdef[MAX_PATH];
		char	mdin[MAXPNAMELEN];
	} s;
	UINT	update;

	update = 0;
	if (np2cfg.mpuopt != mpu) {
		np2cfg.mpuopt = mpu;
		update |= SYS_UPDATECFG | SYS_UPDATEMIDI;
	}
	GetDlgItemText(hWnd, IDC_MPU98MMAP, s.mmap, sizeof(s.mmap));
	if (milstr_cmp(np2oscfg.mpu.mout, s.mmap)) {
		milstr_ncpy(np2oscfg.mpu.mout, s.mmap, sizeof(np2oscfg.mpu.mout));
		update |= SYS_UPDATEOSCFG | SYS_UPDATEMIDI;
	}
	GetDlgItemText(hWnd, IDC_MPU98MDIN, s.mdin, sizeof(s.mdin));
	if (milstr_cmp(np2oscfg.mpu.min, s.mdin)) {
		milstr_ncpy(np2oscfg.mpu.min, s.mdin, sizeof(np2oscfg.mpu.min));
		update |= SYS_UPDATEOSCFG | SYS_UPDATEMIDI;
	}
	GetDlgItemText(hWnd, IDC_MPU98MMDL, s.mmdl, sizeof(s.mmdl));
	if (milstr_cmp(np2oscfg.mpu.mdl, s.mmdl)) {
		milstr_ncpy(np2oscfg.mpu.mdl, s.mmdl, sizeof(np2oscfg.mpu.mdl));
		update |= SYS_UPDATEOSCFG | SYS_UPDATEMIDI;
	}

	np2oscfg.mpu.def_en = GetDlgItemCheck(hWnd, IDC_MPU98DEFE);
	GetDlgItemText(hWnd, IDC_MPU98DEFF, s.mdef, sizeof(s.mdef));
	if (milstr_cmp(np2oscfg.mpu.def, s.mdef)) {
		milstr_ncpy(np2oscfg.mpu.def, s.mdef, sizeof(np2oscfg.mpu.def));
//		commsmidi_toneload(&mpu98_comm, s.mdef);
		update |= SYS_UPDATEOSCFG;
	}
	sysmng_update(update);
}

LRESULT CALLBACK MidiDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	BYTE	b, bit;
	int		i;
	RECT	rect1;
	RECT	rect2;
	POINT	p;

	switch(msg) {
		case WM_INITDIALOG:
			mpu = np2cfg.mpuopt;
			for (i=0; i<16; i++) {
				char buf[8];
				wsprintf(buf, str_4X, 0xC0D0 + (i << 10));
				SendDlgItemMessage(hWnd, IDC_MPUIO,
									CB_INSERTSTRING, (WPARAM)i, (LPARAM)buf);
			}
			setmpuiopara(hWnd, IDC_MPUIO, mpu);
			SETLISTSTR(hWnd, IDC_MPUINT, mpuinterrupt);
			setmpuintpara(hWnd, IDC_MPUINT, mpu);

			dlgs_setlistmidiout(hWnd, IDC_MPU98MMAP, np2oscfg.mpu.mout);
			dlgs_setlistmidiin(hWnd, IDC_MPU98MDIN, np2oscfg.mpu.min);
			SETLISTSTR(hWnd, IDC_MPU98MMDL, cmmidi_mdlname);
			SetDlgItemText(hWnd, IDC_MPU98MMDL, np2oscfg.mpu.mdl);
			SetDlgItemCheck(hWnd, IDC_MPU98DEFE, np2oscfg.mpu.def_en);
			SetDlgItemText(hWnd, IDC_MPU98DEFF, np2oscfg.mpu.def);

			oldidc_mpujmp = (SUBCLASSPROC)GetWindowLong(GetDlgItem(hWnd,
												IDC_MPUDIP), GWL_WNDPROC);
			SetWindowLong(GetDlgItem(hWnd, IDC_MPUDIP), GWL_WNDPROC,
													(LONG)mpujmp);
			SetFocus(GetDlgItem(hWnd, IDC_MPUIO));					// ver0.30
			return(FALSE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDOK:
					updatempu(hWnd);
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

				case IDC_MPU98DEFB:
					dlgs_browsemimpidef(hWnd, IDC_MPU98DEFF);
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

