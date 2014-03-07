#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"


// �������ŏ������邩�@�V�����_�{�ړ��Ō떂�������Y�܂����Ƃ���

	UINT8	fdd320_stat;


static REG8 IOINPCALL fdd320_i51(UINT port) {

	(void)port;
	return(0x00);
}

static REG8 IOINPCALL fdd320_i55(UINT port) {

	fdd320_stat ^= 0xff;
	(void)port;
	return(fdd320_stat);
}


// ----

static const IOINP fdd320i51[4] = {
					fdd320_i51,	NULL,		fdd320_i55,	NULL};

void fdd320_reset(void) {

	fdd320_stat = 0xff;
}

void fdd320_bind(void) {

	iocore_attachcmninpex(0x0051, 0x00f9, fdd320i51, 4);
}

