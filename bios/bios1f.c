#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"


static REG8 bios0x1f_90(void) {

	BYTE	work[256];
	UINT	srclimit;
	UINT	srcaddr;
	UINT	dstlimit;
	UINT	dstaddr;
	UINT32	srcbase;
	UINT32	dstbase;
	UINT	leng;
	UINT	l;

	MEML_READSTR(CPU_ES, CPU_BX + 0x10, work, 0x10);
	srclimit = work[0] + (work[1] << 8) + 1;
	srcaddr = CPU_SI;
	if (srclimit <= srcaddr) {
		goto p90_err;
	}
	dstlimit = work[8] + (work[9] << 8) + 1;
	dstaddr = CPU_DI;
	if (dstlimit <= dstaddr) {
		goto p90_err;
	}

	srcbase = work[2] + (work[3] << 8) + (work[4] << 16);
	dstbase = work[10] + (work[11] << 8) + (work[12] << 16) + dstaddr;
	leng = LOW16(CPU_CX - 1) + 1;
	do {
		l = min(leng, sizeof(work));
		l = min(l, srclimit - srcaddr);
		l = min(l, dstlimit - dstaddr);
		if (!l) {
			goto p90_err2;
		}
		MEML_READ(srcbase + srcaddr, work, l);
		MEML_WRITE(dstbase + dstaddr, work, l);
		srcaddr = LOW16(srcaddr + l);
		dstaddr = LOW16(dstaddr + l);
		leng -= l;
	} while(leng);
	TRACEOUT(("BIOS1F90 - success"));
	return(0);

p90_err2:
	TRACEOUT(("BIOS1F90 - remain %dbytes", leng));

p90_err:
	TRACEOUT(("BIOS1F90 - failure"));
	return(C_FLAG);
}


// ----

void bios0x1f(void) {

	REG8	cflag;
	REG8	flag;

	if (!(CPU_AH & 0x80)) {
		return;
	}
	if (!(CPU_AH & 0x10)) {
		cflag = 0;
	}
	else if (CPU_AH == 0x90) {
		cflag = bios0x1f_90();
	}
	else {
		return;
	}
	flag = MEML_READ8(CPU_SS, CPU_SP + 4) & (~C_FLAG);
	flag |= cflag;
	MEML_WRITE8(CPU_SS, CPU_SP + 4, flag);
}

