// �t���b�s�[�f�B�X�N�̑}�����ؗp�̃u���b�a

#include	"compiler.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"diskdrv.h"
#include	"fddfile.h"


#define	DISK_DELAY	20			// (0.4sec)

	int		diskdrv_delay[4];
	int		diskdrv_ro[4];
	OEMCHAR	diskdrv_fname[4][MAX_PATH];


void diskdrv_sethdd(REG8 drv, const OEMCHAR *fname) {

	UINT	num;
	OEMCHAR	*p;
	int		leng;

	num = drv & 0x0f;
	p = NULL;
	leng = 0;
	if (!(drv & 0x20)) {			// SASI or IDE
		if (num < 2) {
			p = np2cfg.sasihdd[num];
			leng = NELEMENTS(np2cfg.sasihdd[0]);
		}
	}
#if defined(SUPPORT_SCSI)
	else {							// SCSI
		if (num < 4) {
			p = np2cfg.scsihdd[num];
			leng = NELEMENTS(np2cfg.scsihdd[0]);
		}
	}
#endif
	if (p) {
		if (fname) {
			file_cpyname(p, fname, leng);
		}
		else {
			p[0] = '\0';
		}
		sysmng_update(SYS_UPDATEHDD | SYS_UPDATECFG);
	}
}

void diskdrv_readyfdd(REG8 drv, const OEMCHAR *fname, int readonly) {

	if ((drv < 4) && (fdc.equip & (1 << drv))) {
		if ((fname != NULL) && (fname[0] != '\0')) {
			fdd_set(drv, fname, FTYPE_NONE, readonly);
			fdc.stat[drv] = FDCRLT_AI | drv;
			fdc_interrupt();
			sysmng_update(SYS_UPDATEFDD);
		}
	}
}

void diskdrv_setfdd(REG8 drv, const OEMCHAR *fname, int readonly) {

	if ((drv < 4) && (fdc.equip & (1 << drv))) {
		fdd_eject(drv);
		diskdrv_delay[drv] = 0;
		diskdrv_fname[drv][0] = '\0';
		fdc.stat[drv] = FDCRLT_AI | FDCRLT_NR | drv;
		fdc_interrupt();

		if (fname) {
			diskdrv_delay[drv] = DISK_DELAY;
			diskdrv_ro[drv] = readonly;
			file_cpyname(diskdrv_fname[drv], fname,
												NELEMENTS(diskdrv_fname[0]));
		}
		sysmng_update(SYS_UPDATEFDD);
	}
}

void diskdrv_callback(void) {

	REG8	drv;

	for (drv=0; drv<4; drv++) {
		if (diskdrv_delay[drv]) {
			diskdrv_delay[drv]--;
			if (!diskdrv_delay[drv]) {
				diskdrv_readyfdd(drv, diskdrv_fname[drv], diskdrv_ro[drv]);
				diskdrv_fname[drv][0] = '\0';
			}
		}
	}
}

