#include	"compiler.h"
#include	"resource.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"


void AboutDialogProc(void) {

	DialogPtr	hDlg;
	Str255		verstr;
	int			done;
	short		item;

	hDlg = GetNewDialog(IDD_ABOUT, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	mkstr255(verstr, np2version);
	SetDialogItemText(GetDlgItem(hDlg, IDD_VERSION), verstr);
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

