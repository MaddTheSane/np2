
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

void np2class_wmcreate(HWND hWnd);
void np2class_wmdestroy(HWND hWnd);
void np2class_enablemenu(HWND hWnd, BOOL enable);
void np2class_windowtype(HWND hWnd, UINT8 type);
void np2class_frametype(HWND hWnd, UINT8 thick);
HMENU np2class_gethmenu(HWND hWnd);

