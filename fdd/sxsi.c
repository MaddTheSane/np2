#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"sxsi.h"


static const char sig_vhd[] = "VHD";

static const _SXSIDEV defide = {615*33*8, 615, 256, 33, 8,
								SXSITYPE_IDE | SXSITYPE_HDD, 256, 0, {0x00}};
static const _SXSIDEV defscsi = {40*16*32*8, 40*16, 256, 32, 8,
								SXSITYPE_SCSI | SXSITYPE_HDD, 220, 0, {0x00}};

const SASIHDD sasihdd[7] = {
				{33, 4, 153},			// 5MB
				{33, 4, 310},			// 10MB
				{33, 6, 310},			// 15MB
				{33, 8, 310},			// 20MB
				{33, 4, 615},			// 20MB (not used!)
				{33, 6, 615},			// 30MB
				{33, 8, 615}};			// 40MB

	_SXSIDEV	sxsi_dev[SASIHDD_MAX + SCSIHDD_MAX];


// SASI‹KŠiHDD‚©ƒ`ƒFƒbƒN
static void sasihddcheck(SXSIDEV sxsi) {

const SASIHDD	*sasi;
	UINT		i;

	sasi = sasihdd;
	for (i=0; i<sizeof(sasihdd)/sizeof(SASIHDD); i++) {
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
	for (i=0; i<(sizeof(sxsi_dev)/sizeof(_SXSIDEV)); i++) {
		sxsi_dev[i].fh = (long)FILEH_INVALID;
	}
}

SXSIDEV sxsi_getptr(REG8 drv) {

	UINT	num;

	num = drv & 0x0f;
	if (!(drv & 0x20)) {			// SASI or IDE
		if (num < 2) {
			return(sxsi_dev + num);
		}
	}
	else {
		if (num < 4) {				// SCSI
			return(sxsi_dev + SASIHDD_MAX + num);
		}
	}
	return(NULL);
}

const char *sxsi_getname(REG8 drv) {

	SXSIDEV	sxsi;

	sxsi = sxsi_getptr(drv);
	if (sxsi) {
		return(sxsi->fname);
	}
	return(NULL);
}

BOOL sxsi_hddopen(REG8 drv, const char *file) {

	SXSIDEV		sxsi;
const char		*ext;
	FILEH		fh;
	THDHDR		thd;
	HDIHDR		hdi;
	VHDHDR		vhd;

	if ((file == NULL) || (file[0] == '\0')) {
		goto sxsiope_err;
	}
	sxsi = sxsi_getptr(drv);
	if (sxsi == NULL) {
		goto sxsiope_err;
	}
	ext = file_getext((char *)file);
	if ((!file_cmpname(ext, str_thd)) && (!(drv & 0x20))) {
		fh = file_open(file);								// T98 HDD (IDE)
		if (fh == FILEH_INVALID) {
			goto sxsiope_err;
		}
		if (file_read(fh, &thd, sizeof(thd)) == sizeof(thd)) {
			*sxsi = defide;
			sxsi->cylinders = LOADINTELWORD(thd.cylinders);
			sxsi->totals = sxsi->cylinders * sxsi->sectors * sxsi->surfaces;
			sasihddcheck(sxsi);
			file_cpyname(sxsi->fname, file, sizeof(sxsi->fname));
			sxsi->fh = (long)fh;
			return(SUCCESS);
		}
		file_close(fh);
	}
	else if ((!file_cmpname(ext, str_hdi)) && (!(drv & 0x20))) {
		fh = file_open(file);				// ANEX86 HDD (SASI) thanx Mamiya
		if (fh == FILEH_INVALID) {
			goto sxsiope_err;
		}
		if (file_read(fh, &hdi, sizeof(hdi)) == sizeof(hdi)) {
			*sxsi = defide;
			sxsi->size = LOADINTELWORD(hdi.sectorsize);
			sxsi->headersize = LOADINTELDWORD(hdi.headersize);
			sxsi->cylinders = LOADINTELWORD(hdi.cylinders);
			sxsi->surfaces = hdi.surfaces[0];
			sxsi->sectors = hdi.sectors[0];
			sxsi->totals = sxsi->cylinders * sxsi->sectors * sxsi->surfaces;
			sasihddcheck(sxsi);
			file_cpyname(sxsi->fname, file, sizeof(sxsi->fname));
			sxsi->fh = (long)fh;
			return(SUCCESS);
		}
		file_close(fh);
	}
	else if ((!file_cmpname(ext, str_hdd)) && (drv & 0x20)) {
		TRACEOUT(("insert hdd - %.2x", drv));
		fh = file_open(file);						// Virtual98 HDD (SCSI)
		if (fh == FILEH_INVALID) {
			goto sxsiope_err;
		}
		if ((file_read(fh, &vhd, sizeof(vhd)) == sizeof(vhd)) &&
			(!memcmp(vhd.sig, sig_vhd, 3))) {
			*sxsi = defscsi;
			sxsi->totals = (SINT32)LOADINTELDWORD(vhd.totals);
			sxsi->cylinders = LOADINTELWORD(vhd.cylinders);
			sxsi->size = LOADINTELWORD(vhd.sectorsize);
			sxsi->sectors = vhd.sectors;
			sxsi->surfaces = vhd.surfaces;
			file_cpyname(sxsi->fname, file, sizeof(sxsi->fname));
			sxsi->fh = (long)fh;
			TRACEOUT(("success"));
			return(SUCCESS);
		}
		file_close(fh);
	}

sxsiope_err:
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
	drv = 0x20;
	for (i=0; i<4; i++) {
		if (sxsi_hddopen(drv, np2cfg.scsihdd[i]) == SUCCESS) {
			drv++;
		}
	}
}

void sxsi_flash(void) {

	SXSIDEV	sxsi;
	SXSIDEV	sxsiterm;

	sxsi = sxsi_dev;
	sxsiterm = sxsi + (sizeof(sxsi_dev)/sizeof(_SXSIDEV));
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
	sxsiterm = sxsi + (sizeof(sxsi_dev)/sizeof(_SXSIDEV));
	while(sxsi < sxsiterm) {
		if ((FILEH)sxsi->fh != FILEH_INVALID) {
			file_close((FILEH)sxsi->fh);
			sxsi->fh = (long)FILEH_INVALID;
		}
		sxsi->fname[0] = '\0';
		sxsi++;
	}
}

static SXSIDEV getdrive(REG8 drv) {

	UINT	num;
	SXSIDEV	ret;

	num = drv & 0x0f;
	if (num >= 2) {
		return(NULL);
	}
	num += (drv & 0x20) >> 4;
	ret = sxsi_dev + num;
	if (ret->fname[0] == '\0') {
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

REG8 sxsi_read(REG8 drv, long pos, BYTE *buf, UINT size) {

const _SXSIDEV	*sxsi;
	long		r;
	UINT		rsize;

	sxsi = getdrive(drv);
	if (sxsi == NULL) {
		return(0x60);
	}
	pos = pos * sxsi->size + sxsi->headersize;
	r = file_seek((FILEH)sxsi->fh, pos, FSEEK_SET);
	if (r == -1) {
		return(0x40);
	}
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

REG8 sxsi_write(REG8 drv, long pos, const BYTE *buf, UINT size) {

const _SXSIDEV	*sxsi;
	long		r;
	UINT		wsize;

	sxsi = getdrive(drv);
	if (sxsi == NULL) {
		return(0x60);
	}
	pos = pos * sxsi->size + sxsi->headersize;
	r = file_seek((FILEH)sxsi->fh, pos, FSEEK_SET);
	if (r == -1) {
		return(0x40);
	}
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
	BYTE		work[256];
	UINT		size;
	UINT		wsize;

	sxsi = getdrive(drv);
	if (sxsi == NULL) {
		return(0x60);
	}
	pos = pos * sxsi->size + sxsi->headersize;
	r = file_seek((FILEH)sxsi->fh, pos, FSEEK_SET);
	if (r == -1) {
		return(0x40);
	}
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

