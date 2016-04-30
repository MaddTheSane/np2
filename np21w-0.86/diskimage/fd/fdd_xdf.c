#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"

#include	"diskimage/fddfile.h"
#include	"diskimage/fd/fdd_xdf.h"

#ifdef SUPPORT_KAI_IMAGES

//	BKDSK(DD6)�ABKDSK(DDB)����p
//static const OEMCHAR str_dd6[] = OEMTEXT("dd6");	//	���g�p�Ȃ̂ŃR�����g(kai9)
static const OEMCHAR str_ddb[] = OEMTEXT("ddb");
//

static const _XDFINFO supportxdf[] = {
#if 0
			// 256
			{0, 154, 26, 1, DISKTYPE_2HD, 0},
			// 512
			{0, 154, 15, 2, DISKTYPE_2HD, 0},
#endif
#if 1
			// 512
			{0, 160, 15, 2, DISKTYPE_2HD, 0},	//	BKDSK(HD5)	MS-DOS 1.21M(2HC)
#endif
			// 1024
			{0, 154,  8, 3, DISKTYPE_2HD, 0},	//	XDF��2HD�ABIN�AFLP��
												//	BKDSK(HDM)	MS-DOS 1.25M(2HD)
			// 1.44MB
			{0, 160, 18, 2, DISKTYPE_2HD, 1},	//	BKDSK(HD4)	MS-DOS 1.44M(2HD)

			//	�ǉ��ł�������BKDSK�`���ɑΉ�
			{0, 154, 26, 1, DISKTYPE_2HD, 0},	//	BKDSK(H01)	2HD:256byte*26sec (0-154)track
			{0, 154,  9, 3, DISKTYPE_2HD, 0},	//	BKDSK(HD9)	MS-DOS 2HD(9sec)
			{0, 160,  8, 2, DISKTYPE_2DD, 0},	//	BKDSK(DD6)	MS-DOS  640K(2DD)
			{0, 160,  9, 2, DISKTYPE_2DD, 0},	//	BKDSK(DD9)	MS-DOS  720K(2DD)
			//	track 0�Ƃ���ȊO�ŃZ�N�^�T�C�Y���Ⴄ���ߖ��Ή�
//			{0, 154, 26, 1, DISKTYPE_2HD, 0},	//	BKDSK(HDB)	BASIC 2HD
			//
			{0, 160, 16, 1, DISKTYPE_2DD, 0},	//	BKDSK(DDB)	BASIC 2DD
			//
			//	�w�b�_�T�C�Y���w�肵�A���莞�ɍl������悤�ɂ����
			//	DIP��DCP�^DCU(�S�g���b�N�i�[�C���[�W)�Ƃ���������
			{256, 154,  8, 3, DISKTYPE_2HD, 0},	//	DIP			2HD-8�Z�N�^(1.25MB)
												//	FIM			2HD-8�Z�N�^(1.25MB)
			{256, 154, 26, 1, DISKTYPE_2HD, 0},	//	FIM			2HD-26�Z�N�^
#if 0	//	fdd_dcp.c�ֈړ�(kai9)
			{162, 154,  8, 3, DISKTYPE_2HD, 0},	//	DCP�^DCU	01h	2HD- 8�Z�N�^(1.25MB)
			{162, 160, 15, 2, DISKTYPE_2HD, 0},	//	DCP�^DCU	02h	2HD-15�Z�N�^(1.21MB)
			{162, 160, 18, 2, DISKTYPE_2HD, 1},	//	DCP�^DCU	03h	2HQ-18�Z�N�^(1.44MB)
			{162, 160,  8, 2, DISKTYPE_2DD, 0},	//	DCP�^DCU	04h	2DD- 8�Z�N�^( 640KB)
			{162, 160,  9, 2, DISKTYPE_2DD, 0},	//	DCP�^DCU	05h	2DD- 9�Z�N�^( 720KB)
			{162, 154,  9, 3, DISKTYPE_2HD, 0},	//	DCP�^DCU	08h	2HD- 9�Z�N�^(1.44MB)
//			{162, 154, 26, 1, DISKTYPE_2HD, 0},	//	DCP�^DCU	11h	BASIC-2HD
			{162, 160, 16, 1, DISKTYPE_2DD, 0},	//	DCP�^DCU	19h	BASIC-2DD
			{162, 154, 26, 1, DISKTYPE_2HD, 0},	//	DCP�^DCU	21h	2HD-26�Z�N�^
#endif
			//
};

