#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"dosio.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"diskdrv.h"
#include	"newdisk.h"


void dialog_changefdd(BYTE drv) {

	char	fname[MAX_PATH];

	if (drv < 4) {
		if (dlgs_selectfile(fname, sizeof(fname))) {
			diskdrv_setfdd(drv, fname, 0);
		}
	}
}

void dialog_changehdd(BYTE drv) {

	char	fname[MAX_PATH];

	if (drv < 2) {
		if (dlgs_selectfile(fname, sizeof(fname))) {
			diskdrv_sethdd(drv, fname);
		}
	}
}


// ---- newdisk

static const char str_newdisk[] = "newdisk.d88";

typedef struct {
	BYTE	fdtype;
	char	label[16+1];
} NEWDISK;

static int NewHddDlgProc(UINT *hddsize) {

	DialogPtr	hDlg;
	Str255		sizestr;
	int			done;
	short		item;
	char		work[16];

	hDlg = GetNewDialog(IDD_NEWHDDDISK, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return(0);
	}

	mkstr255(sizestr, "41");
	SetDialogItemText(GetDlgItem(hDlg, IDC_HDDSIZE), sizestr);
	SelectDialogItemText(hDlg, IDC_HDDSIZE, 0x0000, 0x7fff);
	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				GetDialogItemText(GetDlgItem(hDlg, IDC_HDDSIZE), sizestr);
				mkcstr(work, sizeof(work), sizestr);
				*hddsize = milstr_solveINT(work);
				done = IDOK;
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;
		}
	}
	DisposeDialog(hDlg);
	return(done);
}

static int NewdiskDlgProc(NEWDISK *newdisk) {

	DialogPtr		hDlg;
	int				media;
	int				done;
	short			item;
	Str255			disklabel;
	ControlHandle	btn[2];

	hDlg = GetNewDialog(IDD_NEWFDDDISK, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return(0);
	}

	SelectDialogItemText(hDlg, IDC_DISKLABEL, 0x0000, 0x7fff);
	btn[0] = (ControlHandle)GetDlgItem(hDlg, IDC_MAKE2DD);
	btn[1] = (ControlHandle)GetDlgItem(hDlg, IDC_MAKE2HD);
	SetControlValue(btn[0], 0);
	SetControlValue(btn[1], 1);
	media = 1;
	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				if (media == 0) {
					newdisk->fdtype = (DISKTYPE_2DD << 4);
				}
				else if (media == 1) {
					newdisk->fdtype = (DISKTYPE_2HD << 4);
				}
				else {
					newdisk->fdtype = (DISKTYPE_2HD << 4) + 1;
				}
				GetDialogItemText(GetDlgItem(hDlg, IDC_DISKLABEL), disklabel);
				mkcstr(newdisk->label, sizeof(newdisk->label), disklabel);
				done = IDOK;
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;

			case IDC_DISKLABEL:
				break;

			case IDC_MAKE2DD:
				SetControlValue(btn[0], 1);
				SetControlValue(btn[1], 0);
				media = 0;
				break;

			case IDC_MAKE2HD:
				SetControlValue(btn[0], 0);
				SetControlValue(btn[1], 1);
				media = 1;
				break;
		}
	}
	DisposeDialog(hDlg);
	return(done);
}

void dialog_newdisk(void) {

	char	path[MAX_PATH];
	UINT	hddsize;
	NEWDISK	disk;

	if (!dlgs_selectwritefile(path, sizeof(path), str_newdisk)) {
		return;
	}
	if (!file_cmpname(file_getext(path), str_thd)) {
		hddsize = 0;
		if (NewHddDlgProc(&hddsize) == IDOK) {
			newdisk_thd(path, hddsize);
		}
	}
	else if (!file_cmpname(file_getext(path), str_hdd)) {
		hddsize = 0;
		if (NewHddDlgProc(&hddsize) == IDOK) {
			newdisk_vhd(path, hddsize);
		}
	}
	else {
		if (NewdiskDlgProc(&disk) == IDOK) {
			newdisk_fdd(path, disk.fdtype, disk.label);
		}
	}
}

