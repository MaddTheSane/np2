#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"sxsi.h"


typedef struct {
	char	vhd[3];
	char	ver[4];
	char	delimita;
	char	comment[128];
	BYTE	padding[4];
	BYTE	mbsize[2];
	BYTE	sectorsize[2];
	BYTE	sectors;
	BYTE	surfaces;
	BYTE	tracks[2];
	BYTE	totals[4];
} V98SCSIHDR;

typedef struct {
	BYTE	tracks[2];
} THDHDR;

typedef struct {
	BYTE	dummy[8];
	BYTE	headersize[4];
	BYTE	filesize[4];
	BYTE	sectorsize[4];
	BYTE	sectors[4];
	BYTE	surfaces[4];
	BYTE	tracks[4];
} HDIHDR;

static const _SXSIHDD defsasi = {615*33*8, 615, 256, 33, 8,
											HDDTYPE_SASI, 256, {0x00}};
static const _SXSIHDD defscsi = {40*16*32*8, 40*16, 256, 32, 8,
											HDDTYPE_SCSI, 220, {0x00}};


	_SXSIHDD	sxsi_hd[4];


// ----

void sxsi_initialize(void) {

	UINT	i;

	ZeroMemory(sxsi_hd, sizeof(sxsi_hd));
	for (i=0; i<(sizeof(sxsi_hd)/sizeof(_SXSIHDD)); i++) {
		sxsi_hd[i].fh = (void *)FILEH_INVALID;
	}
}

SXSIHDD sxsi_getptr(BYTE drv) {

	UINT	num;

	num = drv & 0x0f;
	if (num >= 2) {
		return(NULL);
	}
	num += (drv & 0x20) >> 4;
	return(sxsi_hd + num);
}

const char *sxsi_getname(BYTE drv) {

	SXSIHDD	sxsi;

	sxsi = sxsi_getptr(drv);
	if (sxsi) {
		return(sxsi->fname);
	}
	return(NULL);
}

BOOL sxsi_hddopen(BYTE drv, const char *file) {

	SXSIHDD		sxsi;
const char		*ext;
	FILEH		fh;
	THDHDR		thd;
	HDIHDR		hdi;
	V98SCSIHDR	v98;

	if ((file == NULL) || (file[0] == '\0')) {
		goto sxsiope_err;
	}
	sxsi = sxsi_getptr(drv);
	if (sxsi == NULL) {
		goto sxsiope_err;
	}
	ext = file_getext((char *)file);
	if ((!file_cmpname(ext, "thd")) && (!(drv & 0x20))) {
		fh = file_open(file);								// T98 HDD (SASI)
		if (fh == FILEH_INVALID) {
			goto sxsiope_err;
		}
		if (file_read(fh, &thd, sizeof(thd)) == sizeof(thd)) {
			*sxsi = defsasi;
			sxsi->tracks = LOADINTELWORD(thd.tracks);
			sxsi->totals = sxsi->tracks * sxsi->sectors * sxsi->surfaces;
			file_cpyname(sxsi->fname, file, sizeof(sxsi->fname));
			sxsi->fh = (void *)fh;
			return(SUCCESS);
		}
		file_close(fh);
	}
	else if ((!file_cmpname(ext, "hdi")) && (!(drv & 0x20))) {
		fh = file_open(file);				// ANEX86 HDD (SASI) thanx Mamiya
		if (fh == FILEH_INVALID) {
			goto sxsiope_err;
		}
		if (file_read(fh, &hdi, sizeof(hdi)) == sizeof(hdi)) {
			*sxsi = defsasi;
			sxsi->size = LOADINTELWORD(hdi.sectorsize);
			sxsi->headersize = LOADINTELDWORD(hdi.headersize);
			sxsi->tracks = LOADINTELWORD(hdi.tracks);
			sxsi->surfaces = hdi.surfaces[0];
			sxsi->sectors = hdi.sectors[0];
			sxsi->totals = sxsi->tracks * sxsi->sectors * sxsi->surfaces;
			file_cpyname(sxsi->fname, file, sizeof(sxsi->fname));
			sxsi->fh = (void *)fh;
			return(SUCCESS);
		}
		file_close(fh);
	}
	else if ((!file_cmpname(ext, "hdd")) && (drv & 0x20)) {
		fh = file_open(file);						// Virtual98 HDD (SCSI)
		if (fh == FILEH_INVALID) {
			goto sxsiope_err;
		}
		if ((file_read(fh, &v98, sizeof(v98)) == sizeof(v98)) &&
			(!memcmp(v98.vhd, "VHD", 3))) {
			sxsi = &sxsi_hd[drv+2];
			*sxsi = defscsi;
			sxsi->totals = (SINT32)LOADINTELDWORD(v98.totals);
			sxsi->tracks = LOADINTELWORD(v98.tracks);
			sxsi->size = LOADINTELWORD(v98.sectorsize);
			sxsi->sectors = v98.sectors;
			sxsi->surfaces = v98.surfaces;
			milstr_ncpy(sxsi->fname, file, sizeof(sxsi->fname));
			sxsi->fh = (void *)fh;
			return(SUCCESS);
		}
		file_close(fh);
	}

sxsiope_err:
	return(FAILURE);
}

