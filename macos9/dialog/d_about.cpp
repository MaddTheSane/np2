#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"dialog.h"
#include	"pccore.h"


void AboutDialogProc(void) {

	DialogPtr	hDlg;
	int			done;
	short		item;
	Str255		ver;
	Str255		dummy;

	hDlg = GetNewDialog(IDD_ABOUT, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	mkstr255(ver, np2version);
	mkstr255(dummy, str_null);
	ParamText(ver, dummy, dummy, dummy);
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

