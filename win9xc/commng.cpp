#include	"compiler.h"
#include	"commng.h"
#include	"cmmidi.h"


// ---- non connect

UINT commng_ncread(COMMNG self, BYTE *data) {

	(void)self;
	(void)data;
	return(0);
}

UINT commng_ncwrite(COMMNG self, BYTE data) {

	(void)self;
	(void)data;
	return(0);
}

BYTE commng_ncgetstat(COMMNG self) {

	(void)self;
	return(0xf0);
}

UINT commng_ncmsg(COMMNG self, UINT msg, long param) {

	(void)self;
	(void)msg;
	(void)param;
	return(0);
}

static void ncrelease(COMMNG self) {
}

static const _COMMNG com_nc = {
		COMCONNECT_OFF,
		commng_ncread, commng_ncwrite, commng_ncgetstat, commng_ncmsg,
		ncrelease};


// ----

void commng_initialize(void) {
}

COMMNG commng_create(UINT device) {

	COMMNG	ret;

	ret = NULL;
	if (device == COMCREATE_MPU98II) {
		ret = cmmidi_create();
	}
	if (ret == NULL) {
		ret = (COMMNG)&com_nc;
	}
	return(ret);
}

void commng_destroy(COMMNG hdl) {

	if (hdl) {
		hdl->release(hdl);
	}
}

