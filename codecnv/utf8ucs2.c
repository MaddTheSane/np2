#include	"compiler.h"
#include	"codecnv.h"


UINT codecnv_utf8toucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt) {

	UINT	orgdcnt;
	UINT	c;

	(void)scnt;
	if (src == NULL) {
		return(0);
	}

	orgdcnt = dcnt;
	if (dcnt == 0) {
		dst = NULL;
	}
	dcnt--;
	while(dcnt) {
		if (src[0] == '\0') {
			break;
		}
		else if (!(src[0] & 0x80)) {
			c = src[0];
			src += 1;
		}
		else if ((src[0] & 0xe0) == 0xc0) {
			if ((src[1] & 0xc0) != 0x80) {
				break;
			}
			c = ((src[0] & 0x1f) << 6) + (src[1] & 0x3f);
			src += 2;
		}
		else if ((src[0] & 0xf0) == 0xe0) {
			if (((src[1] & 0xc0) != 0x80) ||
				((src[2] & 0xc0) != 0x80)) {
				break;
			}
			c = ((src[0] & 0x0f) << 12) +
								((src[1] & 0x3f) << 6) + (src[2] & 0x3f);
			src += 3;
		}
		else {
			break;
		}
		dcnt--;
		if (dst) {
			dst[0] = (UINT16)c;
			dst += 1;
		}
	}
	if (dst) {
		dst[0] = '\0';
	}
	return((UINT)(orgdcnt - dcnt));
}

