#include	"compiler.h"
#include	"strres.h"
#include	"np2.h"
#include	"scrnmng.h"
#include	"np2info.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"sound.h"
#include	"fmboard.h"


static const char str_2halfMHz[] = "2.5MHz";
#define str_5MHz	(str_2halfMHz + 2)
static const char str_8MHz[] = "8MHz";
static const char str_notexist[] = "not exist";
static const char str_disable[] = "disable";

static const char str_8bpp[] = "256-colors";
static const char str_16bpp[] = "65536-colors";
static const char str_24bpp[] = "full color";
static const char str_32bpp[] = "true color";
static const char *winclr[4] = {str_8bpp, str_16bpp, str_24bpp, str_32bpp};

static const char str_j8bpp[] = "256-colors";
static const char str_j16bpp[] = "65536-colors";
static const char *jwinclr[4] = {str_j8bpp, str_j16bpp, str_24bpp, str_32bpp};

static const char str_V30[] = "V30";
static const char str_i286[] = "i286";

static const char str_clockfmt[] = "%d.%1dMHz";
static const char str_memfmt[] = "%3uKB";
static const char str_memfmt2[] = "%3uKB + %uKB";
static const char str_memfmt3[] = "%d.%1dMB";

static const char str_grcg[] = "GRCG ";
static const char str_grcgcgwin[] = "GRCG CG-Window ";
static const char str_egc[] = "EGC CG-Window ";
static const char *grcgchip[4] = {str_null, str_grcg, str_grcgcgwin, str_egc};


#ifdef __cplusplus
extern "C" {
#endif

extern	RHYTHMCFG	rhythmcfg;

#ifdef __cplusplus
}
#endif


// ---- common

static void info_progtitle(char *str, int maxlen) {

	milstr_ncpy(str, np2oscfg.titles, maxlen);
}

static void info_ver(char *str, int maxlen) {

	milstr_ncpy(str, np2version, maxlen);
}

static void info_cpu(char *str, int maxlen) {

	milstr_ncpy(str, (CPU_TYPE & CPUTYPE_V30)?str_V30:str_i286, maxlen);
}

static void info_clock(char *str, int maxlen) {

	UINT32	clock;
	char	clockstr[16];

	clock = (pccore.realclock + 50000) / 100000;
	SPRINTF(clockstr, str_clockfmt, clock/10, clock % 10);
	milstr_ncpy(str, clockstr, maxlen);
}

static void info_base(char *str, int maxlen) {

	milstr_ncpy(str, (pccore.baseclock == PCBASECLOCK25)?str_5MHz:str_8MHz,
																	maxlen);
}

static void info_mem1(char *str, int maxlen) {

	UINT	memsize;
	char	memstr[32];

	memsize = np2cfg.memsw[2] & 7;
	if (memsize < 6) {
		memsize = (memsize + 1) * 128;
	}
	else {
		memsize = 640;
	}
	if (extmem.maxmem > 1) {
		SPRINTF(memstr, str_memfmt2, memsize, (extmem.maxmem - 1) * 1024);
	}
	else {
		SPRINTF(memstr, str_memfmt, memsize);
	}
	milstr_ncpy(str, memstr, maxlen);
}

static void info_mem2(char *str, int maxlen) {

	UINT	memsize;
	char	memstr[16];

	memsize = np2cfg.memsw[2] & 7;
	if (memsize < 6) {
		memsize = (memsize + 1) * 128;
	}
	else {
		memsize = 640;
	}
	if (extmem.maxmem > 1) {
		memsize += (extmem.maxmem - 1) * 1024;
	}
	SPRINTF(memstr, str_memfmt, memsize);
	milstr_ncpy(str, memstr, maxlen);
}

static void info_mem3(char *str, int maxlen) {

	UINT	memsize;
	char	memstr[16];

	memsize = np2cfg.memsw[2] & 7;
	if (memsize < 6) {
		memsize = (memsize + 1) * 128;
	}
	else {
		memsize = 640;
	}
	if (extmem.maxmem > 1) {
		SPRINTF(memstr, str_memfmt3, extmem.maxmem - 1, memsize / 100);
	}
	else {
		SPRINTF(memstr, str_memfmt, memsize);
	}
	milstr_ncpy(str, memstr, maxlen);
}

