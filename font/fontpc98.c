#include	"compiler.h"
#include	"bmpdata.h"
#include	"dosio.h"
#include	"memory.h"
#include	"font.h"
#include	"fontdata.h"


#define	FONTYSIZE		16

#define	BMPWIDTH		2048L
#define	BMPHEIGHT		2048L

#define	BMPLINESIZE		(BMPWIDTH / 8)			// 割り切れる^^;

#define	BMPDATASIZE		(BMPLINESIZE * BMPHEIGHT)


static void pc98ankcpy(BYTE *dst, const BYTE *src, int from, int to) {

	int		y;
const BYTE	*p;
	int		ank;

	for (ank=from; ank<to; ank++) {

		// ANKフォントのスタート位置
		p = src + BMPDATASIZE + ank + (0 * FONTYSIZE * BMPLINESIZE);
		for (y=0; y<FONTYSIZE; y++) {
			p -= BMPLINESIZE;				// BMPなのでポインタは引かれる
			*dst++ = ~(*p);
		}
	}
}

static void pc98knjcpy(BYTE *dst, const BYTE *src, int from, int to) {

	int		i, j, k;
const BYTE	*p;
	BYTE	*q;

	for (i=from; i<to; i++) {
		p = src + BMPDATASIZE + (i << 1) - (FONTYSIZE * BMPLINESIZE);
		q = dst + 0x1000 + (i << 4);
		for (j=1; j<0x80; j++) {
			for (k=0; k<16; k++) {
				p -= BMPLINESIZE;
				*(q + 0x800) = ~(*(p+1));
				*q++ = ~(*p);
			}
			q += 0x1000 - 16;
		}
	}
}

BYTE fontpc98_read(const char *filename, BYTE loading) {

	FILEH	fh;
	BMPFILE	bf;
	BMPINFO	bi;
	BYTE	*bmpdata;
	BMPDATA	bd;
	long	fptr;

	if (!(loading & FONTLOAD_16)) {
		goto fr98_err1;
	}

	// ファイルをオープン
	fh = file_open_rb(filename);
	if (fh == FILEH_INVALID) {
		goto fr98_err1;
	}

	// BITMAPFILEHEADER の読み込み
	if ((file_read(fh, &bf, sizeof(bf)) != sizeof(bf)) ||
		(bf.bfType[0] != 'B') || (bf.bfType[1] != 'M')) {
		goto fr98_err2;
	}

	// BITMAPINFOHEADER の読み込み
	if ((file_read(fh, &bi, sizeof(bi)) != sizeof(bi)) ||
		(bmpdata_getinfo(&bi, &bd) != SUCCESS) ||
		(bd.width != BMPWIDTH) || (bd.height != BMPHEIGHT) || (bd.bpp != 1) ||
		(LOADINTELDWORD(bi.biSizeImage) != BMPDATASIZE)) {
		goto fr98_err2;
	}

	// BITMAPデータ頭だし
	fptr = LOADINTELDWORD(bf.bfOffBits);
	if (file_seek(fh, fptr, FSEEK_SET) != fptr) {
		goto fr98_err2;
	}

	// メモリアロケート
	bmpdata = (BYTE *)_MALLOC(BMPDATASIZE, "pc98font");
	if (bmpdata == NULL) {
		goto fr98_err2;
	}

	// BITMAPデータの読みだし
	if (file_read(fh, bmpdata, BMPDATASIZE) != BMPDATASIZE) {
		goto fr98_err3;
	}

	// 8x16 フォント(～0x7f)を読む必要がある？
	if (loading & FONT_ANK16a) {
		loading &= ~FONT_ANK16a;
		pc98ankcpy(fontrom + 0x80000, bmpdata, 0x000, 0x080);
	}
	// 8x16 フォント(0x80～)を読む必要がある？
	if (loading & FONT_ANK16b) {
		loading &= ~FONT_ANK16b;
		pc98ankcpy(fontrom + 0x80800, bmpdata, 0x080, 0x100);
	}

	// 第一水準漢字を読む必要がある？
	if (loading & FONT_KNJ1) {
		loading &= ~FONT_KNJ1;
		pc98knjcpy(fontrom, bmpdata, 0x01, 0x30);
	}
	// 第二水準漢字を読む必要がある？
	if (loading & FONT_KNJ2) {
		loading &= ~FONT_KNJ2;
		pc98knjcpy(fontrom, bmpdata, 0x30, 0x56);
	}
	// 拡張漢字を読む必要がある？
	if (loading & FONT_KNJ3) {
		loading &= ~FONT_KNJ3;
		pc98knjcpy(fontrom, bmpdata, 0x58, 0x60);
	}

fr98_err3:
	_MFREE(bmpdata);

fr98_err2:
	file_close(fh);

fr98_err1:
	return(loading);
}

