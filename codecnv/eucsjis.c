#include	"compiler.h"
#include	"codecnv.h"


UINT codecnv_euctosjis(char *dst, UINT dcnt, const char *src, UINT scnt) {

	UINT	orgdcnt;
	UINT	h;
	UINT	l;

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
		h = (UINT8)*src++;
		if (h < 0x80) {				// ascii
			if (!h) {
				break;
			}
			dcnt--;
			if (dst) {
				dst[0] = (char)h;
				dst++;
			}
		}
		else if (h == 0x8e) {
			l = (UINT8)*src++;
			if (!l) {
				break;
			}
			dcnt--;
			if (dst) {
				dst[0] = (char)h;
				dst++;
			}
		}
		else {
			l = (UINT8)*src++;
			if (!l) {
				break;
			}
			if (dcnt < 2) {
				break;
			}
			dcnt -= 2;
			if (dst) {
				h &= 0x7f;
				l &= 0x7f;
				l += ((h & 1) - 1) & 0x5e;
				if (l >= 0x60) {
					l++;
				}
				h += 0x121;
				l += 0x1f;
				h >>= 1;
				h ^= 0x20;
				dst[0] = (char)h;
				dst[1] = (char)l;
				dst += 2;
			}
		}
	}
	if (dst) {
		dst[0] = '\0';
	}
	return((UINT)(orgdcnt - dcnt));
}

