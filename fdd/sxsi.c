#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"sxsi.h"


const char sig_vhd[8] = "VHD1.00";
const char sig_nhd[15] = "T98HDDIMAGE.R0";

const SASIHDD sasihdd[7] = {
				{33, 4, 153},			// 5MB
				{33, 4, 310},			// 10MB
				{33, 6, 310},			// 15MB
				{33, 8, 310},			// 20MB
				{33, 4, 615},			// 20MB (not used!)
				{33, 6, 615},			// 30MB
				{33, 8, 615}};			// 40MB

#if 0
static const _SXSIDEV defide = {615*33*8, 615, 256, 33, 8,
								SXSITYPE_IDE | SXSITYPE_HDD, 256, 0, {0x00}};
static const _SXSIDEV defscsi = {40*16*32*8, 40*16, 256, 32, 8,
								SXSITYPE_SCSI | SXSITYPE_HDD, 220, 0, {0x00}};
#endif


	_SXSIDEV	sxsi_dev[SASIHDD_MAX + SCSIHDD_MAX];


// SASI規格HDDかチェック
static void sasihddcheck(SXSIDEV sxsi) {

const SASIHDD	*sasi;
	UINT		i;

	sasi = sasihdd;
	for (i=0; i<NELEMENTS(sasihdd); i++, sasi++) {
		if ((sxsi->size == 256) &&
			(sxsi->sectors == sasi->sectors) &&
			(sxsi->surfaces == sasi->surfaces) &&
			(sxsi->cylinders == sasi->cylinders)) {
			sxsi->type = (UINT16)(SXSITYPE_SASI + (i << 8) + SXSITYPE_HDD);
			break;
		}
	}
}


// ----

void sxsi_initialize(void) {

	UINT	i;

	ZeroMemory(sxsi_dev, sizeof(sxsi_dev));
	for (i=0; i<NELEMENTS(sxsi_dev); i++) {
		sxsi_dev[i].fh = (long)FILEH_INVALID;
	}
}

SXSIDEV sxsi_getptr(REG8 drv) {

	UINT	num;

	num = drv & 0x0f;
	if (!(drv & 0x20)) {					// SASI or IDE
		if (num < SASIHDD_MAX) {
			return(sxsi_dev + num);
		}
	}
#if defined(SUPPORT_SCSI)
	else {
		if (num < SCSIHDD_MAX) {			// SCSI
			return(sxsi_dev + SASIHDD_MAX + num);
		}
	}
#endif
	return(NULL);
}

const OEMCHAR *sxsi_getname(REG8 drv) {

	SXSIDEV	sxsi;

	sxsi = sxsi_getptr(drv);
	if (sxsi) {
		return(sxsi->fname);
	}
	return(NULL);
}

