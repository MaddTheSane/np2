#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"debugsub.h"


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

static const char file_i286reg[] = "i286reg.%03u";
static const char file_i286cs[] = "i286_cs.%03u";
static const char file_i286ds[] = "i286_ds.%03u";
static const char file_i286es[] = "i286_es.%03u";
static const char file_i286ss[] = "i286_ss.%03u";
static const char file_i286txt[] = "i286txt.%03u";
static const char file_memorybin[] = "memory.bin";

static const char str_register[] =									\
					"AX=%04x  BX=%04x  CX=%04x  DX=%04x  "			\
					"SP=%04x  BP=%04x  SI=%04x  DI=%04x\n"			\
					"DS=%04x  ES=%04x  SS=%04x  CS=%04x  "			\
					"IP=%04x   ";
static const char str_picstat[] = 									\
					"\nPIC0=%02x:%02x:%02x\nPIC1=%02x:%02x:%02x\n"	\
					"8255PORTC = %02x / system-port = %02x";


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

	SPRINTF(work, str_register, I286_AX, I286_BX, I286_CX, I286_DX,
								I286_SP, I286_BP, I286_SI, I286_DI,
								I286_DS, I286_ES, I286_SS, I286_CS, I286_IP);
	milstr_ncat(work, debugsub_flags(I286_FLAG), sizeof(work));
	milstr_ncat(work, str_cr, sizeof(work));
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
		for (i=0; i<34; i++) {
			file_write(fh, mem + i*0x8000, 0x8000);
		}
		file_close(fh);
	}
}

