#include "compiler.h"

#include "np2.h"
#include "commng.h"


// ---- non connect

static UINT
ncread(COMMNG self, BYTE *data)
{

	UNUSED(self);
	UNUSED(data);

	return 0;
}

static UINT
ncwrite(COMMNG self, BYTE data)
{

	UNUSED(self);
	UNUSED(data);

	return 0;
}

static BYTE
ncgetstat(COMMNG self)
{

	UNUSED(self);

	return 0xf0;
}

static long
ncmsg(COMMNG self, UINT msg, long param)
{

	UNUSED(self);
	UNUSED(msg);
	UNUSED(param);

	return 0;
}

static void
ncrelease(COMMNG self)
{

	UNUSED(self);
}

static const _COMMNG com_nc = {
	COMCONNECT_OFF, ncread, ncwrite, ncgetstat, ncmsg, ncrelease
};


// ----

void
commng_initialize(void)
{

	cmmidi_initailize();
}

COMMNG
commng_create(UINT device)
{
	COMMNG ret;
	COMCFG *cfg;

	ret = NULL;

	switch (device) {
	case COMCREATE_MPU98II:
		cfg = &np2oscfg.mpu;
		break;

	default:
		cfg = NULL;
		break;
	}
	if (cfg) {
		if (cfg->port == COMPORT_MIDI) {
			ret = cmmidi_create(cfg->mout, cfg->min, cfg->mdl);
			if (ret) {
				(*ret->msg)(ret, COMMSG_MIMPIDEFFILE, (long)cfg->def);
				(*ret->msg)(ret, COMMSG_MIMPIDEFEN, (long)cfg->def_en);
			}
		}
	}
	if (ret)
		return ret;
	return (COMMNG)&com_nc;
}

void
commng_destroy(COMMNG hdl)
{

	if (hdl) {
		hdl->release(hdl);
	}
}
