
typedef struct {
	int		x;
	int		y;
	RGB32	*pal;
	UINT	pals;
const BYTE	*data;
} BIT2RES;


extern const BIT2RES snd26dip;
extern const BIT2RES snd86dip;
extern const BIT2RES spbdip;
extern const BIT2RES mpudip;
extern const BIT2RES pc9861dip;

int	bit2res_getsize(const BIT2RES *p);
void bit2res_sethead(BITMAPINFO *bi, const BIT2RES *p);
void bit2res_setdata(BYTE *dat, const BIT2RES *p);

