#include	"compiler.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"sstp.h"
#include	"sstpres.h"
#include	"np2info.h"
#include	"pccore.h"


static char cr[] = "\\n";

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
		strcat(str, "ＣＰＵクロックを !CLOCKに");
	}
	if (update & SYS_UPDATEMEMORY) {
		if (str[0]) {
			strcat(str, cr);
		}
		strcat(str, "メモリを !MEM3に");
	}
	if (update & SYS_UPDATESBOARD) {
		if (str[0]) {
			strcat(str, cr);
		}
		strcat(str, "音源を !JSNDに");
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
		np2info(out, str, sizeof(out));
		sstp_send(out, NULL);
	}
}


void sstpmsg_about(void) {

	char	str[2048];
	char	out[2048];
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
	np2info(out, str, sizeof(out));
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

// -----------------------------------------------------------

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

#if 0
static void e_sstpexit2(HWND hWnd, char *buf) {

	char	*p;
	int		ret;

	p = get_code(buf, &ret);
	if (ret == 200) {
		if (!memcmp(p, "いい", 4)) {
			SendMessage(hWnd, WM_NP2CMD, 0, NP2CMD_EXIT2);
		}
	}
}
#endif

BOOL sstpconfirm_exit(void) {

	char	str[512];

#if 1
	sstpsolve(str, s_exit);
	return(sstp_send(str, e_sstpexit));
#else
	if (rand() & 3) {
		sstpsolve(str, s_exit);
		return(sstp_send(str, e_sstpexit));
	}
	else {
		sstpsolve(str, s_exit2);
		return(sstp_send(str, e_sstpexit2));
	}
#endif
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

