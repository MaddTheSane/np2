#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"np2info.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"


static	RECT	aboutrct;
static const char np2infostr[] = 									\
						"CPU: !CPU !CLOCK\r\n"						\
						"MEM: !MEM1\r\n"							\
						"GDC: !GDC\r\n"								\
						"TEXT: !TEXT\r\n"							\
						"GRPH: !GRPH\r\n"							\
						"SOUND: !EXSND\r\n"							\
						"\r\n"										\
						"BIOS: !BIOS\r\n"							\
						"RHYTHM: !RHYTHM\r\n"						\
						"\r\n"										\
						"SCREEN: !DISP";


// ----

static void about_init(HWND hWnd) {

	char	work[128];

	strcpy(work, "Neko Project II  ");
	strcat(work, np2version);
#ifdef NEW286
	strcat(work, "+");
#endif
	SetDlgItemText(hWnd, IDC_NP2VER, work);
	GetWindowRect(hWnd, &aboutrct);
	MoveWindow(hWnd, aboutrct.left, aboutrct.top,
								aboutrct.right - aboutrct.left, 84, TRUE);
	SetFocus(GetDlgItem(hWnd, IDOK));
}

static void about_more(HWND hWnd) {

	char	infostr[1024];

	np2info(infostr, np2infostr, sizeof(infostr));
	SetDlgItemText(hWnd, IDC_NP2INFO, infostr);
	EnableWindow(GetDlgItem(hWnd, IDC_MORE), FALSE);
	MoveWindow(hWnd, aboutrct.left, aboutrct.top,
							aboutrct.right - aboutrct.left,
							aboutrct.bottom - aboutrct.top, TRUE);
	SetFocus(GetDlgItem(hWnd, IDOK));
}

LRESULT CALLBACK AboutDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			ShowWindow(GetDlgItem(hWnd, IDC_EXTICON), SW_HIDE);
			about_init(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					EndDialog(hWnd, IDOK);
					break;
				case IDC_MORE:
					about_more(hWnd);
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

