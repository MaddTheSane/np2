#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"cpucore.h"
#include	"font.h"
#include	"fontdata.h"
#include	"fontmake.h"


#ifndef FONTMEMORYBIND
	BYTE	__font[0x84000];
#endif

static const char fonttmpname[] = "font.tmp";


void font_init(void) {

	BYTE	*p;
	BYTE	*q;
	UINT	i;
	UINT	j;
	UINT32	dbit;

	ZeroMemory(fontrom, sizeof(fontrom));
	p = fontrom + 0x81000;
	q = fontrom + 0x82800;
	for (i=0; i<256; i++) {
		for (j=0; j<4; j++) {
			dbit = 0;
			if (i & (0x01 << j)) {
				dbit |= 0xf0f0f0f0;
			}
			if (i & (0x10 << j)) {
				dbit |= 0x0f0f0f0f;
			}
			*(UINT32 *)p = dbit;
			p += 4;
			*(UINT16 *)q = (UINT16)dbit;
			q += 2;
		}
	}
}

static BYTE fonttypecheck(const char *fname) {

const char	*p;

	p = file_getext((char *)fname);
	if (!file_cmpname(p, str_bmp)) {
		return(FONTTYPE_PC98);
	}
	p = file_getname((char *)fname);
	if (!file_cmpname(p, v98fontname)) {
		return(FONTTYPE_V98);
	}
	if ((!file_cmpname(p, pc88ankname)) ||
		(!file_cmpname(p, pc88knj1name)) ||
		(!file_cmpname(p, pc88knj2name))) {
		return(FONTTYPE_PC88);
	}
	if ((!file_cmpname(p, fm7ankname)) ||
		(!file_cmpname(p, fm7knjname))) {
		return(FONTTYPE_FM7);
	}
	if ((!file_cmpname(p, x1ank1name)) ||
		(!file_cmpname(p, x1ank2name)) ||
		(!file_cmpname(p, x1knjname))) {
		return(FONTTYPE_X1);
	}
	if (!file_cmpname(p, x68kfontname)) {
		return(FONTTYPE_X68);
	}
	return(FONTTYPE_NONE);
}

BYTE font_load(const char *filename, BOOL force) {

	UINT	i;
	BYTE	*q;
const BYTE	*p;
	UINT	j;
	char	fname[MAX_PATH];
	BYTE	type;
	BYTE	loading;

	if (filename) {
		file_cpyname(fname, filename, sizeof(fname));
	}
	else {
		fname[0] = '\0';
	}
	type = fonttypecheck(fname);
	if ((!type) && (!force)) {
		return(0);
	}

	// �O��: font[??560-??57f], font[??d60-??d7f] �͍��Ȃ��悤�Ɂc
	for (i=0; i<0x80; i++) {
		q = fontrom + (i << 12);
		ZeroMemory(q + 0x000, 0x0560 - 0x000);
		ZeroMemory(q + 0x580, 0x0d60 - 0x580);
		ZeroMemory(q + 0xd80, 0x1000 - 0xd80);
	}

	CopyMemory(fontrom + 0x82000, fontdata_8, 0x800);
	p = fontdata_8;
	q = fontrom + 0x80000;
	for (i=0; i<256; i++) {
		for (j=0; j<8; j++) {
			q[0] = p[0];
			q[1] = p[0];
			p += 1;
			q += 2;
		}
	}

	loading = 0xff;
	switch(type) {
		case FONTTYPE_PC98:
			loading = fontpc98_read(fname, loading);
			break;

		case FONTTYPE_V98:
			loading = fontv98_read(fname, loading);
			break;

		case FONTTYPE_PC88:
			loading = fontpc88_read(fname, loading);
			break;

		case FONTTYPE_FM7:
			loading = fontfm7_read(fname, loading);
			break;

		case FONTTYPE_X1:
			loading = fontx1_read(fname, loading);
			break;

		case FONTTYPE_X68:
			loading = fontx68k_read(fname, loading);
			break;
	}
	loading = fontpc98_read(file_getcd(pc98fontname), loading);
	loading = fontv98_read(file_getcd(v98fontname), loading);
	loading = fontpc88_read(file_getcd(pc88ankname), loading);
	if (loading & FONTLOAD_16) {
		file_cpyname(fname, file_getcd(fonttmpname), sizeof(fname));
		if (file_attr(fname) == -1) {
			makepc98bmp(fname);
		}
		loading = fontpc98_read(fname, loading);
	}
	return(type);
}

