#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"


static const char str_np2title[] = "Neko Project II  ";


static void about_init(HWND hWnd) {

	char	work[128];

	milstr_ncpy(work, str_np2title, sizeof(work));
	milstr_ncat(work, np2version, sizeof(work));
	SetDlgItemText(hWnd, IDC_NP2VER, work);
	SetFocus(GetDlgItem(hWnd, IDOK));
}

LRESULT CALLBACK AboutDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			about_init(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					EndDialog(hWnd, IDOK);
					break;

				default:
					return(FALSE);
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDOK, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}