//	FDI�w�b�_
typedef struct {
	UINT8	dummy[4];
	UINT8	fddtype[4];
	UINT8	headersize[4];
	UINT8	fddsize[4];
	UINT8	sectorsize[4];
	UINT8	sectors[4];
	UINT8	surfaces[4];
	UINT8	cylinders[4];
} FDIHDR;

//BRESULT fddxdf_set(FDDFILE fdd, const OEMCHAR *fname, int ro) {
BRESULT fdd_set_xdf(FDDFILE fdd, FDDFUNC fdd_fn, const OEMCHAR *fname, int ro) {

const _XDFINFO	*xdf;
	short		attr;
	FILEH		fh;
	UINT32		fdsize;
	UINT		size;
const OEMCHAR	*p;			//	BKDSK(DD6) or BKDSK(DDB)����p

	attr = file_attr(fname);
	if (attr & 0x18) {
		return(FAILURE);
	}
	fh = file_open(fname);
	if (fh == FILEH_INVALID) {
		return(FAILURE);
	}
	fdsize = file_getsize(fh);
	file_close(fh);

	p = file_getext(fname);	//	BKDSK(DD6) or BKDSK(DDB)����p

	xdf = supportxdf;
	while(xdf < (supportxdf + NELEMENTS(supportxdf))) {
		size = xdf->tracks;
		size *= xdf->sectors;
		size <<= (7 + xdf->n);
		//	�w�b�_�T�C�Y���l������悤��
		size += xdf->headersize;
		//
		if (size == fdsize) {
			//	BKDSK(DD6)��BKDSK(DDB)������T�C�Y�̂��߁A�g���q�Ŕ���
			if (!milstr_cmp(p, str_ddb) && xdf->sectors == 8) {
				xdf++;
				continue;
			}
			//
			fdd->type = DISKTYPE_BETA;
			fdd->protect = ((attr & 1) || (ro))?TRUE:FALSE;
			fdd->inf.xdf = *xdf;
			//	�����֐��Q��o�^(kai9)
			fdd_fn->eject		= fdd_eject_xxx;
//			fdd_fn->diskaccess	= fdd_diskaccess_xdf;
//			fdd_fn->seek		= fdd_seek_xdf;
//			fdd_fn->seeksector	= fdd_seeksector_xdf;
			fdd_fn->diskaccess	= fdd_diskaccess_common;
			fdd_fn->seek		= fdd_seek_common;
			fdd_fn->seeksector	= fdd_seeksector_common;
			fdd_fn->read		= fdd_read_xdf;
			fdd_fn->write		= fdd_write_xdf;
//			fdd_fn->readid		= fdd_readid_xdf;
			fdd_fn->readid		= fdd_readid_common;
			fdd_fn->writeid		= fdd_dummy_xxx;
			fdd_fn->formatinit	= fdd_dummy_xxx;
			fdd_fn->formating	= fdd_formating_xxx;
			fdd_fn->isformating	= fdd_isformating_xxx;
			//
			return(SUCCESS);
		}
		xdf++;
	}
	return(FAILURE);
}

