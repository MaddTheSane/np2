#include	"compiler.h"
#include	"resource.h"
#include	"dialog.h"
#include	"dialogs.h"


void MPU98DialogProc(void) {

	DialogPtr	hDlg;
	int			done;
	short		item;

	hDlg = GetNewDialog(IDD_MPU98II, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	SetDialogDefaultItem(hDlg, IDOK);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				done = 1;
				break;
		}
	}
	DisposeDialog(hDlg);
}