BOOL sxsi_hddopen(REG8 drv, const OEMCHAR *file) {

	SXSIDEV		sxsi;
	FILEH		fh;
const OEMCHAR	*ext;
	UINT16		type;
	long		totals;
	UINT32		headersize;
	UINT32		surfaces;
	UINT32		cylinders;
	UINT32		sectors;
	UINT32		size;

	if ((file == NULL) || (file[0] == '\0')) {
		goto sxsiope_err1;
	}
	sxsi = sxsi_getptr(drv);
	if (sxsi == NULL) {
		goto sxsiope_err1;
	}
	fh = file_open(file);
	if (fh == FILEH_INVALID) {
		goto sxsiope_err1;
	}
	ext = file_getext(file);
	type = SXSITYPE_HDD;
	if ((!file_cmpname(ext, str_thd)) && (!(drv & 0x20))) {
		THDHDR thd;						// T98 HDD (IDE)
		if (file_read(fh, &thd, sizeof(thd)) != sizeof(thd)) {
			goto sxsiope_err2;
		}
		headersize = 256;
		surfaces = 8;
		cylinders = LOADINTELWORD(thd.cylinders);
		sectors = 33;
		size = 256;
		totals = cylinders * sectors * surfaces;
	}
	else if ((!file_cmpname(ext, str_nhd)) && (!(drv & 0x20))) {
		NHDHDR nhd;						// T98Next HDD (IDE)
		if ((file_read(fh, &nhd, sizeof(nhd)) != sizeof(nhd)) ||
			(memcmp(nhd.sig, sig_nhd, 15))) {
			goto sxsiope_err2;
		}
		headersize = LOADINTELDWORD(nhd.headersize);
		surfaces = LOADINTELWORD(nhd.surfaces);
		cylinders = LOADINTELDWORD(nhd.cylinders);
		sectors = LOADINTELWORD(nhd.sectors);
		size = LOADINTELWORD(nhd.sectorsize);
		totals = cylinders * sectors * surfaces;
	}
	else if ((!file_cmpname(ext, str_hdi)) && (!(drv & 0x20))) {
		HDIHDR hdi;						// ANEX86 HDD (SASI) thanx Mamiya
		if (file_read(fh, &hdi, sizeof(hdi)) != sizeof(hdi)) {
			goto sxsiope_err2;
		}
		headersize = LOADINTELDWORD(hdi.headersize);
		surfaces = LOADINTELDWORD(hdi.surfaces);
		cylinders = LOADINTELDWORD(hdi.cylinders);
		sectors = LOADINTELDWORD(hdi.sectors);
		size = LOADINTELDWORD(hdi.sectorsize);
		totals = cylinders * sectors * surfaces;
	}
	else if ((!file_cmpname(ext, str_hdd)) && (drv & 0x20)) {
		VHDHDR vhd;						// Virtual98 HDD (SCSI)
		if ((file_read(fh, &vhd, sizeof(vhd)) != sizeof(vhd)) ||
			(memcmp(vhd.sig, sig_vhd, 5))) {
			goto sxsiope_err2;
		}
		headersize = sizeof(vhd);
		surfaces = vhd.surfaces;
		cylinders = LOADINTELWORD(vhd.cylinders);
		sectors = vhd.sectors;
		size = LOADINTELWORD(vhd.sectorsize);
		totals = (SINT32)LOADINTELDWORD(vhd.totals);
	}
	else {
		goto sxsiope_err2;
	}

	// フォーマット確認～
	if ((surfaces == 0) || (surfaces >= 256) ||
		(cylinders == 0) || (cylinders >= 65536) ||
		(sectors == 0) || (sectors >= 256) ||
		(size == 0) || ((size & (size - 1)) != 0)) {
		goto sxsiope_err2;
	}
	if (!(drv & 0x20)) {
		type |= SXSITYPE_IDE;
	}
	else {
		type |= SXSITYPE_SCSI;
		if (!(size & 0x700)) {			// not 256,512,1024
			goto sxsiope_err2;
		}
	}
	sxsi->totals = totals;
	sxsi->cylinders = (UINT16)cylinders;
	sxsi->size = (UINT16)size;
	sxsi->sectors = (UINT8)sectors;
	sxsi->surfaces = (UINT8)surfaces;
	sxsi->type = type;
	sxsi->headersize = headersize;
	sxsi->fh = (long)fh;
	file_cpyname(sxsi->fname, file, NELEMENTS(sxsi->fname));
	if (type == (SXSITYPE_IDE | SXSITYPE_HDD)) {
		sasihddcheck(sxsi);
	}
	return(SUCCESS);

sxsiope_err2:
	file_close(fh);

sxsiope_err1:
	return(FAILURE);
}

void sxsi_open(void) {

	int		i;
	REG8	drv;

	sxsi_trash();
	drv = 0;
	for (i=0; i<2; i++) {
		if (sxsi_hddopen(drv, np2cfg.sasihdd[i]) == SUCCESS) {
			drv++;
		}
	}
#if defined(SUPPORT_SCSI)
	drv = 0x20;
	for (i=0; i<4; i++) {
		if (sxsi_hddopen(drv, np2cfg.scsihdd[i]) == SUCCESS) {
			drv++;
		}
	}
#endif
}

void sxsi_flash(void) {

	SXSIDEV	sxsi;
	SXSIDEV	sxsiterm;

	sxsi = sxsi_dev;
	sxsiterm = sxsi + NELEMENTS(sxsi_dev);
	while(sxsi < sxsiterm) {
		if ((FILEH)sxsi->fh != FILEH_INVALID) {
			file_close((FILEH)sxsi->fh);
			sxsi->fh = (long)FILEH_INVALID;
		}
		sxsi++;
	}
}

void sxsi_trash(void) {

	SXSIDEV	sxsi;
	SXSIDEV	sxsiterm;

	sxsi = sxsi_dev;
	sxsiterm = sxsi + NELEMENTS(sxsi_dev);
	while(sxsi < sxsiterm) {
		if ((FILEH)sxsi->fh != FILEH_INVALID) {
			file_close((FILEH)sxsi->fh);
		}
		ZeroMemory(sxsi, sizeof(_SXSIDEV));
		sxsi->fh = (long)FILEH_INVALID;
		sxsi++;
	}
}

