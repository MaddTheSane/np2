// フロッピーディスクの挿入延滞用のブリッヂ

#include	"compiler.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"diskdrv.h"
#include	"fddfile.h"


#define	DISK_DELAY	20			// (0.4sec)

	int		diskdrv_delay[4];							// ver0.26
	int		diskdrv_ro[4];								// ver0.26
	char	diskdrv_fname[4][MAX_PATH];					// ver0.26


void diskdrv_sethdd(BYTE drv, const char *fname) {

	char	*p;

	if (drv < 2) {
		p = np2cfg.hddfile[drv];
		if (fname) {
			file_cpyname(p, fname, sizeof(np2cfg.hddfile[0]));
		}
		else {
			p[0] = '\0';
		}
		sysmng_update(SYS_UPDATEHDD | SYS_UPDATECFG);
	}
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

