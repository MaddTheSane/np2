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

	dlgs_setlistmidiout(hWnd, IDC_MPU98MMAP, np2oscfg.mpu.mout);
	dlgs_setlistmidiin(hWnd, IDC_MPU98MDIN, np2oscfg.mpu.min);
	SETLISTSTR(hWnd, IDC_MPU98MMDL, cmmidi_mdlname);
	SetDlgItemText(hWnd, IDC_MPU98MMDL, np2oscfg.mpu.mdl);
	SetDlgItemCheck(hWnd, IDC_MPU98DEFE, np2oscfg.mpu.def_en);
	SetDlgItemText(hWnd, IDC_MPU98DEFF, np2oscfg.mpu.def);

	// SS_OWNERDRAW‚É‚·‚é‚Æ IDE‚Å•s“s‡‚ªo‚é‚Ì‚Åc
	sub = GetDlgItem(hWnd, IDC_MPUDIP);
	SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));

	SetFocus(GetDlgItem(hWnd, IDC_MPUIO));
}

static void mpuupdate(HWND hWnd) {

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
	if (cm_mpu98) {
		cm_mpu98->msg(cm_mpu98, COMMSG_MIMPIDEFEN, np2oscfg.mpu.def_en);
	}
	GetDlgItemText(hWnd, IDC_MPU98DEFF, s.mdef, sizeof(s.mdef));
	if (milstr_cmp(np2oscfg.mpu.def, s.mdef)) {
		milstr_ncpy(np2oscfg.mpu.def, s.mdef, sizeof(np2oscfg.mpu.def));
		if (cm_mpu98) {
			cm_mpu98->msg(cm_mpu98, COMMSG_MIMPIDEFFILE, (long)s.mdef);
		}
		update |= SYS_UPDATEOSCFG;
	}
	sysmng_update(update);
}

static void mpucmddipsw(HWND hWnd) {

	RECT	rect1;
	RECT	rect2;
	POINT	p;
	BYTE	bit;

	GetWindowRect(GetDlgItem(hWnd, IDC_MPUDIP), &rect1);
	GetClientRect(GetDlgItem(hWnd, IDC_MPUDIP), &rect2);
	GetCursorPos(&p);
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;
	if ((p.y < 1) || (p.y >= 3)) {
		return;
	}
	if ((p.x >= 2) && (p.x < 6)) {
		bit = 0x80 >> (p.x - 2);
		mpu ^= bit;
		setmpuiopara(hWnd, IDC_MPUIO, mpu);
		InvalidateRect(GetDlgItem(hWnd, IDC_MPUDIP), NULL, TRUE);
	}
	else if ((p.x >= 9) && (p.x < 13)) {
		bit = (BYTE)(12 - p.x);
		if ((mpu ^ bit) & 3) {
			mpu &= ~0x3;
			mpu |= bit;
			setmpuintpara(hWnd, IDC_MPUINT, mpu);
			InvalidateRect(GetDlgItem(hWnd, IDC_MPUDIP), NULL, TRUE);
		}
	}
}

static void mpudrawdipsw(HWND hWnd, HDC hdc) {

	BITMAPINFO	*bmi;
	HBITMAP		hbmp;
	BYTE		*image;
	int			align;
	BYTE		*imgbtm;
	HDC			hmdc;

	bmi = (BITMAPINFO *)_MALLOC(bit2res_getsize(&mpudip), "bitmap");
	if (bmi == NULL) {
		return;
	}
	bit2res_sethead(bmi, &mpudip);
	hbmp = CreateDIBSection(hdc, bmi, DIB_RGB_COLORS,
												(void **)&image, NULL, 0);
	bit2res_setdata(image, &mpudip);
	align = ((mpudip.x + 7) / 2) & ~3;
	imgbtm = image + align * (mpudip.y - 1);
	setmpuiodip(imgbtm, 2, 1, align, mpu);
	setmpuintdip(imgbtm, 9, 1, align, mpu);
	hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);
	BitBlt(hdc, 0, 0, mpudip.x, mpudip.y, hmdc, 0, 0, SRCCOPY);
	DeleteDC(hmdc);
	DeleteObject(hbmp);
	_MFREE(bmi);
}

LRESULT CALLBACK MidiDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

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
					mpucmddipsw(hWnd);
					return(FALSE);

				case IDC_MPU98DEFB:
					dlgs_browsemimpidef(hWnd, IDC_MPU98DEFF);
					return(FALSE);
			}
			break;

		case WM_DRAWITEM:
			if (LOWORD(wp) == IDC_MPUDIP) {
				mpudrawdipsw(hWnd, ((LPDRAWITEMSTRUCT)lp)->hDC);
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

