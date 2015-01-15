
extern const char viewcmn_hex[16];

void viewcmn_putcaption(NP2VIEW_T *view);

BOOL viewcmn_alloc(VIEWMEMBUF *buf, UINT32 size);
void viewcmn_free(VIEWMEMBUF *buf);

void viewcmn_setmode(NP2VIEW_T *dst, NP2VIEW_T *src, UINT8 type);
LRESULT CALLBACK viewcmn_dispat(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void viewcmn_setmenuseg(HWND hwnd);

void viewcmn_setvscroll(HWND hWnd, NP2VIEW_T *view);

void viewcmn_paint(NP2VIEW_T *view, UINT32 bkgcolor,
						void (*callback)(NP2VIEW_T *view, RECT *rc, HDC hdc));