// ��������Ή������肵��
//BRESULT fddxdf_setfdi(FDDFILE fdd, const OEMCHAR *fname, int ro) {
BRESULT fdd_set_fdi(FDDFILE fdd, FDDFUNC fdd_fn, const OEMCHAR *fname, int ro) {

	short	attr;
	FILEH	fh;
	UINT32	fdsize;
	UINT	r;
	FDIHDR	fdi;
	UINT32	fddtype;
	UINT32	headersize;
	UINT32	size;
	UINT32	sectors;
	UINT32	surfaces;
	UINT32	cylinders;
	UINT8	n;
	UINT8	disktype;
	UINT8	rpm;

	attr = file_attr(fname);
	if (attr & 0x18) {
		return(FAILURE);
	}
	fdsize = 0;
	r = 0;
	fh = file_open_rb(fname);
	if (fh != FILEH_INVALID) {
		fdsize = file_getsize(fh);
		r = file_read(fh, &fdi, sizeof(fdi));
		file_close(fh);
	}
	if (r != sizeof(fdi)) {
		return(FAILURE);
	}
	fddtype = LOADINTELDWORD(fdi.fddtype);
	headersize = LOADINTELDWORD(fdi.headersize);
	size = LOADINTELDWORD(fdi.sectorsize);
	sectors = LOADINTELDWORD(fdi.sectors);
	surfaces = LOADINTELDWORD(fdi.surfaces);
	cylinders = LOADINTELDWORD(fdi.cylinders);
	if (((size & (size - 1)) != 0) || (!(size & 0x7f80)) ||
		(sectors == 0) || (sectors >= 256) ||
		(surfaces != 2) ||
		(cylinders == 0) || (cylinders >= 128)) {
		return(FAILURE);
	}
	if (fdsize != (headersize + (size * sectors * surfaces * cylinders))) {
		return(FAILURE);
	}
	size >>= 8;
	n = 0;
	while(size) {
		size >>= 1;
		n++;
	}
	disktype = DISKTYPE_2HD;
	rpm = 0;
	switch(fddtype & 0xf0) {
		case 0x10:				// 1MB/640KB - 2DD
		case 0x70:				// 640KB - 2DD
		case 0xf0:
			disktype = DISKTYPE_2DD;
			break;

		case 0x30:				// 1.44MB - 2HD
		case 0xb0:
			rpm = 1;
			break;

		case 0x50:				// 320KB - 2D
		case 0xd0:				// 
			disktype = DISKTYPE_2D;
			break;

		case 0x90:				// 2HD
			break;

		default:
			return(FAILURE);
	}
	fdd->type = DISKTYPE_BETA;
	fdd->protect = ((attr & 1) || (ro))?TRUE:FALSE;
	fdd->inf.xdf.headersize = headersize;
	fdd->inf.xdf.tracks = (UINT8)(cylinders * 2);
	fdd->inf.xdf.sectors = (UINT8)sectors;
	fdd->inf.xdf.n = n;
	fdd->inf.xdf.disktype = disktype;
	fdd->inf.xdf.rpm = rpm;
	//	�����֐��Q��o�^(kai9)
	fdd_fn->eject		= fdd_eject_xxx;
	fdd_fn->diskaccess	= fdd_diskaccess_common;
	fdd_fn->seek		= fdd_seek_common;
	fdd_fn->seeksector	= fdd_seeksector_common;
	fdd_fn->read		= fdd_read_xdf;
	fdd_fn->write		= fdd_write_xdf;
	fdd_fn->readid		= fdd_readid_common;
	fdd_fn->writeid		= fdd_dummy_xxx;
	fdd_fn->formatinit	= fdd_dummy_xxx;
	fdd_fn->formating	= fdd_formating_xxx;
	fdd_fn->isformating	= fdd_isformating_xxx;
	//
	return(SUCCESS);
}

#if 0	//	���ʊ֐��������̂Ŕp�~(fddfile.c��)(kai9)
//BRESULT fddxdf_eject(FDDFILE fdd) {
BRESULT fdd_eject_xdf(FDDFILE fdd) {

//	fdd->fname[0] = '\0';
//	fdd->type = DISKTYPE_NOTREADY;
	return(SUCCESS);
}

//BRESULT fddxdf_diskaccess(FDDFILE fdd) {
BRESULT fdd_diskaccess_xdf(FDDFILE fdd) {

	if ((fdd->type != DISKTYPE_BETA) ||
		(CTRL_FDMEDIA != fdd->inf.xdf.disktype)) {
		return(FAILURE);
	}
	return(SUCCESS);
}

//BRESULT fddxdf_seek(FDDFILE fdd) {
BRESULT fdd_seek_xdf(FDDFILE fdd) {

	if ((fdd->type != DISKTYPE_BETA) ||
		(CTRL_FDMEDIA != fdd->inf.xdf.disktype) ||
		(fdc.rpm[fdc.us] != fdd->inf.xdf.rpm) ||
		(fdc.ncn >= (fdd->inf.xdf.tracks >> 1))) {
		return(FAILURE);
	}
	return(SUCCESS);
}

