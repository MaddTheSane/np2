#include	"compiler.h"
#include	"bmpdata.h"


UINT bmpdata_getalign(const BMPINFO *bi) {

	UINT	ret;
	int		width;
	int		bit;

	width = LOADINTELDWORD(bi->biWidth);
	bit = LOADINTELWORD(bi->biBitCount);
	ret = ((width * bit) + 7) / 8;
	ret = (ret + 3) & (~3);
	return(ret);
}

UINT bmpdata_getdatasize(const BMPINFO *bi) {

	int		height;

	height = (SINT32)LOADINTELDWORD(bi->biHeight);
	if (height < 0) {
		height = 0 - height;
	}
	return(height * bmpdata_getalign(bi));
}

UINT bmpdata_sethead(BMPFILE *bf, const BMPINFO *bi) {

	UINT	ret;
	UINT	bit;
	UINT	pal;

	ret = 0;
	if (bi == NULL) {
		goto bdsh_exit;
	}
	ret = sizeof(BMPFILE) + sizeof(BMPINFO);
	bit = LOADINTELWORD(bi->biBitCount);
	if (bit < 8) {
		pal = LOADINTELDWORD(bi->biClrUsed);
		pal = min(pal, (UINT)(1 << bit));
		ret += pal * 4;
	}
	if (bf) {
		ZeroMemory(bf, sizeof(BMPFILE));
		bf->bfType[0] = 'B';
		bf->bfType[1] = 'M';
		STOREINTELDWORD(bf->bfOffBits, ret);
	}
	ret += bmpdata_getdatasize(bi);

bdsh_exit:
	return(ret);
}

UINT bmpdata_setinfo(BMPINFO *bi, const BMPDATA *inf) {

	UINT	ret;
	UINT	tmp;

	ret = 0;
	if ((bi == NULL) && (inf == NULL)) {
		goto bdsi_exit;
	}
	ZeroMemory(bi, sizeof(BMPINFO));
	STOREINTELDWORD(bi->biSize, sizeof(BMPINFO));
	STOREINTELDWORD(bi->biWidth, inf->width);
	STOREINTELDWORD(bi->biHeight, inf->height);
	STOREINTELWORD(bi->biPlanes, 1);
	STOREINTELWORD(bi->biBitCount, inf->bpp);
//	STOREINTELDWORD(bi->biCompression, BI_RGB);
	ret = bmpdata_getdatasize(bi);
	STOREINTELDWORD(bi->biSizeImage, ret);
//	STOREINTELDWORD(bi->biXPelsPerMeter, 0);
//	STOREINTELDWORD(bi->biYPelsPerMeter, 0);
	if (inf->bpp <= 8) {
		tmp = 1 << inf->bpp;
		STOREINTELDWORD(bi->biClrUsed, tmp);
		STOREINTELDWORD(bi->biClrImportant, tmp);
	}

bdsi_exit:
	return(ret);
}

BOOL bmpdata_getinfo(const BMPINFO *bi, BMPDATA *inf) {

	UINT	tmp;
	int		width;
	int		height;

	if ((bi == NULL) || (inf == NULL)) {
		goto bdgi_err;
	}

	tmp = LOADINTELDWORD(bi->biSize);
	if (tmp != sizeof(BMPINFO)) {
		goto bdgi_err;
	}
	tmp = LOADINTELWORD(bi->biPlanes);
	if (tmp != 1) {
		goto bdgi_err;
	}
	tmp = LOADINTELDWORD(bi->biCompression);
	if (tmp != 0) {
		goto bdgi_err;
	}
	width = LOADINTELDWORD(bi->biWidth);
	height = LOADINTELDWORD(bi->biHeight);
	if ((width <= 0) || (height == 0)) {
		goto bdgi_err;
	}
	inf->width = width;
	inf->height = height;
	inf->bpp = LOADINTELWORD(bi->biBitCount);
	return(SUCCESS);

bdgi_err:
	return(FAILURE);
}

