#include	"compiler.h"
#include	"codecnv.h"


UINT codecnv_ucs2toutf8(char *dst, UINT dcnt, const UINT16 *src, UINT scnt) {

	UINT	orgdcnt;
	UINT	c;

	(void)scnt;
	orgdcnt = dcnt;
	if (dcnt == 0) {
		dst = NULL;
	}

	dcnt--;
	while(dcnt) {
		c = *src++;
		if (!c) {
			break;
		}
		else if (c < 0x80) {
			dcnt--;
			if (dst) {
				dst[0] = (char)c;
				dst += 1;
			}
		}
		else if (c < 0x800) {
			if (dcnt < 2) {
				break;
			}
			dcnt -= 2;
			if (dst) {
				dst[0] = (char)(0xc0 + ((c >> 6) & 0x1f));
				dst[1] = (char)(0x80 + ((c >> 0) & 0x3f));
				dst += 2;
			}
		}
		else {
			if (dcnt < 3) {
				break;
			}
			dcnt -= 3;
			if (dst) {
				dst[0] = (char)(0xe0 + ((c >> 12) & 0x0f));
				dst[1] = (char)(0x80 + ((c >> 6) & 0x3f));
				dst[2] = (char)(0x80 + ((c >> 0) & 0x3f));
				dst += 3;
			}
		}
	}
	if (dst) {
		dst[0] = '\0';
	}
	return((UINT)(orgdcnt - dcnt));
}

