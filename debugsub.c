#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"debugsub.h"


#if defined(MACOS)
#define	CRLITERAL	"\r"
#define	CRCONST		str_cr
#elif defined(X11) || defined(SLZAURUS)
#define	CRLITERAL	"\n"
#define	CRCONST		str_lf
#else
#define	CRLITERAL	"\r\n"
#define	CRCONST		str_crlf
#endif


static const char s_nv[] = "NV";
static const char s_ov[] = "OV";
static const char s_dn[] = "DN";
static const char s_up[] = "UP";
static const char s_di[] = "DI";
static const char s_ei[] = "EI";
static const char s_pl[] = "PL";
static const char s_ng[] = "NG";
static const char s_nz[] = "NZ";
static const char s_zr[] = "ZR";
static const char s_na[] = "NA";
static const char s_ac[] = "AC";
static const char s_po[] = "PO";
static const char s_pe[] = "PE";
static const char s_nc[] = "NC";
static const char s_cy[] = "CY";

static const char *flagstr[16][2] = {
				{NULL, NULL},		// 0x8000
				{NULL, NULL},		// 0x4000
				{NULL, NULL},		// 0x2000
				{NULL, NULL},		// 0x1000
				{s_nv, s_ov},		// 0x0800
				{s_dn, s_up},		// 0x0400
				{s_di, s_ei},		// 0x0200
				{NULL, NULL},		// 0x0100
				{s_pl, s_ng},		// 0x0080
				{s_nz, s_zr},		// 0x0040
				{NULL, NULL},		// 0x0020
				{s_na, s_ac},		// 0x0010
				{NULL, NULL},		// 0x0008
				{s_po, s_pe},		// 0x0004
				{NULL, NULL},		// 0x0002
				{s_nc, s_cy}};		// 0x0001

static const char file_i286reg[] = "i286reg.%.3u";
static const char file_i286cs[] = "i286_cs.%.3u";
static const char file_i286ds[] = "i286_ds.%.3u";
static const char file_i286es[] = "i286_es.%.3u";
static const char file_i286ss[] = "i286_ss.%.3u";
static const char file_i286txt[] = "i286txt.%.3u";
static const char file_memorybin[] = "memory.bin";

static const char str_register[] =										\
					"AX=%.4x  BX=%.4x  CX=%.4x  DX=%.4x  "				\
					"SP=%.4x  BP=%.4x  SI=%.4x  DI=%.4x" CRLITERAL		\
					"DS=%.4x  ES=%.4x  SS=%.4x  CS=%.4x  "				\
					"IP=%.4x   ";
static const char str_picstat[] = 										\
					CRLITERAL "PIC0=%.2x:%.2x:%.2x"						\
					CRLITERAL "PIC1=%.2x:%.2x:%.2x"						\
					CRLITERAL "8255PORTC = %.2x / system-port = %.2x";


const char *debugsub_flags(UINT16 flag) {

static char	work[128];
	int		i;
	UINT16	bit;

	work[0] = 0;
	for (i=0, bit=0x8000; bit; i++, bit>>=1) {
		if (flagstr[i][0]) {
			if (flag & bit) {
				milstr_ncat(work, flagstr[i][1], sizeof(work));
			}
			else {
				milstr_ncat(work, flagstr[i][0], sizeof(work));
			}
			if (bit != 1) {
				milstr_ncat(work, str_space, sizeof(work));
			}
		}
	}
	return(work);
}

const char *debugsub_regs(void) {

static char work[256];

	SPRINTF(work, str_register, CPU_AX, CPU_BX, CPU_CX, CPU_DX,
								CPU_SP, CPU_BP, CPU_SI, CPU_DI,
								CPU_DS, CPU_ES, CPU_SS, CPU_CS, CPU_IP);
	milstr_ncat(work, debugsub_flags(CPU_FLAG), sizeof(work));
	milstr_ncat(work, CRCONST, sizeof(work));
	return(work);
}

void debugsub_status(void) {

static int	filenum = 0;
	FILEH	fh;
	char	work[512];
const char	*p;

	SPRINTF(work, file_i286reg, filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		p = debugsub_regs();
		file_write(fh, p, strlen(p));
		SPRINTF(work, str_picstat,
								pic.pi[0].imr, pic.pi[0].irr, pic.pi[0].isr,
								pic.pi[1].imr, pic.pi[1].irr, pic.pi[1].isr,
								mouseif.portc, sysport.c);
		file_write(fh, work, strlen(work));
		file_close(fh);
	}

	SPRINTF(work, file_i286cs, filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		file_write(fh, &mem[CS_BASE], 0x10000);
		file_close(fh);
	}
	SPRINTF(work, file_i286ds, filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		file_write(fh, &mem[DS_BASE], 0x10000);
		file_close(fh);
	}
	SPRINTF(work, file_i286es, filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		file_write(fh, &mem[ES_BASE], 0x10000);
		file_close(fh);
	}
	SPRINTF(work, file_i286ss, filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		file_write(fh, &mem[SS_BASE], 0x10000);
		file_close(fh);
	}
	SPRINTF(work, file_i286txt, filenum);
	fh = file_create_c(work);
	if (fh != FILEH_INVALID) {
		file_write(fh, &mem[0xa0000], 0x4000);
		file_close(fh);
	}
	filenum++;
}

void debugsub_memorydump(void) {

	FILEH	fh;
	int		i;

	fh = file_create_c(file_memorybin);
	if (fh != FILEH_INVALID) {
//		for (i=0; i<34; i++) {
		for (i=0; i<64; i++) {
			file_write(fh, mem + i*0x8000, 0x8000);
		}
		file_close(fh);
	}
}

