#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fddfile.h"
#include	"fdd_xdf.h"


static const _XDFINFO supportxdf[] = {
#if 0
			// 256
			{154, 26, 1, DISKTYPE_2HD, 0},
			// 512
			{154, 15, 2, DISKTYPE_2HD, 0},
#endif
			// 1024
			{154,  8, 3, DISKTYPE_2HD, 0},
			// 1.44MB
			{160, 18, 2, DISKTYPE_2HD, 1},
};


BOOL fddxdf_set(FDDFILE fdd, const char *fname, int ro) {

const _XDFINFO	*xdf;
	short		attr;
	FILEH		fh;
	UINT		fdsize;
	UINT		size;

	attr = file_attr(fname);
	if (attr & 0x18) {
		return(FAILURE);
	}
	fh = file_open(fname);
	if (fh == FILEH_INVALID) {
		return(FAILURE);
	}
	fdsize = file_seek(fh, 0, FSEEK_END);
	file_close(fh);

	xdf = supportxdf;
	while(xdf < (supportxdf + (sizeof(supportxdf)/sizeof(_XDFINFO)))) {
		size = xdf->tracks;
		size *= xdf->sectors;
		size <<= (7 + xdf->n);
		if (size == fdsize) {
			milstr_ncpy(fdd->fname, fname, sizeof(fdd->fname));
			fdd->type = DISKTYPE_BETA;
			fdd->protect = ((attr & 1) || (ro))?TRUE:FALSE;
			fdd->inf.xdf = *xdf;
			return(SUCCESS);
		}
		xdf++;
	}
	return(FAILURE);
}

BOOL fddxdf_eject(FDDFILE fdd) {

	fdd->fname[0] = '\0';
	fdd->type = DISKTYPE_NOTREADY;
	return(SUCCESS);
}


BOOL fddxdf_diskaccess(FDDFILE fdd) {

	if (CTRL_FDMEDIA != DISKTYPE_2HD) {
		return(FAILURE);
	}
	(void)fdd;
	return(SUCCESS);
}

BOOL fddxdf_seek(FDDFILE fdd) {

	if ((!fdd->fname[0]) ||
		(fdd->type != DISKTYPE_BETA) ||
		(CTRL_FDMEDIA != DISKTYPE_2HD) ||
		(fdc.rpm != fdd->inf.xdf.rpm) ||
		(fdc.ncn >= (fdd->inf.xdf.tracks >> 1))) {
		return(FAILURE);
	}
	return(SUCCESS);
}

BOOL fddxdf_seeksector(FDDFILE fdd) {

	if ((!fdd->fname[0]) ||
		(fdd->type != DISKTYPE_BETA) ||
		(CTRL_FDMEDIA != DISKTYPE_2HD) ||
		(fdc.rpm != fdd->inf.xdf.rpm) ||
		(fdc.treg[fdc.us] >= (fdd->inf.xdf.tracks >> 1))) {
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	if ((!fdc.R) || (fdc.R > fdd->inf.xdf.sectors)) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	// ver0.29
	if ((fdc.mf != 0xff) && (fdc.mf != 0x40)) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	return(SUCCESS);
}

BOOL fddxdf_read(FDDFILE fdd) {

	FILEH	hdl;
	long	seekp;
	UINT	secsize;

	fddlasterror = 0x00;										// ver0.28
	if (fddxdf_seeksector(fdd)) {
		return(FAILURE);
	}
	if (fdc.N != fdd->inf.xdf.n) {
		fddlasterror = 0xc0;									// ver0.28
		return(FAILURE);
	}

	seekp = (fdc.treg[fdc.us] << 1) + fdc.hd;
	seekp *= fdd->inf.xdf.sectors;
	seekp += fdc.R - 1;
	seekp <<= (7 + fdd->inf.xdf.n);
	secsize = 128 << fdd->inf.xdf.n;

	hdl = file_open_rb(fdd->fname);
	if (hdl == FILEH_INVALID) {
		fddlasterror = 0xe0;									// ver0.28
		return(FAILURE);
	}
	if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
		(file_read(hdl, fdc.buf, secsize) != secsize)) {
		file_close(hdl);
		fddlasterror = 0xe0;									// ver0.28
		return(FAILURE);
	}
	file_close(hdl);
	fdc.bufcnt = secsize;
	fddlasterror = 0x00;										// ver0.28
	return(SUCCESS);
}

BOOL fddxdf_write(FDDFILE fdd) {

	FILEH	hdl;
	long	seekp;
	UINT	secsize;

	fddlasterror = 0x00;										// ver0.28
	if (fddxdf_seeksector(fdd)) {
		fddlasterror = 0xe0;									// ver0.28
		return(FAILURE);
	}
	if (fdd->protect) {
		fddlasterror = 0x70;									// ver0.28
		return(FAILURE);
	}
	if (fdc.N != fdd->inf.xdf.n) {
		fddlasterror = 0xc0;									// ver0.28
		return(FAILURE);
	}

	seekp = (fdc.treg[fdc.us] << 1) + fdc.hd;
	seekp *= fdd->inf.xdf.sectors;
	seekp += fdc.R - 1;
	seekp <<= (7 + fdd->inf.xdf.n);
	secsize = 128 << fdd->inf.xdf.n;

	hdl = file_open(fdd->fname);
	if (hdl == FILEH_INVALID) {
		fddlasterror = 0xc0;									// ver0.28
		return(FAILURE);
	}
	if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
		(file_write(hdl, fdc.buf, secsize) != secsize)) {
		file_close(hdl);
		fddlasterror = 0xc0;									// ver0.28
		return(FAILURE);
	}
	file_close(hdl);
	fdc.bufcnt = secsize;
	fddlasterror = 0x00;										// ver0.28
	return(SUCCESS);
}

BOOL fddxdf_readid(FDDFILE fdd) {

	fddlasterror = 0x00;
	if ((fdc.rpm != fdd->inf.xdf.rpm) ||
		(fdc.crcn >= fdd->inf.xdf.sectors)) {
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	fdc.C = fdc.treg[fdc.us];
	fdc.H = fdc.hd;
	fdc.R = ++fdc.crcn;
	fdc.N = fdd->inf.xdf.n;
	return(SUCCESS);
}

