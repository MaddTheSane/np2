
typedef struct {
	UINT	flag;
	int		gx;
	int		gy;
	int		tx;
	int		ty;
} WINLOC;

void winloc_movingstart(WINLOC *wl);
void winloc_movingproc(WINLOC *wl, RECT *prc);


