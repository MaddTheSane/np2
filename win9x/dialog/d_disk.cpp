#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"newdisk.h"


static const char fddui_title[] = "Select floppy image";
static const char fddui_filter[] =										\
					"D88 image files (*.D88;*.D98)\0"					\
									"*.d88;*.88d;*.d98;*.98d\0"			\
					"Floppy disk images (*.XDF;*.HDM;*.DUP;*.2HD)\0"	\
									"*.xdf;*.hdm;*.dup;*.2hd;*.tfd\0"	\
					"All supported Files\0"								\
									"*.d88;*.88d;*.d98;*.98d;"			\
									"*.xdf;*.hdm;*.dup;*.2hd;*.tfd\0"	\
					"All files (*.*)\0*.*\0";
static const FILESEL fddui = {fddui_title, str_d88, fddui_filter, 3};

static const char hddui_title[] = "Select HDD image";
static const char hddui_filter[] =										\
					"T98 harddisk image files (*.THD)\0"				\
									"*.thd\0"							\
					"Anex86 harddisk image files (*.HDI)\0"				\
									"*.hdi\0"							\
					"All supported Files\0"								\
									"*.thd;*.hdi\0";
static const FILESEL hddui = {hddui_title, str_thd, hddui_filter, 3};

static const char newdisk_title[] = "Create disk image";
static const char newdisk_filter[] =									\
					"D88 image files (*.D88;*.88D)\0"					\
										"*.d88;*.88d\0"					\
					"T98 harddisk image files (*.THD)\0"				\
										"*.thd\0";
static const FILESEL newdiskui = {newdisk_title, str_d88, newdisk_filter, 1};


// ----

void dialog_changefdd(HWND hWnd, BYTE drv) {

const char	*p;
	int		readonly;

	if (drv < 4) {
		p = dlgs_selectfile(hWnd, &fddui, fdd_diskname(drv),
									fddfolder, sizeof(fddfolder), &readonly);
		if (p != NULL) {
			diskdrv_setfdd(drv, p, readonly);
		}
	}
}

void dialog_changehdd(HWND hWnd, BYTE drv) {

const char *p;

	if (drv < 2) {
		p = dlgs_selectfile(hWnd, &hddui, np2cfg.hddfile[drv],
									hddfolder, sizeof(hddfolder), NULL);
		if (p != NULL) {
			diskdrv_sethdd(drv, p);
		}
	}
}


// ---- newdisk

static const char str_newdisk[] = "newdisk";
static const UINT32 hddsizetbl[5] = {20, 41, 65, 80, 128};

static	BYTE	makefdtype = DISKTYPE_2HD << 4;
static	char	disklabel[16+1];
static	UINT	hddsize;

static LRESULT CALLBACK NewHddDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	UINT	val;
	char	work[32];

	switch (msg) {
		case WM_INITDIALOG:
			SETLISTUINT32(hWnd, IDC_HDDSIZE, hddsizetbl);
			SetFocus(GetDlgItem(hWnd, IDC_HDDSIZE));
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					GetWindowText(GetDlgItem(hWnd, IDC_HDDSIZE),
													work, sizeof(work));
					val = (UINT)milstr_solveINT(work);
					if (val < 0) {
						val = 0;
					}
					else if (val > 512) {
						val = 512;
					}
					hddsize = val;
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

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

static LRESULT CALLBACK NewdiskDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	WORD	res;

	switch (msg) {
		case WM_INITDIALOG:
			switch(makefdtype) {
				case (DISKTYPE_2DD << 4):
					res = IDC_MAKE2DD;
					break;

				case (DISKTYPE_2HD << 4):
					res = IDC_MAKE2HD;
					break;

				default:
					res = IDC_MAKE144;
					break;
			}
			SetDlgItemCheck(hWnd, res, 1);
			SetFocus(GetDlgItem(hWnd, IDC_DISKLABEL));
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					GetWindowText(GetDlgItem(hWnd, IDC_DISKLABEL),
											disklabel, sizeof(disklabel));
					if (milstr_kanji1st(disklabel, sizeof(disklabel) - 1)) {
						disklabel[sizeof(disklabel) - 1] = '\0';
					}
					if (GetDlgItemCheck(hWnd, IDC_MAKE2DD)) {
						makefdtype = (DISKTYPE_2DD << 4);
					}
					else if (GetDlgItemCheck(hWnd, IDC_MAKE2HD)) {
						makefdtype = (DISKTYPE_2HD << 4);
					}
					else {
						makefdtype = (DISKTYPE_2HD << 4) + 1;
					}
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

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

void dialog_newdisk(HWND hWnd) {

	char		path[MAX_PATH];
const char		*p;
	HINSTANCE	hinst;

	file_cpyname(path, fddfolder, sizeof(path));
	file_cutname(path);
	file_catname(path, str_newdisk, sizeof(path));

	p = dlgs_selectwritefile(hWnd, &newdiskui, path, NULL, 0);
	if (p == NULL) {
		return;
	}
	hinst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	if (!file_cmpname(file_getext((char *)p), str_thd)) {
		hddsize = 0;
		if (DialogBox(hinst, MAKEINTRESOURCE(IDD_NEWHDDDISK),
									hWnd, (DLGPROC)NewHddDlgProc) == IDOK) {
			newdisk_hdd(p, hddsize);	// (hddsize < 5) || (hddsize > 256)
		}
	}
	else {
		if (DialogBox(hinst,
				MAKEINTRESOURCE(np2cfg.usefd144?IDD_NEWDISK2:IDD_NEWDISK),
									hWnd, (DLGPROC)NewdiskDlgProc) == IDOK) {
			newdisk_fdd(p, makefdtype, disklabel);
		}
	}
}

