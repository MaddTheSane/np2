#include	"compiler.h"
#include	"strres.h"
#include	"np2.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"sstp.h"
#include	"sstpres.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"np2info.h"


static char cr[] = "\\n";


// ---- np2info extend

static const char str_jwinclr[] =
						"256色\0ハイカラー\0フルカラー\0トゥルーカラー";
static const char str_jwinmode[] =
						" (窓モード)\0 (フルスクリーン)";


static void info_progtitle(char *str, int maxlen, NP2INFOEX *ex) {

	milstr_ncpy(str, np2oscfg.titles, maxlen);
}

static void info_jsound(char *str, int maxlen, NP2INFOEX *ex) {

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

		case 0x80:
			p = "AMD-98";
			break;

		default:
			p = "なし";
			break;
	}
	milstr_ncpy(str, p, maxlen);
}

static void info_jdisp(char *str, int maxlen, NP2INFOEX *ex) {

	UINT	bpp;

	bpp = scrnmng_getbpp();
	milstr_ncpy(str, milstr_list(str_jwinclr, ((bpp >> 3) - 1) & 3), maxlen);
	milstr_ncat(str, milstr_list(str_jwinmode, (scrnmng_isfullscreen())?1:0),
																	maxlen);
	(void)ex;
}

static void info_jbios(char *str, int maxlen, NP2INFOEX *ex) {

	str[0] = '\0';
	if (biosrom) {
		milstr_ncat(str, str_biosrom, maxlen);
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

static void info_jrhythm(char *str, int maxlen, NP2INFOEX *ex) {

const char	*p;
	char	jrhythmstr[16];
	UINT	exist;
	UINT	i;

	if (!(usesound & 0x6c)) {
		p = "不要やで";
	}
	else {
		exist = rhythm_getcaps();
		if (exist == 0) {
			p = "用意されてないんか…";
		}
		else if (exist == 0x3f) {
			p = "全部あるで";
		}
		else {
			milstr_ncpy(jrhythmstr, "BSCHTRや", sizeof(jrhythmstr));
			for (i=0; i<6; i++) {
				if (!(exist & (1 << i))) {
					jrhythmstr[i] = '_';
				}
			}
			p = jrhythmstr;
		}
	}
	milstr_ncpy(str, p, maxlen);
}

typedef struct {
	char	key[8];
	void	(*proc)(char *str, int maxlen, NP2INFOEX *ex);
} INFOPROC;

static const INFOPROC infoproc[] = {
			{"PROG",		info_progtitle},
			{"JSND",		info_jsound},
			{"JBIOS",		info_jbios},
			{"JDISP",		info_jdisp},
			{"JRHYTHM",		info_jrhythm}};

static BOOL sstpext(char *dst, const char *key, int maxlen, NP2INFOEX *ex) {

const INFOPROC	*inf;
const INFOPROC	*infterm;

	inf = infoproc;
	infterm = infoproc + (sizeof(infoproc) / sizeof(INFOPROC));
	while(inf < infterm) {
		if (!milstr_cmp(key, inf->key)) {
			inf->proc(dst, maxlen, ex);
			return(TRUE);
		}
		inf++;
	}
	return(FALSE);
}

static const NP2INFOEX sstpex = {"\\n", sstpext};


// ----

static const BYTE prs2[] = {0xaa,0xac,0xae,0xb0,0xb2,0xbe,0xf0,0x9f,
							0xa1,0xa3,0xa5,0xa7,0xe1,0xe3,0xe5,0xc1,
							0xb8,0xa0,0xa2,0xa4,0xa6,0xa8,0xa9,0xab,
							0xad,0xaf,0xb1,0xb3,0xb5,0xb7,0xb9,0xbb,
							0xbd,0xbf,0xc2,0xc4,0xc6,0xc8,0xc9,0xca,
							0xcb,0xcc,0xcd,0xd0,0xd3,0xd6,0xd9,0xdc,
							0xdd,0xde,0xdf,0xe0,0xe2,0xe4,0xe6,0xe7,
							0xe8,0xe9,0xea,0xeb,0xed,0xf1,0xb4,0xb8};

#define	GETSSTPDAT1(a) {								\
				(a) = last;								\
				last = *dat++;							\
				(a) += last;							\
				(a) = ((a) << 2) | ((a) >> 6);			\
		}

static char *sstpsolve(char *buf, const unsigned char *dat) {

	BYTE	c;
	BYTE	last;

	last = 0x80;
	while(1) {
		GETSSTPDAT1(c);
		if (!c) {
			break;
		}
		else if (c < 0x20) {
			if (c == 0x12) {
				*buf++ = 13;
				*buf++ = 10;
			}
			else {
				*buf++ = '\\';
				if (c < 0x1b) {
					*buf++ = (c + 0x60);
				}
				else {
					*buf++ = 's';
					*buf++ = (c + 0x30 - 0x1b);
				}
			}
		}
		else if (c < 0x7f) {
			*buf++ = c;
		}
		else if (c == 0x7f) {
			BYTE ms;
			GETSSTPDAT1(ms);
			if (!ms) {
				break;
			}
			while(ms > 10) {
				CopyMemory(buf, "\\w9", 3);
				buf += 3;
				ms -= 10;
			}
			if (ms) {
				wsprintf(buf, "\\w%u", ms);
				buf += 3;
			}
		}
		else if (c == 0x80) {
			BYTE c2;
			GETSSTPDAT1(c2);
			if (c2) {
				*buf++ = c2;
			}
			else {
				break;
			}
		}
		else if (c >= 0xf0) {
			int i;
			const BYTE *p;
			i = c - 0xf0;
			if (c == 0xff) {
				BYTE c2;
				GETSSTPDAT1(c2);
				if (!c2) {
					break;
				}
				i += (c2 - 1);
			}
			p = xitems;
			while(i--) {
				p += xitems2[i];
			}
			buf = sstpsolve(buf, p);
		}
		else if ((c >= 0xa0) && (c < 0xe0)) {
			*buf++ = (BYTE)0x82;
			*buf++ = prs2[c-0xa0];
		}
		else {
			BYTE c2;
			GETSSTPDAT1(c2);
			if (c2) {
				*buf++ = c;
				*buf++ = c2;
			}
			else {
				break;
			}
		}
	}
	*buf = '\0';
	return(buf);
}


// -------------------------------

static int check_keropi(void) {

	char	buf[64];

	sstpsolve(buf, k_keropi);
	if (FindWindow(buf, NULL)) {
		return(1);
	}
	sstpsolve(buf, k_winx68k);
	if (FindWindow(buf, NULL)) {
		return(2);
	}
	sstpsolve(buf, k_t98next);
	if (FindWindow(NULL, buf)) {
		return(3);
	}
	sstpsolve(buf, k_anex86);
	if (FindWindow(NULL, buf)) {
		return(4);
	}
	return(0);
}


// ------------------------------------------------------------------------

void sstpmsg_welcome(void) {

	int		kero;
	char	*p;
	char	buf[512];

	p = buf;

	kero = check_keropi();
	if (!kero) {
		switch(rand() & 15) {
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
				p = sstpsolve(p, s_welcome1);
				break;

			case 0x04:
			case 0x05:
			case 0x06:
				p = sstpsolve(p, s_welcome2);
				break;

			case 0x07:
			case 0x08:
				p = sstpsolve(p, s_welcome3);
				break;

			case 0x09:
				p = sstpsolve(p, s_welcome4);
				break;

			case 0x0a:
				p = sstpsolve(p, s_welcome5);
				break;

			default:
				p = sstpsolve(p, s_welcome6);
				break;
		}
	}
	else {
		switch(kero) {
			case 1:
				p = sstpsolve(p, s_keropi0);
				switch(rand() & 3) {
					case 0:
						p = sstpsolve(p, s_keropi1);
						break;
					case 1:
						p = sstpsolve(p, s_keropi2);
						break;
					default:
						p = sstpsolve(p, s_keropi3);
						break;
				}
				break;
			case 2:
				p = sstpsolve(p, s_winx68k);
				break;
			case 3:
				p = sstpsolve(p, s_t98next);
				break;
			case 4:
				p = sstpsolve(p, s_anex86);
				break;
			default:
				p = sstpsolve(p, s_error);
				break;
		}
	}
	sstp_send(buf, NULL);
}

void sstpmsg_reset(void) {

	char	str[1024];
	UINT	update;

	str[0] = '\0';

	update = sys_updates;
	if (update & SYS_UPDATECLOCK) {
		strcat(str, "ＣＰＵクロックを %CLOCK%に");
	}
	if (update & SYS_UPDATEMEMORY) {
		if (str[0]) {
			strcat(str, cr);
		}
		strcat(str, "メモリを %MEM3%に");
	}
	if (update & SYS_UPDATESBOARD) {
		if (str[0]) {
			strcat(str, cr);
		}
		strcat(str, "音源を %JSND%に");
	}
	if (update & (SYS_UPDATERATE | SYS_UPDATESBUF | SYS_UPDATEMIDI |
					SYS_UPDATEHDD | SYS_UPDATESERIAL1)) {
		BOOL hit = FALSE;
		if (str[0]) {
			strcat(str, "\\nあと…\\w5");
		}
		if (update & SYS_UPDATEMIDI) {
			hit = TRUE;
			strcat(str, "MIDI");
		}
		if (update & (SYS_UPDATERATE | SYS_UPDATESBUF)) {
			if (hit) {
				strcat(str, " ");
			}
			hit = TRUE;
			strcat(str, "サウンド設定");
		}
		if (update & SYS_UPDATEHDD) {
			if (hit) {
				strcat(str, " ");
			}
			hit = TRUE;
			strcat(str, "ハードディスク");
		}
		if (update & SYS_UPDATESERIAL1) {
			if (hit) {
				strcat(str, " ");
			}
			hit = TRUE;
			strcat(str, "シリアル");
		}
		strcat(str, "の設定を");
	}
	if (str[0]) {
		char out[1024];
		strcat(str, "変更しました。");
		np2info(out, str, sizeof(out), &sstpex);
		sstp_send(out, NULL);
	}
}


void sstpmsg_about(void) {

	char	str[1024];
	char	out[1024];
	char	*p;
	int		nostat = FALSE;

	p = str;
	switch(rand() & 7) {
		case 0:
		case 1:
		case 2:
		case 3:
			p = sstpsolve(p, s_ver0);
			break;

		case 4:
		case 5:
			p = sstpsolve(p, s_ver1);
			break;
		case 6:
			p = sstpsolve(p, s_ver2);
			break;

		case 7:
			p = sstpsolve(p, s_ver3);
			nostat = TRUE;
			break;
	}
	if (!nostat) {
		p = sstpsolve(p, s_info);
	}
	np2info(out, str, sizeof(out), &sstpex);
	sstp_send(out, NULL);
}


void sstpmsg_config(void) {

	char	str[1024];
	char	*p;

	p = sstpsolve(str, s_config0);
	switch(rand() & 7) {
		case 0:
			p = sstpsolve(p, s_config1);
			break;
		case 1:
			p = sstpsolve(p, s_config2);
			break;
		case 2:
		case 3:
			p = sstpsolve(p, s_config3);
			break;
		default:
			p = sstpsolve(p, s_config4);
			break;
	}
	sstp_send(str, NULL);
}


// ----

static char *get_code(char *buf, int *ret) {

	int	stat;

	stat = 0;
	if (!memcmp(buf, "SSTP", 4)) {
		while(*buf) {
			if (*buf++ == ' ') {
				break;
			}
		}
		while((*buf >= '0') && (*buf <= '9')) {
			stat *= 10;
			stat += (*buf++ - '0');
		}
		while(*buf) {
			if (*buf++ == '\n') {
				break;
			}
		}
	}
	if (ret) {
		*ret = stat;
	}
	return(buf);
}

static void e_sstpreset(HWND hWnd, char *buf) {

	char	*p;
	int		ret;

	p = get_code(buf, &ret);
	if (ret == 200) {
		if (!memcmp(p, "いい", 4)) {
			SendMessage(hWnd, WM_NP2CMD, 0, NP2CMD_RESET);
		}
	}
}

BOOL sstpconfirm_reset(void) {

	char	str[256];

	sstpsolve(str, s_reset);
	return(sstp_send(str, e_sstpreset));
}

static void e_sstpexit(HWND hWnd, char *buf) {

	char	*p;
	int		ret;

	p = get_code(buf, &ret);
	if (ret == 200) {
		if (!memcmp(p, "いい", 4)) {
			SendMessage(hWnd, WM_NP2CMD, 0, NP2CMD_EXIT);
		}
	}
}

BOOL sstpconfirm_exit(void) {

	char	str[512];

	sstpsolve(str, s_exit);
	return(sstp_send(str, e_sstpexit));
}


// ------------------------------------------------------------ 単発。

BOOL sstpmsg_running(void) {

	char	buf[256];
	char	*p;

	p = buf;
	switch(rand() & 7) {
		case 0:
			p = sstpsolve(p, s_running1);
			break;
		case 1:
			p = sstpsolve(p, s_running2);
			break;
		case 2:
			p = sstpsolve(p, s_running3);
			break;
		default:
			p = sstpsolve(p, s_running4);
			break;
	}
	return(sstp_sendonly(buf));
}


BOOL sstpmsg_dxerror(void) {

	char	buf[256];

	sstpsolve(buf, s_dxerror);
	return(sstp_sendonly(buf));
}

