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
	char	diskdrv_fname[4][MAX_PATH];


void diskdrv_sethdd(REG8 drv, const char *fname) {

	UINT	num;
	char	*p;
	int		leng;

	num = drv & 0x0f;
	if (!(drv & 0x20)) {			// SASI or IDE
		if (num >= 2) {
			return;
		}
		p = np2cfg.sasihdd[num];
		leng = sizeof(np2cfg.sasihdd[0]);
	}
	else {							// SCSI
		if (num >= 4) {
			return;
		}
		p = np2cfg.scsihdd[num];
		leng = sizeof(np2cfg.scsihdd[0]);
	}
	if (fname) {
		file_cpyname(p, fname, leng);
	}
	else {
		p[0] = '\0';
	}
	sysmng_update(SYS_UPDATEHDD | SYS_UPDATECFG);
}

void diskdrv_setfdd(BYTE drv, const char *fname, int readonly) {

	if (drv < 4) {
		fdd_eject(drv);
		diskdrv_delay[drv] = 0;
		diskdrv_fname[drv][0] = '\0';
		fdc.stat[drv] = FDCRLT_IC0 | FDCRLT_IC1 | FDCRLT_NR | drv;
		fdc_interrupt();

		if (fname) {
			diskdrv_delay[drv] = DISK_DELAY;
			diskdrv_ro[drv] = readonly;
			file_cpyname(diskdrv_fname[drv], fname, sizeof(diskdrv_fname[0]));
		}
		sysmng_update(SYS_UPDATEFDD);
	}
}

void diskdrv_callback(void) {

	BYTE	drv;

	for (drv=0; drv<4; drv++) {
		if (diskdrv_delay[drv]) {
			diskdrv_delay[drv]--;
			if ((!diskdrv_delay[drv]) && (diskdrv_fname[drv][0])) {
				fdd_set(drv, diskdrv_fname[drv], FTYPE_NONE, diskdrv_ro[drv]);
				diskdrv_fname[drv][0] = '\0';
				fdc.stat[drv] = FDCRLT_IC0 | FDCRLT_IC1 | drv;
				fdc_interrupt();
				sysmng_update(SYS_UPDATEFDD);
			}
		}
	}
}

