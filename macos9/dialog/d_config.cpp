#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"


static const char str_clockfmt[] = "%2u.%.4u";

static void setbase(ControlHandle *btn, UINT base) {

	SetControlValue(btn[0], (base == PCBASECLOCK20));
	SetControlValue(btn[1], (base == PCBASECLOCK25));
}

static void setclock(Handle hdl, UINT base, UINT multiple) {

	UINT	clk;
	char	work[32];
	Str255	clockstr;

	clk = base / 100;
	clk *= multiple;
	SPRINTF(work, str_clockfmt, clk / 10000, clk % 10000);
	mkstr255(clockstr, work);
	SetDialogItemText(hdl, clockstr);
}

static void setrate(ControlHandle *btn, UINT rate) {

	SetControlValue(btn[0], (rate == 11025));
	SetControlValue(btn[1], (rate == 22050));
	SetControlValue(btn[2], (rate == 44100));
}


void ConfigDialogProc(void) {

	DialogPtr		hDlg;
	ControlHandle	basebtn[2];
	ControlHandle	ratebtn[3];
	ControlHandle	resumebtn;
	char			work[32];
	Str255			workstr;
	UINT			base;
	UINT			multiple;
	UINT			rate;
	UINT			ms;
	UINT			resume;
	int				done;
	short			item;
	UINT			update;

	hDlg = GetNewDialog(IDD_CONFIGURE, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	basebtn[0] = (ControlHandle)GetDlgItem(hDlg, IDC_BASECLOCK20);
	basebtn[1] = (ControlHandle)GetDlgItem(hDlg, IDC_BASECLOCK25);
	ratebtn[0] = (ControlHandle)GetDlgItem(hDlg, IDC_RATE11);
	ratebtn[1] = (ControlHandle)GetDlgItem(hDlg, IDC_RATE22);
	ratebtn[2] = (ControlHandle)GetDlgItem(hDlg, IDC_RATE44);
	resumebtn = (ControlHandle)GetDlgItem(hDlg, IDC_RESUME);

	if (np2cfg.baseclock < ((PCBASECLOCK20 + PCBASECLOCK25) / 2)) {
		base = PCBASECLOCK20;
	}
	else {
		base = PCBASECLOCK25;
	}
	setbase(basebtn, base);
	multiple = np2cfg.multiple;
	if (multiple < 1) {
		multiple = 1;
	}
	else if (multiple > 32) {
		multiple = 32;
	}
	SPRINTF(work, str_u, multiple);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_MULTIPLE), workstr);
	setclock(GetDlgItem(hDlg, IDC_CLOCKMSG), base, multiple);

	rate = np2cfg.samplingrate;
	if ((rate != 0) && (rate != 11025) && (rate != 44100)) {
		rate = 22050;
	}
	setrate(ratebtn, rate);
	ms = np2cfg.delayms;
	if (ms < 50) {
		ms = 50;
	}
	else if (ms > 1000) {
		ms = 1000;
	}
	SPRINTF(work, str_u, ms);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_SOUNDBUF), workstr);

	resume = (np2oscfg.resume)?1:0;
	SetControlValue(resumebtn, resume);

	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				update = 0;
				if (np2cfg.baseclock != base) {
					np2cfg.baseclock = base;
					update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
				}
				if (np2cfg.multiple != multiple) {
					np2cfg.multiple = multiple;
					update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
				}
				if (np2cfg.samplingrate != rate) {
					np2cfg.samplingrate = rate;
					update |= SYS_UPDATECFG | SYS_UPDATERATE;
					soundrenewal = 1;
				}
				GetDialogItemText(GetDlgItem(hDlg, IDC_SOUNDBUF), workstr);
				mkcstr(work, sizeof(work), workstr);
				ms = milstr_solveINT(work);
				if (ms < 50) {
					ms = 50;
				}
				else if (ms > 1000) {
					ms = 1000;
				}
				if (np2cfg.delayms != ms) {
					soundrenewal = 1;
					np2cfg.delayms = ms;
					update |= SYS_UPDATECFG | SYS_UPDATESBUF;
				}
				if (np2oscfg.resume != resume) {
					np2oscfg.resume = resume;
					update |= SYS_UPDATEOSCFG;
				}
				sysmng_update(update);
				done = IDOK;
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;

			case IDC_BASECLOCK20:
				base = PCBASECLOCK20;
				setbase(basebtn, base);
				setclock(GetDlgItem(hDlg, IDC_CLOCKMSG), base, multiple);
				break;

			case IDC_BASECLOCK25:
				base = PCBASECLOCK25;
				setbase(basebtn, base);
				setclock(GetDlgItem(hDlg, IDC_CLOCKMSG), base, multiple);
				break;

			case IDC_MULTIPLE:
				GetDialogItemText(GetDlgItem(hDlg, IDC_MULTIPLE), workstr);
				mkcstr(work, sizeof(work), workstr);
				multiple = milstr_solveINT(work);
				if (multiple < 1) {
					multiple = 1;
				}
				else if (multiple > 32) {
					multiple = 32;
				}
				setclock(GetDlgItem(hDlg, IDC_CLOCKMSG), base, multiple);
				break;

			case IDC_RATE11:
				rate = 11025;
				setrate(ratebtn, rate);
				break;

			case IDC_RATE22:
				rate = 22050;
				setrate(ratebtn, rate);
				break;

			case IDC_RATE44:
				rate = 44100;
				setrate(ratebtn, rate);
				break;

			case IDC_RESUME:
				resume ^= 1;
				SetControlValue(resumebtn, resume);
				break;
		}
	}
	DisposeDialog(hDlg);
}

