
void winloc_setclientsize(HWND hwnd, int width, int height);


// ----

typedef struct {
	UINT	flag;
	int		gx;
	int		gy;
	int		tx;
	int		ty;
} WINLOC;

void winloc_movingstart(WINLOC *wl);
void winloc_movingproc(WINLOC *wl, RECT *rect);


// ----

typedef struct {
	HWND	hwnd;
	RECT	rect;
	BYTE	connect;
	BYTE	padding;
	UINT16	parent;
} WLEXWND;

typedef struct {
	UINT	flagx;
	UINT	flagy;
	int		gx;
	int		gy;
	int		tx;
	int		ty;
	HWND	base;
	RECT	rect;
	UINT	count;
} _WINLOCEX, *WINLOCEX;

WINLOCEX winlocex_create(HWND base, const HWND *child, UINT count);
void winlocex_destroy(WINLOCEX wle);
void winlocex_moving(WINLOCEX wle, RECT *rect);
void winlocex_move(WINLOCEX wle);
void winlocex_close(WINLOCEX wle);