//BRESULT fddxdf_seeksector(FDDFILE fdd) {
BRESULT fdd_seeksector_xdf(FDDFILE fdd) {

	if ((fdd->type != DISKTYPE_BETA) ||
		(CTRL_FDMEDIA != fdd->inf.xdf.disktype) ||
		(fdc.rpm[fdc.us] != fdd->inf.xdf.rpm) ||
		(fdc.treg[fdc.us] >= (fdd->inf.xdf.tracks >> 1))) {
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	if ((!fdc.R) || (fdc.R > fdd->inf.xdf.sectors)) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	if ((fdc.mf != 0xff) && (fdc.mf != 0x40)) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	return(SUCCESS);
}
#endif

//BRESULT fddxdf_read(FDDFILE fdd) {
BRESULT fdd_read_xdf(FDDFILE fdd) {

	FILEH	hdl;
	long	seekp;
	UINT	secsize;

	fddlasterror = 0x00;
//	if (fddxdf_seeksector(fdd)) {
	if (fdd_seeksector_common(fdd)) {
		return(FAILURE);
	}
	if (fdc.N != fdd->inf.xdf.n) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}

	seekp = (fdc.treg[fdc.us] << 1) + fdc.hd;
	seekp *= fdd->inf.xdf.sectors;
	seekp += fdc.R - 1;
	seekp <<= (7 + fdd->inf.xdf.n);
	seekp += fdd->inf.xdf.headersize;
	secsize = 128 << fdd->inf.xdf.n;

	hdl = file_open_rb(fdd->fname);
	if (hdl == FILEH_INVALID) {
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
		(file_read(hdl, fdc.buf, secsize) != secsize)) {
		file_close(hdl);
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	file_close(hdl);
	fdc.bufcnt = secsize;
	fddlasterror = 0x00;
	return(SUCCESS);
}

//BRESULT fddxdf_write(FDDFILE fdd) {
BRESULT fdd_write_xdf(FDDFILE fdd) {

	FILEH	hdl;
	long	seekp;
	UINT	secsize;

	fddlasterror = 0x00;
//	if (fddxdf_seeksector(fdd)) {
	if (fdd_seeksector_common(fdd)) {
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	if (fdd->protect) {
		fddlasterror = 0x70;
		return(FAILURE);
	}
	if (fdc.N != fdd->inf.xdf.n) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}

	seekp = (fdc.treg[fdc.us] << 1) + fdc.hd;
	seekp *= fdd->inf.xdf.sectors;
	seekp += fdc.R - 1;
	seekp <<= (7 + fdd->inf.xdf.n);
	seekp += fdd->inf.xdf.headersize;
	secsize = 128 << fdd->inf.xdf.n;

	hdl = file_open(fdd->fname);
	if (hdl == FILEH_INVALID) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
		(file_write(hdl, fdc.buf, secsize) != secsize)) {
		file_close(hdl);
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	file_close(hdl);
	fdc.bufcnt = secsize;
	fddlasterror = 0x00;
	return(SUCCESS);
}

#if 0	//	���ʊ֐��������̂Ŕp�~(fddfile.c��)(kai9)
//BRESULT fddxdf_readid(FDDFILE fdd) {
BRESULT fdd_readid_xdf(FDDFILE fdd) {

	fddlasterror = 0x00;
	if ((!fdc.mf) ||
		(fdc.rpm[fdc.us] != fdd->inf.xdf.rpm) ||
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
#endif

#else


static const _XDFINFO supportxdf[] = {
#if 0
			// 256
			{0, 154, 26, 1, DISKTYPE_2HD, 0},
			// 512
			{0, 154, 15, 2, DISKTYPE_2HD, 0},
#endif
#if 1
			// 512
			{0, 160, 15, 2, DISKTYPE_2HD, 0},
#endif
			// 1024
			{0, 154,  8, 3, DISKTYPE_2HD, 0},
			// 1.44MB
			{0, 160, 18, 2, DISKTYPE_2HD, 1},
};

typedef struct {
	UINT8	dummy[4];
	UINT8	fddtype[4];
	UINT8	headersize[4];
	UINT8	fddsize[4];
	UINT8	sectorsize[4];
	UINT8	sectors[4];
	UINT8	surfaces[4];
	UINT8	cylinders[4];
} FDIHDR;


BRESULT fddxdf_set(FDDFILE fdd, const OEMCHAR *fname, int ro) {

const _XDFINFO	*xdf;
	short		attr;
	FILEH		fh;
	UINT32		fdsize;
	UINT		size;

	attr = file_attr(fname);
	if (attr & 0x18) {
		return(FAILURE);
	}
	fh = file_open(fname);
	if (fh == FILEH_INVALID) {
		return(FAILURE);
	}
	fdsize = file_getsize(fh);
	file_close(fh);

	xdf = supportxdf;
	while(xdf < (supportxdf + NELEMENTS(supportxdf))) {
		size = xdf->tracks;
		size *= xdf->sectors;
		size <<= (7 + xdf->n);
		if (size == fdsize) {
			fdd->type = DISKTYPE_BETA;
			fdd->protect = ((attr & 1) || (ro))?TRUE:FALSE;
			fdd->inf.xdf = *xdf;
			return(SUCCESS);
		}
		xdf++;
	}
	return(FAILURE);
}

// ��������Ή������肵��
BRESULT fddxdf_setfdi(FDDFILE fdd, const OEMCHAR *fname, int ro) {

	short	attr;
	FILEH	fh;
	UINT32	fdsize;
	UINT	r;
	FDIHDR	fdi;
	UINT32	fddtype;
	UINT32	headersize;
	UINT32	size;
	UINT32	sectors;
	UINT32	surfaces;
	UINT32	cylinders;
	UINT8	n;
	UINT8	disktype;
	UINT8	rpm;

	attr = file_attr(fname);
	if (attr & 0x18) {
		return(FAILURE);
	}
	fdsize = 0;
	r = 0;
	fh = file_open_rb(fname);
	if (fh != FILEH_INVALID) {
		fdsize = file_getsize(fh);
		r = file_read(fh, &fdi, sizeof(fdi));
		file_close(fh);
	}
	if (r != sizeof(fdi)) {
		return(FAILURE);
	}
	fddtype = LOADINTELDWORD(fdi.fddtype);
	headersize = LOADINTELDWORD(fdi.headersize);
	size = LOADINTELDWORD(fdi.sectorsize);
	sectors = LOADINTELDWORD(fdi.sectors);
	surfaces = LOADINTELDWORD(fdi.surfaces);
	cylinders = LOADINTELDWORD(fdi.cylinders);
	if (((size & (size - 1)) != 0) || (!(size & 0x7f80)) ||
		(sectors == 0) || (sectors >= 256) ||
		(surfaces != 2) ||
		(cylinders == 0) || (cylinders >= 128)) {
		return(FAILURE);
	}
	if (fdsize != (headersize + (size * sectors * surfaces * cylinders))) {
		return(FAILURE);
	}
	size >>= 8;
	n = 0;
	while(size) {
		size >>= 1;
		n++;
	}
	disktype = DISKTYPE_2HD;
	rpm = 0;
	switch(fddtype & 0xf0) {
		case 0x10:				// 1MB/640KB - 2DD
		case 0x70:				// 640KB - 2DD
		case 0xf0:
			disktype = DISKTYPE_2DD;
			break;

		case 0x30:				// 1.44MB - 2HD
		case 0xb0:
			rpm = 1;
			break;

		case 0x50:				// 320KB - 2D
		case 0xd0:				// 
			disktype = DISKTYPE_2D;
			break;

		case 0x90:				// 2HD
			break;

		default:
			return(FAILURE);
	}
	fdd->type = DISKTYPE_BETA;
	fdd->protect = ((attr & 1) || (ro))?TRUE:FALSE;
	fdd->inf.xdf.headersize = headersize;
	fdd->inf.xdf.tracks = (UINT8)(cylinders * 2);
	fdd->inf.xdf.sectors = (UINT8)sectors;
	fdd->inf.xdf.n = n;
	fdd->inf.xdf.disktype = disktype;
	fdd->inf.xdf.rpm = rpm;
	return(SUCCESS);
}

BRESULT fddxdf_eject(FDDFILE fdd) {

	fdd->fname[0] = '\0';
	fdd->type = DISKTYPE_NOTREADY;
	return(SUCCESS);
}


BRESULT fddxdf_diskaccess(FDDFILE fdd) {

	if ((fdd->type != DISKTYPE_BETA) ||
		(CTRL_FDMEDIA != fdd->inf.xdf.disktype)) {
		return(FAILURE);
	}
	return(SUCCESS);
}

BRESULT fddxdf_seek(FDDFILE fdd) {

	if ((fdd->type != DISKTYPE_BETA) ||
		(CTRL_FDMEDIA != fdd->inf.xdf.disktype) ||
		(fdc.rpm[fdc.us] != fdd->inf.xdf.rpm) ||
		(fdc.ncn >= (fdd->inf.xdf.tracks >> 1))) {
		return(FAILURE);
	}
	return(SUCCESS);
}

BRESULT fddxdf_seeksector(FDDFILE fdd) {

	if ((fdd->type != DISKTYPE_BETA) ||
		(CTRL_FDMEDIA != fdd->inf.xdf.disktype) ||
		(fdc.rpm[fdc.us] != fdd->inf.xdf.rpm) ||
		(fdc.treg[fdc.us] >= (fdd->inf.xdf.tracks >> 1))) {
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	if ((!fdc.R) || (fdc.R > fdd->inf.xdf.sectors)) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	if ((fdc.mf != 0xff) && (fdc.mf != 0x40)) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	return(SUCCESS);
}

BRESULT fddxdf_read(FDDFILE fdd) {

	FILEH	hdl;
	long	seekp;
	UINT	secsize;

	fddlasterror = 0x00;
	if (fddxdf_seeksector(fdd)) {
		return(FAILURE);
	}
	if (fdc.N != fdd->inf.xdf.n) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}

	seekp = (fdc.treg[fdc.us] << 1) + fdc.hd;
	seekp *= fdd->inf.xdf.sectors;
	seekp += fdc.R - 1;
	seekp <<= (7 + fdd->inf.xdf.n);
	seekp += fdd->inf.xdf.headersize;
	secsize = 128 << fdd->inf.xdf.n;

	hdl = file_open_rb(fdd->fname);
	if (hdl == FILEH_INVALID) {
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
		(file_read(hdl, fdc.buf, secsize) != secsize)) {
		file_close(hdl);
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	file_close(hdl);
	fdc.bufcnt = secsize;
	fddlasterror = 0x00;
	return(SUCCESS);
}

BRESULT fddxdf_write(FDDFILE fdd) {

	FILEH	hdl;
	long	seekp;
	UINT	secsize;

	fddlasterror = 0x00;
	if (fddxdf_seeksector(fdd)) {
		fddlasterror = 0xe0;
		return(FAILURE);
	}
	if (fdd->protect) {
		fddlasterror = 0x70;
		return(FAILURE);
	}
	if (fdc.N != fdd->inf.xdf.n) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}

	seekp = (fdc.treg[fdc.us] << 1) + fdc.hd;
	seekp *= fdd->inf.xdf.sectors;
	seekp += fdc.R - 1;
	seekp <<= (7 + fdd->inf.xdf.n);
	seekp += fdd->inf.xdf.headersize;
	secsize = 128 << fdd->inf.xdf.n;

	hdl = file_open(fdd->fname);
	if (hdl == FILEH_INVALID) {
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	if ((file_seek(hdl, seekp, FSEEK_SET) != seekp) ||
		(file_write(hdl, fdc.buf, secsize) != secsize)) {
		file_close(hdl);
		fddlasterror = 0xc0;
		return(FAILURE);
	}
	file_close(hdl);
	fdc.bufcnt = secsize;
	fddlasterror = 0x00;
	return(SUCCESS);
}

BRESULT fddxdf_readid(FDDFILE fdd) {

	fddlasterror = 0x00;
	if ((!fdc.mf) ||
		(fdc.rpm[fdc.us] != fdd->inf.xdf.rpm) ||
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

#endif