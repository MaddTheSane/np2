extern char viewcmn_hex[16];

void viewcmn_caption(NP2VIEW_T *view, char *buf);
void viewcmn_putcaption(NP2VIEW_T *view);


BOOL viewcmn_alloc(VIEWMEMBUF *buf, DWORD size);
void viewcmn_free(VIEWMEMBUF *buf);

NP2VIEW_T *viewcmn_find(HWND hwnd);
void viewcmn_setmode(NP2VIEW_T *dst, NP2VIEW_T *src, BYTE type);
LRESULT CALLBACK viewcmn_dispat(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
void viewcmn_setbank(NP2VIEW_T *view);


void viewcmn_setmenuseg(HWND hwnd);

void viewcmn_setvscroll(HWND hWnd, NP2VIEW_T *view);

void viewcmn_paint(NP2VIEW_T *view, DWORD bkgcolor,
						void (*callback)(NP2VIEW_T *view, RECT *rc, HDC hdc));