static void info_gdc(char *str, int maxlen) {

	milstr_ncpy(str, grcgchip[grcg.chip], maxlen);
	milstr_ncat(str, str_2halfMHz + ((gdc.clock & 0x80)?2:0), maxlen);
}

static void info_text(char *str, int maxlen) {

const char	*p;
	char	textstr[64];

	if (!(gdcs.textdisp & GDCSCRN_ENABLE)) {
		p = str_disable;
	}
	else {
		SPRINTF(textstr, "width-%d", ((gdc.mode1 & 0x4)?40:80));
		p = textstr;
	}
	milstr_ncpy(str, p, maxlen);
}

static void info_grph(char *str, int maxlen) {

const char	*p;
	char	grphstr[64];

	if (!(gdcs.grphdisp & GDCSCRN_ENABLE)) {
		p = str_disable;
	}
	else {
		SPRINTF(grphstr, "page-%d  ", gdcs.access);
		if (gdc.analog) {
			milstr_ncat(grphstr, "(Analog)", sizeof(grphstr));
		}
		else {
			milstr_ncat(grphstr, "(Degital)", sizeof(grphstr));
		}
		p = grphstr;
	}
	milstr_ncpy(str, p, maxlen);
}

static void info_sound(char *str, int maxlen) {

const char	*p;

	switch(usesound) {
		case 0x01:
			p = "PC-9801-14";
			break;

		case 0x02:
			p = "PC-9801-26";
			break;

		case 0x04:
			p = "PC-9801-86";
			break;

		case 0x06:
			p = "PC-9801-26 + 86";
			break;

		case 0x08:
			p = "PC-9801-118(Hardware)";
			break;

		case 0x14:
			p = "PC-9801-86 + Chibi-oto";
			break;

		case 0x20:
			p = "Speak board";
			break;

		case 0x40:
			p = "Spark board";
			break;

		default:
			p = "none";
			break;
	}
	milstr_ncpy(str, p, maxlen);
}

static void info_extsnd(char *str, int maxlen) {

static const char *chpan[4] = {"none", "Mono-R", "Mono-L", "Stereo"};
	char	buf[256];

	info_sound(str, maxlen);
	if (usesound & 4) {
		milstr_ncat(str, "\r\n   PCM: ", maxlen);
		SPRINTF(buf, "%dHz %dbit %s\r\n        %d / %d / 32768",
							pcm86rate8[pcm86.fifo & 7] >> 3,
							(16 - ((pcm86.dactrl >> 3) & 8)),
							chpan[(pcm86.dactrl >> 4) & 3],
							pcm86.virbuf, pcm86.fifosize);
		milstr_ncat(str, buf, maxlen);
	}
}

static void info_bios(char *str, int maxlen) {

	str[0] = '\0';
	if (biosrom) {
		milstr_ncat(str, "BIOS.ROM", maxlen);
	}
	if (soundrom.name[0]) {
		if (str[0]) {
			milstr_ncat(str, ", ", maxlen);
		}
		milstr_ncat(str, soundrom.name, maxlen);
	}
	if (str[0] == '\0') {
		milstr_ncat(str, str_notexist, maxlen);
	}
}

static void info_rhythm(char *str, int maxlen) {

	char	rhythmstr[8];
	UINT	i;

	milstr_ncpy(rhythmstr, "BSCHTR", sizeof(rhythmstr));
	for (i=0; i<6; i++) {
		if (rhythmcfg.pcm[i].data == NULL) {
			rhythmstr[i] = '_';
		}
	}
	milstr_ncpy(str, rhythmstr, maxlen);
}

static void info_display(char *str, int maxlen) {

	UINT	bpp;
const char	*p;

	bpp = scrnmng_getbpp();
	milstr_ncpy(str, winclr[((bpp >> 3) - 1) & 3], maxlen);
	if (scrnmng_isfullscreen()) {
		p = " (fullscreen)";
	}
	else {
		p = " (window)";
	}
	milstr_ncat(str, p, maxlen);
}


// ------------------------------------------------------------------ japanese

