
// 猫用、拡張クラス。


extern	const char np2dlgclass[];

void np2class_initialize(HINSTANCE hinst);
void np2class_deinitialize(void);

void np2class_move(HWND hWnd, int posx, int posy, int cx, int cy);
int CALLBACK np2class_propetysheet(HWND hWndDlg, UINT uMsg, LPARAM lParam);


enum {
	NP2GWL_HMENU	= 0,
	NP2GWL_SIZE		= 4
};

void np2class_windowtype(HWND hWnd, BYTE type);
HMENU np2class_gethmenu(HWND hWnd);
void np2class_destroymenu(HWND hWnd);

