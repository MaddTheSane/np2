#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"


static const char str_2halfmhz[] = "2.4576MHz";
static const char str_2mhz[] = "1.9968MHz";
static const char *basecstr[2] = {str_2halfmhz, str_2mhz};
static const UINT32 mulval[10] = {1, 2, 4, 5, 6, 8, 10, 12, 16, 20};
static const char str_clockfmt[] = "%2u.%.4u";


static void setclock(HWND hWnd, UINT multiple) {

	UINT32	clock;
	char	work[32];

	GetDlgItemText(hWnd, IDC_BASECLOCK, work, sizeof(work));
	if (work[0] == '1') {
		clock = PCBASECLOCK20 / 100;
	}
	else {
		clock = PCBASECLOCK25 / 100;
	}
	if (multiple == 0) {
		GetDlgItemText(hWnd, IDC_MULTIPLE, work, sizeof(work));
		multiple = (UINT)milstr_solveINT(work);
	}
	if (multiple < 1) {
		multiple = 1;
	}
	else if (multiple > 32) {
		multiple = 32;
	}
	clock *= multiple;
	wsprintf(work, str_clockfmt, clock / 10000, clock % 10000);
	SetDlgItemText(hWnd, IDC_CLOCKMSG, work);
}

LRESULT CALLBACK CfgDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	char	work[128];
	BYTE	bval;
	WORD	wval;
	UINT32	dval;
	UINT	val;
	UINT	update;

	switch (msg) {
		case WM_INITDIALOG:
			SETLISTSTR(hWnd, IDC_BASECLOCK, basecstr);
			if (np2cfg.baseclock >= AVE(PCBASECLOCK25, PCBASECLOCK20)) {
				val = 0;
			}
			else {
				val = 1;
			}
			SendDlgItemMessage(hWnd, IDC_BASECLOCK, CB_SETCURSEL, val, 0);
			SETLISTUINT32(hWnd, IDC_MULTIPLE, mulval);
			wsprintf(work, str_u, np2cfg.multiple);
			SetDlgItemText(hWnd, IDC_MULTIPLE, work);

			if (np2cfg.samplingrate < AVE(11025, 22050)) {
				wval = IDC_RATE11;
			}
			else if (np2cfg.samplingrate < AVE(22050, 44100)) {
				wval = IDC_RATE22;
			}
			else {
				wval = IDC_RATE44;
			}
			SetDlgItemCheck(hWnd, wval, TRUE);
			wsprintf(work, str_u, np2cfg.delayms);
			SetDlgItemText(hWnd, IDC_SOUNDBUF, work);

			if (mmxflag & MMXFLAG_NOTSUPPORT) {
				EnableWindow(GetDlgItem(hWnd, IDC_DISABLEMMX), FALSE);
				SetDlgItemCheck(hWnd, IDC_DISABLEMMX, TRUE);
			}
			else {
				SetDlgItemCheck(hWnd, IDC_DISABLEMMX, np2oscfg.disablemmx);
			}
			SetDlgItemCheck(hWnd, IDC_COMFIRM, np2oscfg.comfirm);
			SetDlgItemCheck(hWnd, IDC_RESUME, np2oscfg.resume);
			setclock(hWnd, 0);
			SetFocus(GetDlgItem(hWnd, IDC_BASECLOCK));
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					update = 0;
					GetDlgItemText(hWnd, IDC_BASECLOCK, work, sizeof(work));
					if (work[0] == '1') {
						dval = PCBASECLOCK20;
					}
					else {
						dval = PCBASECLOCK25;
					}
					if (np2cfg.baseclock != dval) {
						np2cfg.baseclock = dval;
						update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
					}

					GetDlgItemText(hWnd, IDC_MULTIPLE, work, sizeof(work));
					val = (UINT)milstr_solveINT(work);
					if (val < 1) {
						val = 1;
					}
					else if (val > 32) {
						val = 32;
					}
					if (np2cfg.multiple != val) {
						np2cfg.multiple = val;
						update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
					}

					if (GetDlgItemCheck(hWnd, IDC_RATE11)) {
						wval = 11025;
					}
					else if (GetDlgItemCheck(hWnd, IDC_RATE22)) {
						wval = 22050;
					}
					else {
						wval = 44100;
					}
					if (np2cfg.samplingrate != wval) {
						np2cfg.samplingrate = wval;
						update |= SYS_UPDATECFG | SYS_UPDATERATE;
						soundrenewal = 1;
					}

					GetDlgItemText(hWnd, IDC_SOUNDBUF, work, sizeof(work));
					val = (UINT)milstr_solveINT(work);
					if (val < 100) {
						val = 100;
					}
					else if (val > 1000) {
						val = 1000;
					}
					if (np2cfg.delayms != (UINT16)val) {
						soundrenewal = 1;
						np2cfg.delayms = (UINT16)val;
						update |= SYS_UPDATECFG | SYS_UPDATESBUF;
					}

					if (!(mmxflag & MMXFLAG_NOTSUPPORT)) {
						bval = GetDlgItemCheck(hWnd, IDC_DISABLEMMX);
						if (np2oscfg.disablemmx != bval) {
							np2oscfg.disablemmx = bval;
							mmxflag &= ~MMXFLAG_DISABLE;
							mmxflag |= (bval)?MMXFLAG_DISABLE:0;
							update |= SYS_UPDATEOSCFG;
						}
					}

					bval = GetDlgItemCheck(hWnd, IDC_COMFIRM);
					if (np2oscfg.comfirm != bval) {
						np2oscfg.comfirm = bval;
						update |= SYS_UPDATEOSCFG;
					}

					bval = GetDlgItemCheck(hWnd, IDC_RESUME);
					if (np2oscfg.resume != bval) {
						np2oscfg.resume = bval;
						update |= SYS_UPDATEOSCFG;
					}
					sysmng_update(update);
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

				case IDC_BASECLOCK:
					setclock(hWnd, 0);
					return(FALSE);

				case IDC_MULTIPLE:
					if (HIWORD(wp) == CBN_SELCHANGE) {
						val = (UINT)SendDlgItemMessage(hWnd, IDC_MULTIPLE,
														CB_GETCURSEL, 0, 0);
						if (val < sizeof(mulval)/sizeof(UINT32)) {
							setclock(hWnd, mulval[val]);
						}
					}
					else {
						setclock(hWnd, 0);
					}
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

