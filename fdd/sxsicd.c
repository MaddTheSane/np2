#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"sxsi.h"


static const UINT8 cd001[7] = {0x01,'C','D','0','0','1',0x01};


// ---- 後でまとめる。

// ---- セクタ2048

static long issec2048(FILEH fh) {

	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
	UINT	fsize;

	fpos = 16 * 2048;
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec2048_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec2048_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec2048_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec2048_err;
	}
	fsize = file_getsize(fh);
	if ((fsize % 2048) != 0) {
		goto sec2048_err;
	}
	return(fsize / 2048);

sec2048_err:
	return(-1);
}

static REG8 sec2048_read(SXSIDEV sxsi, long pos, UINT8 *buf, UINT size) {

	UINT	rsize;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}
	pos = pos * 2048;
	if (file_seek((FILEH)sxsi->fh, pos, FSEEK_SET) != pos) {
		return(0xd0);
	}
	while(size) {
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read((FILEH)sxsi->fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
	}
	return(0x00);
}


// ---- セクタ2352

static long issec2352(FILEH fh) {

	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
	UINT	fsize;

	fpos = (16 * 2352) + 16;
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec2352_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec2352_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec2352_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec2352_err;
	}
	fsize = file_getsize(fh);
	if ((fsize % 2352) != 0) {
		goto sec2352_err;
	}
	return(fsize / 2352);

sec2352_err:
	return(-1);
}

static REG8 sec2352_read(SXSIDEV sxsi, long pos, UINT8 *buf, UINT size) {

	long	fpos;
	UINT	rsize;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}
	while(size) {
		fpos = (pos * 2352) + 16;
		if (file_seek((FILEH)sxsi->fh, fpos, FSEEK_SET) != fpos) {
			return(0xd0);
		}
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read((FILEH)sxsi->fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
		pos++;
	}
	return(0x00);
}


// ----

static REG8 cd_write(SXSIDEV sxsi, long pos, const UINT8 *buf, UINT size) {

	(void)sxsi;
	(void)pos;
	(void)buf;
	(void)size;

	return(0x60);
}

static REG8 cd_format(SXSIDEV sxsi, long pos) {

	(void)sxsi;
	(void)pos;

	return(0x60);
}


// ----

BRESULT sxsicd_open(SXSIDEV sxsi, const OEMCHAR *file) {

	FILEH		fh;
	long		totals;

	fh = file_open(file);
	if (fh == FILEH_INVALID) {
		goto sxsiope_err1;
	}
	totals = issec2048(fh);
	if (totals >= 0) {
		sxsi->fh = (INTPTR)fh;
		sxsi->read = sec2048_read;
		sxsi->write = cd_write;
		sxsi->format = cd_format;

		sxsi->totals = totals;
		sxsi->cylinders = 0;
		sxsi->size = 2048;
		sxsi->sectors = 1;
		sxsi->surfaces = 1;
		sxsi->headersize = 0;
		sxsi->mediatype = SXSIMEDIA_DATA;
		return(SUCCESS);
	}
	totals = issec2352(fh);
	if (totals >= 0) {
		sxsi->fh = (INTPTR)fh;
		sxsi->read = sec2352_read;
		sxsi->write = cd_write;
		sxsi->format = cd_format;

		sxsi->totals = totals;
		sxsi->cylinders = 0;
		sxsi->size = 2048;
		sxsi->sectors = 1;
		sxsi->surfaces = 1;
		sxsi->headersize = 0;
		sxsi->mediatype = SXSIMEDIA_DATA;
		return(SUCCESS);
	}
	file_close(fh);

sxsiope_err1:
	return(FAILURE);
}

