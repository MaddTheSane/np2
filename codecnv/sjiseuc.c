#include	"compiler.h"
#include	"codecnv.h"


UINT codecnv_sjistoeuc(char *dst, UINT dcnt, const char *src, UINT scnt) {

	UINT	orgdcnt;
	int		s;
	int		c;

	(void)scnt;			// ”»’è‚µ‚Ä‚È‚¢‚Ì‚©‚æ
	if (src == NULL) {
		return(0);
	}

	orgdcnt = dcnt;
	if (dcnt == 0) {
		dst = NULL;
	}
	dcnt--;
	while(dcnt) {
		s = (UINT8)*src++;
		if (s < 0x80) {				// ascii
			if (!s) {
				break;
			}
			dcnt--;
			if (dst) {
				dst[0] = (char)s;
				dst++;
			}
		}
		else if ((((s ^ 0x20) - 0xa1) & 0xff) < 0x2f) {
			c = (UINT8)*src++;
			if (!c) {
				break;
			}
			if (dcnt < 2) {
				break;
			}
			dcnt -= 2;
			if (dst) {
				c += 0x62 - ((c & 0x80) >> 7);
				if (c < 256) {
					c = (c - 0xa2) & 0x1ff;
				}
				c += 0x9fa1;
				dst[0] = (char)(((s & 0x3f) << 1) + (c >> 8));
				dst[1] = (char)c;
				dst += 2;
			}
		}
		else if (((s - 0xa0) & 0xff) < 0x40) {
			if (dcnt < 2) {
				break;
			}
			dcnt -= 2;
			if (dst) {
				dst[0] = (char)0x8e;
				dst[1] = (char)s;
				dst += 2;
			}
		}
	}
	if (dst) {
		dst[0] = '\0';
	}
	return((UINT)(orgdcnt - dcnt));
}

