#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"sxsi.h"
#if defined(SUPPORT_IDEIO)
#include	"ideio.h"
#endif

	_SXSIDEV	sxsi_dev[SASIHDD_MAX + SCSIHDD_MAX];


// ----

static REG8	nc_read(SXSIDEV sxsi, long pos, UINT8 *buf, UINT size) {

	(void)sxsi;
	(void)pos;
	(void)buf;
	(void)size;
	return(0x60);
}

static REG8 nc_write(SXSIDEV sxsi, long pos, const UINT8 *buf, UINT size) {

	(void)sxsi;
	(void)pos;
	(void)buf;
	(void)size;
	return(0x60);
}

static REG8 nc_format(SXSIDEV sxsi, long pos) {

	(void)sxsi;
	(void)pos;
	return(0x60);
}

static void sxsi_disconnect(SXSIDEV sxsi) {

	FILEH	fh;

	if (sxsi) {
#if defined(SUPPORT_IDEIO)
		ideio_notify(sxsi->drv, 0);
#endif
		fh = (FILEH)sxsi->fh;
		sxsi->flag = 0;
		sxsi->fh = (INTPTR)FILEH_INVALID;
		sxsi->read = nc_read;
		sxsi->write = nc_write;
		sxsi->format = nc_format;
		if (fh != FILEH_INVALID) {
			file_close(fh);
		}
	}
}


// ----

void sxsi_initialize(void) {

	UINT	i;

	ZeroMemory(sxsi_dev, sizeof(sxsi_dev));
	for (i=0; i<SASIHDD_MAX; i++) {
		sxsi_dev[i].drv = (UINT8)(SXSIDRV_SASI + i);
	}
	for (i=0; i<SCSIHDD_MAX; i++) {
		sxsi_dev[SASIHDD_MAX + i].drv = (UINT8)(SXSIDRV_SCSI + i);
	}
	for (i=0; i<NELEMENTS(sxsi_dev); i++) {
		sxsi_dev[i].fh = (INTPTR)FILEH_INVALID;
		sxsi_disconnect(sxsi_dev + i);
	}
}

void sxsi_allflash(void) {

	SXSIDEV	sxsi;
	SXSIDEV	sxsiterm;
	FILEH	fh;

	sxsi = sxsi_dev;
	sxsiterm = sxsi + NELEMENTS(sxsi_dev);
	while(sxsi < sxsiterm) {
		fh = (FILEH)sxsi->fh;
		sxsi->fh = (INTPTR)FILEH_INVALID;
		if (fh != FILEH_INVALID) {
			file_close(fh);
		}
		sxsi++;
	}
}

void sxsi_alltrash(void) {

	SXSIDEV	sxsi;
	SXSIDEV	sxsiterm;

	sxsi = sxsi_dev;
	sxsiterm = sxsi + NELEMENTS(sxsi_dev);
	while(sxsi < sxsiterm) {
		sxsi_disconnect(sxsi);
		sxsi++;
	}
}

BOOL sxsi_isconnect(SXSIDEV sxsi) {

	if (sxsi) {
		switch(sxsi->devtype) {
			case SXSIDEV_HDD:
				if (sxsi->flag & SXSIFLAG_READY) {
					return(TRUE);
				}
				break;

			case SXSIDEV_CDROM:
				return(TRUE);
		}
	}
	return(FALSE);
}

BRESULT sxsi_prepare(SXSIDEV sxsi) {

	FILEH	fh;

	if ((sxsi == NULL) || (!(sxsi->flag & SXSIFLAG_READY))) {
		return(FAILURE);
	}
	fh = (FILEH)sxsi->fh;
	if (fh == FILEH_INVALID) {
		fh = file_open(sxsi->filename);
		sxsi->fh = (INTPTR)fh;
		if (fh == FILEH_INVALID) {
			sxsi->flag = 0;
			return(FAILURE);
		}
	}
	sysmng_hddaccess(sxsi->drv);
	return(SUCCESS);
}


// ----

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

const OEMCHAR *sxsi_getfilename(REG8 drv) {

	SXSIDEV	sxsi;

	sxsi = sxsi_getptr(drv);
	if ((sxsi) && (sxsi->flag & SXSIFLAG_READY)) {
		return(sxsi->filename);
	}
	return(NULL);
}

BRESULT sxsi_setdevtype(REG8 drv, UINT8 dev) {

	SXSIDEV	sxsi;

	sxsi = sxsi_getptr(drv);
	if (sxsi) {
		if (sxsi->devtype != dev) {
			sxsi_disconnect(sxsi);
			sxsi->devtype = dev;
		}
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}

BRESULT sxsi_devopen(REG8 drv, const OEMCHAR *file) {

	SXSIDEV		sxsi;
	BRESULT		r;

	if ((file == NULL) || (file[0] == '\0')) {
		goto sxsiope_err;
	}
	sxsi = sxsi_getptr(drv);
	if (sxsi == NULL) {
		goto sxsiope_err;
	}
	switch(sxsi->devtype) {
		case SXSIDEV_HDD:
			r = sxsihdd_open(sxsi, file);
			break;

		case SXSIDEV_CDROM:
			r = sxsicd_open(sxsi, file);
			break;

		default:
			r = FAILURE;
			break;
	}
	if (r != SUCCESS) {
		goto sxsiope_err;
	}
	file_cpyname(sxsi->filename, file, NELEMENTS(sxsi->filename));
	sxsi->flag = SXSIFLAG_READY;
#if defined(SUPPORT_IDEIO)
	ideio_notify(sxsi->drv, 1);
#endif
	return(SUCCESS);

sxsiope_err:
	return(FAILURE);
}

void sxsi_devclose(REG8 drv) {

	SXSIDEV		sxsi;

	sxsi = sxsi_getptr(drv);
	sxsi_disconnect(sxsi);
}

BOOL sxsi_issasi(void) {

	REG8	drv;
	SXSIDEV	sxsi;
	BOOL	ret;

	ret = FALSE;
	for (drv=0x00; drv<0x04; drv++) {
		sxsi = sxsi_getptr(drv);
		if (sxsi) {
			if (sxsi->devtype == SXSIDEV_HDD) {
				if (sxsi->flag & SXSIFLAG_READY) {
					if (sxsi->mediatype & SXSIMEDIA_INVSASI) {
						return(FALSE);
					}
					ret = TRUE;
				}
			}
			else {
				return(FALSE);
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
		if (sxsi_isconnect(sxsi)) {
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
		if (sxsi_isconnect(sxsi)) {
			return(TRUE);
		}
	}
	return(FALSE);
}



REG8 sxsi_read(REG8 drv, long pos, UINT8 *buf, UINT size) {

	SXSIDEV	sxsi;

	sxsi = sxsi_getptr(drv);
	if (sxsi != NULL) {
		return(sxsi->read(sxsi, pos, buf, size));
	}
	else {
		return(0x60);
	}
}

REG8 sxsi_write(REG8 drv, long pos, const UINT8 *buf, UINT size) {

	SXSIDEV	sxsi;

	sxsi = sxsi_getptr(drv);
	if (sxsi != NULL) {
		return(sxsi->write(sxsi, pos, buf, size));
	}
	else {
		return(0x60);
	}
}

REG8 sxsi_format(REG8 drv, long pos) {

	SXSIDEV	sxsi;

	sxsi = sxsi_getptr(drv);
	if (sxsi != NULL) {
		return(sxsi->format(sxsi, pos));
	}
	else {
		return(0x60);
	}
}