static void info_jsound(char *str, int maxlen) {

const char	*p;

	switch(usesound) {
		case 0x01:
			p = "14ボード";
			break;

		case 0x02:
			p = "26音源";
			break;

		case 0x04:
			p = "86音源";
			break;

		case 0x06:
			p = "２枚刺し";
			break;

		case 0x08:
			p = "118音源";
			break;

		case 0x14:
			p = "86音源(ちびおと付)";
			break;

		case 0x20:
			p = "スピークボード";
			break;

		case 0x40:
			p = "スパークボード";
			break;

		default:
			p = "なし";
			break;
	}
	milstr_ncpy(str, p, maxlen);
}

static void info_jdisp(char *str, int maxlen) {

	UINT	bpp;
const char	*p;

	bpp = scrnmng_getbpp();
	milstr_ncpy(str, jwinclr[((bpp >> 3) - 1) & 3], maxlen);
	if (scrnmng_isfullscreen()) {
		p = " (フルスクリーン)";
	}
	else {
		p = " (窓表示)";
	}
	milstr_ncat(str, p, maxlen);
}

static void info_jbios(char *str, int maxlen) {

	str[0] = '\0';
	if (biosrom) {
		milstr_ncat(str, "BIOS.ROM", maxlen);
	}
	if (soundrom.name[0]) {
		if (str[0]) {
			milstr_ncat(str, "と", maxlen);
		}
		milstr_ncat(str, soundrom.name, maxlen);
	}
	if (str[0] == '\0') {
		milstr_ncat(str, "なし", maxlen);
	}
}

static void info_jrhythm(char *str, int maxlen) {

const char	*p;
	char	jrhythmstr[16];
	UINT	rhythmfault;
	UINT	i;

	if (!(usesound & 0x6c)) {
		p = "不要やで";
	}
	else {
		milstr_ncpy(jrhythmstr, "BSCHTRや", sizeof(jrhythmstr));
		rhythmfault = 0;
		for (i=0; i<6; i++) {
			if (rhythmcfg.pcm[i].data == NULL) {
				jrhythmstr[i] = '_';
				rhythmfault++;
			}
		}
		if (!rhythmfault) {
			p = "全部あるで";
		}
		else if (rhythmfault == 6) {
			p = "用意されてないんか…";
		}
		else {
			p = jrhythmstr;
		}
	}
	milstr_ncpy(str, p, maxlen);
}


// ---- make string

typedef struct {
const char	*key;
	void	(*proc)(char *str, int maxlen);
} META_PROC;

static const META_PROC metaproc[] = {
			{"PROG",		info_progtitle},
			{"VER",			info_ver},
			{"CPU",			info_cpu},
			{"CLOCK",		info_clock},
			{"BASE",		info_base},
			{"MEM1",		info_mem1},
			{"MEM2",		info_mem2},
			{"MEM3",		info_mem3},
			{"GDC",			info_gdc},
			{"TEXT",		info_text},
			{"GRPH",		info_grph},
			{"SND",			info_sound},
			{"EXSND",		info_extsnd},
			{"BIOS",		info_bios},
			{"RHYTHM",		info_rhythm},
			{"DISP",		info_display},

			// japanese
			{"JSND",		info_jsound},
			{"JBIOS",		info_jbios},
			{"JDISP",		info_jdisp},
			{"JRHYTHM",		info_jrhythm}};


void np2info(char *dst, const char *src, int maxlen) {

	int		pos;
	char	c;
	UINT	i;
	int		leng;

	if ((dst == NULL) || (src == NULL)) {
		return;
	}
	pos = 0;
	while((pos + 1) < maxlen) {
		c = *src++;
		if (c == '\0') {
			break;
		}
		else if (c == '!') {
			if (*src == c) {
				src++;
				dst[pos++] = c;
			}
			else {
				for (i=0; i<(sizeof(metaproc)/sizeof(META_PROC)); i++) {
					leng = strlen(metaproc[i].key);
					if (!memcmp(src, metaproc[i].key, leng)) {
						src += leng;
						metaproc[i].proc(dst + pos, maxlen - pos);
						pos = strlen(dst);
						break;
					}
				}
			}
		}
		else {
			dst[pos++] = c;
		}
	}
	dst[pos] = '\0';
}

