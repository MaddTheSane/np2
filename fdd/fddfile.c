#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fddfile.h"
#include	"fdd_xdf.h"
#include	"fdd_d88.h"


	_FDDFILE	fddfile[MAX_FDDFILE];
	BYTE		fddlasterror;


static const char str_88d[] = "88d";
static const char str_98d[] = "98d";


// ----

void fddfile_init(void) {

	ZeroMemory(fddfile, sizeof(fddfile));
}

void fddfile_reset2dmode(void) { 			// ver0.29
#if 0
	int		i;

	for (i=0; i<4; i++) {
		fddfile[i].mode2d = 0;
	}
#endif
}

const char *fdd_diskname(BYTE drv) {

	if (drv >= MAX_FDDFILE) {
		return(str_null);
	}
	return(fddfile[drv].fname);
}

BOOL fdd_diskready(BYTE drv) {

	if ((drv >= MAX_FDDFILE) || (!fddfile[drv].fname[0])) {
		return(FALSE);
	}
	return(TRUE);
}

BOOL fdd_diskprotect(BYTE drv) {

	if ((drv >= MAX_FDDFILE) || (!fddfile[drv].protect)) {
		return(FALSE);
	}
	return(TRUE);
}


// --------------------------------------------------------------------------

BOOL fdd_set(BYTE drv, const char *fname, UINT ftype, int ro) {

	FDDFILE		fdd;
const char		*p;

	if (drv >= MAX_FDDFILE) {
		return(FAILURE);
	}
	if (ftype == FTYPE_NONE) {
		p = file_getext((char *)fname);
		if ((!milstr_cmp(p, str_d88)) || (!milstr_cmp(p, str_88d)) ||
			(!milstr_cmp(p, str_d98)) || (!milstr_cmp(p, str_98d))) {
			ftype = FTYPE_D88;
		}
		else {
			ftype = FTYPE_BETA;
		}
	}
	fdd = fddfile + drv;
	switch(ftype) {
		case FTYPE_BETA:
			return(fddxdf_set(fdd, fname, ro));

		case FTYPE_D88:
			return(fddd88_set(fdd, fname, ro));
	}
	return(FAILURE);
}

BOOL fdd_eject(BYTE drv) {

	FDDFILE		fdd;

	if (drv >= MAX_FDDFILE) {
		return(FAILURE);
	}
	fdd = fddfile + drv;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_eject(fdd));

		case DISKTYPE_D88:
			return(fddd88_eject(fdd));
	}
	return(FAILURE);
}


// ----

BOOL fdd_diskaccess(void) {

	FDDFILE		fdd;

	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_diskaccess(fdd));

		case DISKTYPE_D88:
			return(fdd_diskaccess_d88());
	}
	return(FAILURE);
}

BOOL fdd_seek(void) {

	FDDFILE		fdd;
	BOOL		ret = FAILURE;

	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			ret = fddxdf_seek(fdd);
			break;

		case DISKTYPE_D88:
			ret = fdd_seek_d88();
			break;
	}
	fdc.treg[fdc.us] = fdc.ncn;
	return(ret);
}

BOOL fdd_seeksector(void) {

	FDDFILE		fdd;

	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_seeksector(fdd));

		case DISKTYPE_D88:
			return(fdd_seeksector_d88());
	}
	return(FAILURE);
}


BOOL fdd_read(void) {

	FDDFILE		fdd;

	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_read(fdd));

		case DISKTYPE_D88:
			return(fdd_read_d88());
	}
	return(FAILURE);
}

BOOL fdd_write(void) {

	FDDFILE		fdd;

	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_write(fdd));

		case DISKTYPE_D88:
			return(fdd_write_d88());
	}
	return(FAILURE);
}

BOOL fdd_readid(void) {

	FDDFILE		fdd;

	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_readid(fdd));

		case DISKTYPE_D88:
			return(fdd_readid_d88());
	}
	return(FAILURE);
}

BOOL fdd_formatinit(void) {

	if (fddfile[fdc.us].type == DISKTYPE_D88) {
		return(fdd_formatinit_d88());
	}
	return(FAILURE);
}

BOOL fdd_formating(const BYTE *ID) {

	if (fddfile[fdc.us].type == DISKTYPE_D88) {
		return(fdd_formating_d88(ID));
	}
	return(FAILURE);
}

BOOL fdd_isformating(void) {

	if (fddfile[fdc.us].type == DISKTYPE_D88) {
		return(fdd_isformating_d88());
	}
	return(FALSE);
}