void sxsi_open(void) {

	int		i;
	BYTE	sasi;

	sxsi_trash();
	sasi = 0;
	for (i=0; i<2; i++) {
		if (sxsi_hddopen(sasi, np2cfg.hddfile[i]) == SUCCESS) {
			sasi++;
		}
	}
}

void sxsi_flash(void) {

	SXSIHDD	sxsi;
	SXSIHDD	sxsiterm;

	sxsi = sxsi_hd;
	sxsiterm = sxsi + (sizeof(sxsi_hd)/sizeof(_SXSIHDD));
	while(sxsi < sxsiterm) {
		if ((FILEH)sxsi->fh != FILEH_INVALID) {
			file_close((FILEH)sxsi->fh);
			sxsi->fh = (void *)FILEH_INVALID;
		}
		sxsi++;
	}
}

void sxsi_trash(void) {

	SXSIHDD	sxsi;
	SXSIHDD	sxsiterm;

	sxsi = sxsi_hd;
	sxsiterm = sxsi + (sizeof(sxsi_hd)/sizeof(_SXSIHDD));
	while(sxsi < sxsiterm) {
		if ((FILEH)sxsi->fh != FILEH_INVALID) {
			file_close((FILEH)sxsi->fh);
			sxsi->fh = (void *)FILEH_INVALID;
		}
		sxsi->fname[0] = '\0';
		sxsi++;
	}
}

static SXSIHDD getdrive(BYTE drv) {

	UINT	num;
	SXSIHDD	ret;

	num = drv & 0x0f;
	if (num >= 2) {
		return(NULL);
	}
	num += (drv & 0x20) >> 4;
	ret = sxsi_hd + num;
	if (ret->fname[0] == '\0') {
		return(NULL);
	}
	if ((FILEH)ret->fh == FILEH_INVALID) {
		ret->fh = (void *)file_open(ret->fname);
		if ((FILEH)ret->fh == FILEH_INVALID) {
			ret->fname[0] = '\0';
			return(NULL);
		}
	}
	return(ret);
}

BYTE sxsi_read(BYTE drv, long pos, BYTE *buf, UINT16 size) {

const _SXSIHDD	*sxsi;
	long		r;
	UINT16		rsize;

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
		nevent.remainclock -= rsize;
		if (file_read((FILEH)sxsi->fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
	}
	return(0x00);
}

BYTE sxsi_write(BYTE drv, long pos, const BYTE *buf, UINT16 size) {

const _SXSIHDD	*sxsi;
	long		r;
	UINT16		wsize;

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
		nevent.remainclock -= wsize;
		if (file_write((FILEH)sxsi->fh, buf, wsize) != wsize) {
			return(0x70);
		}
		buf += wsize;
		size -= wsize;
	}
	return(0x00);
}

BYTE sxsi_format(BYTE drv, long pos) {

const _SXSIHDD	*sxsi;
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
			nevent.remainclock -= wsize;
			if (file_write((FILEH)sxsi->fh, work, wsize) != wsize) {
				return(0x70);
			}
		}
	}
	return(0x00);
}