static SXSIDEV getdrive(REG8 drv) {

	SXSIDEV	ret;

	ret = sxsi_getptr(drv);
	if ((ret == NULL) || (ret->fname[0] == '\0')) {
		return(NULL);
	}
	if ((FILEH)ret->fh == FILEH_INVALID) {
		ret->fh = (long)file_open(ret->fname);
		if ((FILEH)ret->fh == FILEH_INVALID) {
			ret->fname[0] = '\0';
			return(NULL);
		}
	}
	sysmng_hddaccess(drv);
	return(ret);
}

BOOL sxsi_issasi(void) {

	REG8	drv;
	SXSIDEV	sxsi;
	BOOL	ret;
	UINT	sxsiif;

	ret = FALSE;
	for (drv=0x00; drv<0x04; drv++) {
		sxsi = sxsi_getptr(drv);
		if (sxsi) {
			sxsiif = sxsi->type & SXSITYPE_IFMASK;
			if (sxsiif == SXSITYPE_SASI) {
				ret = TRUE;
			}
			else if (sxsiif == SXSITYPE_IDE) {
				ret = FALSE;
				break;
			}
		}
	}
	return(ret);
}

BOOL sxsi_isscsi(void) {

	REG8	drv;
	SXSIDEV	sxsi;

	for (drv=0x20; drv<0x28; drv++) {
		sxsi = sxsi_getptr(drv);
		if ((sxsi) && (sxsi->type)) {
			return(TRUE);
		}
	}
	return(FALSE);
}

BOOL sxsi_iside(void) {

	REG8	drv;
	SXSIDEV	sxsi;

	for (drv=0x00; drv<0x04; drv++) {
		sxsi = sxsi_getptr(drv);
		if ((sxsi) && (sxsi->type)) {
			return(TRUE);
		}
	}
	return(FALSE);
}

REG8 sxsi_read(REG8 drv, long pos, UINT8 *buf, UINT size) {

const _SXSIDEV	*sxsi;
	long		r;
	UINT		rsize;

	sxsi = getdrive(drv);
	if (sxsi == NULL) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}
	pos = pos * sxsi->size + sxsi->headersize;
	r = file_seek((FILEH)sxsi->fh, pos, FSEEK_SET);
	if (pos != r) {
		return(0xd0);
	}
	while(size) {
		rsize = min(size, sxsi->size);
		CPU_REMCLOCK -= rsize;
		if (file_read((FILEH)sxsi->fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
	}
	return(0x00);
}

REG8 sxsi_write(REG8 drv, long pos, const UINT8 *buf, UINT size) {

const _SXSIDEV	*sxsi;
	long		r;
	UINT		wsize;

	sxsi = getdrive(drv);
	if (sxsi == NULL) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}
	pos = pos * sxsi->size + sxsi->headersize;
	r = file_seek((FILEH)sxsi->fh, pos, FSEEK_SET);
	if (pos != r) {
		return(0xd0);
	}
	while(size) {
		wsize = min(size, sxsi->size);
		CPU_REMCLOCK -= wsize;
		if (file_write((FILEH)sxsi->fh, buf, wsize) != wsize) {
			return(0x70);
		}
		buf += wsize;
		size -= wsize;
	}
	return(0x00);
}

REG8 sxsi_format(REG8 drv, long pos) {

const _SXSIDEV	*sxsi;
	long		r;
	UINT16		i;
	UINT8		work[256];
	UINT		size;
	UINT		wsize;

	sxsi = getdrive(drv);
	if (sxsi == NULL) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}
	pos = pos * sxsi->size + sxsi->headersize;
	r = file_seek((FILEH)sxsi->fh, pos, FSEEK_SET);
	if (pos != r) {
		return(0xd0);
	}
	FillMemory(work, sizeof(work), 0xe5);
	for (i=0; i<sxsi->sectors; i++) {
		size = sxsi->size;
		while(size) {
			wsize = min(size, sizeof(work));
			size -= wsize;
			CPU_REMCLOCK -= wsize;
			if (file_write((FILEH)sxsi->fh, work, wsize) != wsize) {
				return(0x70);
			}
		}
	}
	return(0x00);
}

