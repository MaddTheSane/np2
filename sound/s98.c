//
//  PC-98 Sound logging
//    for S98amp S98 Input plugin for Winamp Version 1.3.1+ by Mamiya
//

#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"


#define S98LOG_BUFSIZE (32*1024)

typedef struct {						// UINT32�� ��� 0fill����Ă�
	BYTE	magic[3];
	BYTE	formatversion;
	UINT32	timerinfo;
	UINT32	timerinfo2;
	UINT32	compressing;
	UINT32	offset;
	UINT32	dumpdata;
	UINT32	looppoint;
	BYTE	headerreserved[0x24];
	BYTE	title[0x40];
} S98HDR;

static const S98HDR s98hdr =
					{{'S','9','8'}, '1', 1, 0, 0, 0x40, 0x80, 0, {0}, {0}};

static struct {
	FILEH	fh;
	UINT32	intcount;
	SINT32	clock;
	UINT	p;
	BYTE	buf[S98LOG_BUFSIZE];
} s98log;


static void s98timer(NEVENTITEM item);

static void sets98event(BOOL absolute) {

	s98log.intcount++;
	nevent_set(NEVENT_S98TIMER, s98log.clock, s98timer, NEVENT_RELATIVE);
	(void)absolute;
}

static void s98timer(NEVENTITEM item) {

	if (s98log.fh != FILEH_INVALID) {
		sets98event(NEVENT_RELATIVE);
	}
	(void)item;
}

static void S98_flush(void) {

	if (s98log.p) {
		file_write(s98log.fh, s98log.buf, s98log.p);
		s98log.p = 0;
	}
}

static void S98_putc(BYTE data) {

	s98log.buf[s98log.p++] = data;
	if (s98log.p == S98LOG_BUFSIZE) {
		S98_flush();
	}
}

static void S98_putint(void) {

	if (s98log.intcount) {
		if (s98log.intcount == 1) {
			S98_putc(0xFF);					/* SYNC(1) */
		}
		else if (s98log.intcount == 2) {
			S98_putc(0xFF);					/* SYNC(1) */
			S98_putc(0xFF);					/* SYNC(1) */
		}
		else {
			S98_putc(0xFE);					/* SYNC(n) */
			s98log.intcount -= 2;
			while (s98log.intcount > 0x7f) {
				S98_putc((BYTE)(0x80 | (s98log.intcount & 0x7f)));
				s98log.intcount >>= 7;
			}
			S98_putc((BYTE)(s98log.intcount & 0x7f));
		}
		s98log.intcount = 0;
	}
}


// ----

void S98_init(void) {

	s98log.fh = FILEH_INVALID;
}

void S98_trash(void) {

	S98_close();
}

BOOL S98_open(const char *filename) {

	UINT	i;

	// �t�@�C���̃I�[�v��
	s98log.fh = file_create(filename);
	if (s98log.fh == FILEH_INVALID) {
		return(FAILURE);
	}

	// ������
	s98log.clock = pc.realclock / 1000;
	s98log.p = 0;

	// �w�b�_�̕ۑ�
	for (i=0; i<sizeof(s98hdr); i++) {
		S98_putc(*(((BYTE *)&s98hdr) + i));
	}

#if 0
	// FM
	for (i=0x30; i<0xb6; i++) {
		if ((i & 3) != 3) {
			S98_putc(NORMAL2608);
			S98_putc((BYTE)i);
			S98_putc(opna.reg.b[i]);
			S98_putc(EXTEND2608);
			S98_putc((BYTE)i);
			S98_putc(opna.reg.b[i+0x100]);
		}
	}
	// PSG
	for (i=0x00; i<0x0e; i++) {
		S98_putc(NORMAL2608);
		S98_putc((BYTE)i);
		S98_putc(opna.reg.b[i]);
	}
#endif

	// �ꉞ�p�f�B���O
	s98log.intcount = 10;

	sets98event(NEVENT_ABSOLUTE);
	return(SUCCESS);
}

void S98_close(void) {

	if (s98log.fh != FILEH_INVALID) {
		S98_putint();
		S98_putc(0xFD);				/* END MARK */
		S98_flush();
		nevent_reset(NEVENT_S98TIMER);
		file_close(s98log.fh);
		s98log.fh = FILEH_INVALID;
	}
}

void S98_put(BYTE module, BYTE addr, BYTE data) {

	if (s98log.fh != FILEH_INVALID) {
		S98_putint();
		S98_putc(module);
		S98_putc(addr);
		S98_putc(data);
	}
}

