#include	"compiler.h"
#include	"ucscnv.h"


// ---- UCS2

UINT ucscnv_utf8toucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt) {

	UINT	orgdcnt;
	UINT	c;

	(void)scnt;

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

UINT ucscnv_ucs2toutf8(char *dst, UINT dcnt, const UINT16 *src, UINT scnt) {

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

