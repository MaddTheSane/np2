#include	"compiler.h"

#ifdef	TRACEOUT
#undef	TRACEOUT
#endif
#define	TRACEOUT(s)	trace_fmt s

// ����Ascsicmd�Ƃǂ���������̂�H

#if defined(SUPPORT_IDEIO)

#include	"dosio.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"ideio.h"
#include	"atapicmd.h"
#include	"sxsi.h"

#define	YUIDEBUG


// INQUIRY
static const UINT8 cdrom_inquiry[] = {
#ifdef YUIDEBUG
	// �����̃h���C�u�̓z NECCD�� Product Level 3.00�ȏ�� modesense10�̃R�[�h�������[
	0x05,	// CD-ROM
	0x80,	// bit7: Removable Medium Bit, other: Reserved
	0x00,	// version [7-6: ISO, ECMA: 5-3, 2-0: ANSI(00)]
	0x21,	// 7-4: ATAPI version, 3-0: Response Data Format
	0x1f,	// Additional length
	0x00,0x00,0x00,	// Reserved
	'N', 'E', 'C', ' ', ' ', ' ', ' ', ' ',	// Vendor ID
	'C', 'D', '-', 'R', 'O', 'M', ' ', 'D',	// Product ID
	'R', 'I', 'V', 'E', ':', '2', '5', '1',	// Product ID
	'4', '.', '0', '9'	// Product Revision Level
#else
	0x05,	// CD-ROM
	0x80,	// bit7: Removable Medium Bit, other: Reserved
	0x00,	// version [7-6: ISO, ECMA: 5-3, 2-0: ANSI(00)]
	0x21,	// 7-4: ATAPI version, 3-0: Response Data Format
	0x1f,	// Additional length
	0x00,0x00,0x00,	// Reserved
	'N', 'E', 'C', ' ', ' ', ' ', ' ', ' ',	// Vendor ID
	'C', 'D', '-', 'R', 'O', 'M', ' ', 'D',	// Product ID
	'R', 'I', 'V', 'E', ' ', ' ', ' ', ' ',	// Product ID
	'1', '.', '0', ' '	// Product Revision Level
#endif
};

static void senddata(IDEDRV drv, UINT size, UINT limit) {

	size = min(size, limit);
	drv->sc = IDEINTR_IO;
	drv->cy = size;
	drv->status &= ~(IDESTAT_BSY|IDESTAT_DMRD|IDESTAT_SERV|IDESTAT_CHK);
	drv->status |= IDESTAT_DRQ;
	drv->error = 0;
	ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_NO_SENSE);
	drv->asc = ATAPI_ASC_NO_ADDITIONAL_SENSE_INFORMATION;
	drv->bufdir = IDEDIR_IN;
	drv->buftc = IDETC_TRANSFEREND;
	drv->bufpos = 0;
	drv->bufsize = size;

	if (!(drv->ctrl & IDECTRL_NIEN)) {
		TRACEOUT(("atapicmd: senddata()"));
		ideio.bank[0] = ideio.bank[1] | 0x80;			// ????
		pic_setirq(IDE_IRQ);
	}
}

static void cmddone(IDEDRV drv) {

	drv->sc = IDEINTR_IO|IDEINTR_CD;
	drv->status &= ~(IDESTAT_BSY|IDESTAT_DRQ|IDESTAT_SERV|IDESTAT_CHK);
	drv->status |= IDESTAT_DRDY;
	drv->error = 0;
	ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_NO_SENSE);
	drv->asc = ATAPI_ASC_NO_ADDITIONAL_SENSE_INFORMATION;

	if (!(drv->ctrl & IDECTRL_NIEN)) {
		TRACEOUT(("atapicmd: cmddone()"));
		ideio.bank[0] = ideio.bank[1] | 0x80;			// ????
		pic_setirq(IDE_IRQ);
	}
}

static void senderror(IDEDRV drv) {

	drv->sc = IDEINTR_IO;
	drv->status &= ~(IDESTAT_BSY|IDESTAT_DMRD|IDESTAT_SERV);
	drv->status |= IDESTAT_CHK;

	if (!(drv->ctrl & IDECTRL_NIEN)) {
		TRACEOUT(("atapicmd: senderror()"));
		ideio.bank[0] = ideio.bank[1] | 0x80;			// ????
		pic_setirq(IDE_IRQ);
	}
}

static void sendabort(IDEDRV drv) {

	drv->sk = ATAPI_SK_ABORTED_COMMAND;
	drv->error = IDEERR_ABRT;
	senderror(drv);
}


// ----- ATAPI packet command

static void atapi_cmd_start_stop_unit(IDEDRV drv);
static void atapi_cmd_prevent_allow_medium_removal(IDEDRV drv);
static void atapi_cmd_read_capacity(IDEDRV drv);
static void atapi_cmd_read(IDEDRV drv, UINT32 lba, UINT32 leng);
static void atapi_cmd_mode_select(IDEDRV drv);
static void atapi_cmd_mode_sense(IDEDRV drv);
static void atapi_cmd_readsubch(IDEDRV drv);
static void atapi_cmd_readtoc(IDEDRV drv);

void atapicmd_a0(IDEDRV drv) {

	UINT32	lba, leng;
	UINT8	cmd;

	cmd = drv->buf[0];
	switch (cmd) {
	case 0x00:		// test unit ready
		TRACEOUT(("atapicmd: test unit ready"));
		if (!(drv->media & IDEIO_MEDIA_LOADED)) {
			/* medium not present */
			ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_NOT_READY);
			drv->asc = ATAPI_ASC_MEDIUM_NOT_PRESENT;
			senderror(drv);
			break;
		}
		if (drv->media & IDEIO_MEDIA_CHANGED) {
			drv->media &= ~IDEIO_MEDIA_CHANGED;
			ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_NOT_READY);
			drv->asc = ATAPI_ASC_NOT_READY_TO_READY_TRANSITION;
			senderror(drv);
			break;
		}

		cmddone(drv);
		break;

	case 0x03:		// request sense
		TRACEOUT(("atapicmd: request sense"));
		leng = drv->buf[4];
		ZeroMemory(drv->buf, 18);
		drv->buf[0] = 0x70;
		drv->buf[2] = drv->sk;
		drv->buf[7] = 11;	// length
		drv->buf[12] = (BYTE)(drv->asc & 0xff);
		senddata(drv, 18, leng);
		break;

	case 0x12:		// inquiry
		TRACEOUT(("atapicmd: inquiry"));
		leng = drv->buf[4];
		CopyMemory(drv->buf, cdrom_inquiry, sizeof(cdrom_inquiry));
		senddata(drv, sizeof(cdrom_inquiry), leng);
		break;

	case 0x1b:		// start stop unit
		TRACEOUT(("atapicmd: start stop unit"));
		atapi_cmd_start_stop_unit(drv);
		break;

	case 0x1e:		// prevent allow medium removal
		TRACEOUT(("atapicmd: prevent allow medium removal"));
		atapi_cmd_prevent_allow_medium_removal(drv);
		break;

	case 0x25:		// read capacity
		TRACEOUT(("atapicmd: read capacity"));
		atapi_cmd_read_capacity(drv);
		break;

	case 0x28:		// read(10)
		TRACEOUT(("atapicmd: read(10)"));
		lba = (drv->buf[2] << 24) + (drv->buf[3] << 16) + (drv->buf[4] << 8) + drv->buf[5];
		leng = (drv->buf[7] << 8) + drv->buf[8];
		atapi_cmd_read(drv, lba, leng);
		break;

	case 0x55:		// mode select
		TRACEOUT(("atapicmd: mode select"));
		atapi_cmd_mode_select(drv);
		break;

	case 0x5a:		// mode sense(10)
		TRACEOUT(("atapicmd: mode sense(10)"));
		atapi_cmd_mode_sense(drv);
		break;

	case 0x42:
		TRACEOUT(("atapicmd: read sub channel"));
		atapi_cmd_readsubch(drv);
		break;

	case 0x43:		// read TOC
		TRACEOUT(("atapicmd: read TOC"));
		atapi_cmd_readtoc(drv);
		break;

	default:
		TRACEOUT(("atapicmd: unknown command = %.2x", cmd));
		sendabort(drv);
		break;
	}
}


//-- command

// 0x1b: START/STOP UNIT
static void atapi_cmd_start_stop_unit(IDEDRV drv) {

	UINT	power;

	power = (drv->buf[4] >> 4);
	if (power != 0) {
		/* power control is not supported */
		ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_ILLEGAL_REQUEST);
		drv->asc = ATAPI_ASC_INVALID_FIELD_IN_CDB;
		goto send_error;
	}
	if (drv->buf[4] & 2) {
		/* lock/eject op. is not supported */
		ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_ILLEGAL_REQUEST);
		drv->asc = ATAPI_ASC_INVALID_FIELD_IN_CDB;
		goto send_error;
	}
	if (!(drv->media & IDEIO_MEDIA_LOADED)) {
		/* medium not present */
		ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_NOT_READY);
		drv->asc = ATAPI_ASC_MEDIUM_NOT_PRESENT;
		goto send_error;
	}

	/* XXX play/read TOC, stop */

	cmddone(drv);
	return;

send_error:
	senderror(drv);
}

// 0x1e: PREVENT/ALLOW MEDIUM REMOVAL
static void atapi_cmd_prevent_allow_medium_removal(IDEDRV drv) {

	/* XXX */
	cmddone(drv);
}

// 0x25: READ CAPACITY
static void atapi_cmd_read_capacity(IDEDRV drv) {

	/* XXX */
	cmddone(drv);
}

// 0x28: READ(10)
void atapi_dataread(IDEDRV drv) {

	// �G���[�����ڒ��ꒃ�`
	if (drv->nsectors == 0) {
		sendabort(drv);
		return;
	}
	if (sxsi_read(drv->sxsidrv, drv->sector, drv->buf, 2048) != 0) {
		ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_ILLEGAL_REQUEST);
		drv->asc = 0x21;
		senderror(drv);
		return;
	}
	drv->sector++;
	drv->nsectors--;

	drv->sc = IDEINTR_IO;
	drv->cy = 2048;
	drv->status &= ~(IDESTAT_BSY|IDESTAT_DMRD|IDESTAT_SERV|IDESTAT_CHK);
	drv->status |= IDESTAT_DRQ;
	drv->error = 0;
	ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_NO_SENSE);
	drv->asc = ATAPI_ASC_NO_ADDITIONAL_SENSE_INFORMATION;
	drv->bufdir = IDEDIR_IN;
	drv->buftc = (drv->nsectors)?IDETC_ATAPIREAD:IDETC_TRANSFEREND;
	drv->bufpos = 0;
	drv->bufsize = 2048;

	if (!(drv->ctrl & IDECTRL_NIEN)) {
		TRACEOUT(("atapicmd: senddata()"));
		ideio.bank[0] = ideio.bank[1] | 0x80;			// ????
		pic_setirq(IDE_IRQ);
	}
}

static void atapi_cmd_read(IDEDRV drv, UINT32 lba, UINT32 nsec) {

	drv->sector = lba;
	drv->nsectors = nsec;
	atapi_dataread(drv);
}

// -- MODE SELECT/SENSE
#define	PC_01_SIZE	8
#define	PC_0D_SIZE	8
#define	PC_0E_SIZE	16
#define	PC_2A_SIZE	20

// page code changeable value
static const BYTE chgval_pagecode_01[PC_01_SIZE] = {
	0x00, 0x00, 0x37, 0xff, 0x00, 0x00, 0x00, 0x00,
};
static const BYTE chgval_pagecode_0d[PC_0D_SIZE] = {
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff,
};
static const BYTE chgval_pagecode_0e[PC_0E_SIZE] = {
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xff, 0xff,
	0x0f, 0xff, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00,
};
static const BYTE chgval_pagecode_2a[PC_2A_SIZE] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0xc2, 0x00, 0x02, 0x00, 0x00, 0x02, 0xc2,
	0x00, 0x00, 0x00, 0x00,
};

// page code default value
static const BYTE defval_pagecode_01[PC_01_SIZE] = {
	0x01, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const BYTE defval_pagecode_0d[PC_0D_SIZE] = {
	0x0d, 0x06, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x4b,
};
static const BYTE defval_pagecode_0e[PC_0E_SIZE] = {
	0x0e, 0x0e, 0x04, 0x00, 0x00, 0x00, 0x00, 0x4b,
	0x01, 0xff, 0x02, 0xff, 0x00, 0x00, 0x00, 0x00,
};

static const BYTE defval_pagecode_2a[PC_2A_SIZE] = {
#ifdef YUIDEBUG
	0x2a, 0x12, 0x00, 0x00, 0x71, 0x65, 0x89, 0x07,
	0x02, 0xc2, 0x00, 0xff, 0x00, 0x80, 0x02, 0xc2,
	0x00, 0x00, 0x00, 0x00,
#else
	0x2a, 0x12, 0x00, 0x00, 0x00, 0x00, 0x20, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
#endif
};

// 0x55: MODE SELECT
static void atapi_cmd_mode_select(IDEDRV drv) {

	if (drv->buf[1] & 1) {
		/* Saved Page is not supported */
		ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_ILLEGAL_REQUEST);
		drv->asc = ATAPI_ASC_INVALID_FIELD_IN_CDB;
		senderror(drv);
		return;
	}

	sendabort(drv);	/* XXX */
}

// 0x5a: MODE SENSE
static void atapi_cmd_mode_sense(IDEDRV drv) {

	const BYTE	*ptr;
	UINT		leng;
	UINT		cnt;
	UINT8		pctrl, pcode;

	leng = (drv->buf[7] << 8) + drv->buf[8];
	pctrl = (drv->buf[2] >> 6) & 3;	// 0: current, 1: changeable, 2: default
	pcode = drv->buf[2] & 0x3f;

	if (pctrl == 3) {
		/* Saved Page is not supported */
		TRACEOUT(("Saved Page is not supported"));
		ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_ILLEGAL_REQUEST);
		drv->asc = ATAPI_ASC_SAVING_PARAMETERS_NOT_SUPPORTED;
		senderror(drv);
		return;
	}

	/* Mode Parameter Header */
	ZeroMemory(drv->buf, 8);
	if (!(drv->media & IDEIO_MEDIA_LOADED)) {
		drv->buf[2] = 0x70;	// Door closed, no disc present
	}
	else if ((drv->media & (IDEIO_MEDIA_COMBINE)) == IDEIO_MEDIA_AUDIO) {
		drv->buf[2] = 0x02;	// 120mm CD-ROM audio only
	}
	else if ((drv->media & (IDEIO_MEDIA_COMBINE)) == IDEIO_MEDIA_COMBINE) {
		drv->buf[2] = 0x03;	// 120mm CD-ROM data & audio combined
	}
	else {
		drv->buf[2] = 0x01;	// 120mm CD-ROM data only
	}
	cnt = 8;
	if (cnt > leng) {
		goto length_exceeded;
	}

	/* Mode Page */
	TRACEOUT(("pcode = %.2x", pcode));
	switch (pcode) {
	case 0x3f:
		/*FALLTHROUGH*/

	case 0x01:	/* Read Error Recovery Parameters Page */
		if (pctrl == 1) {
			ptr = chgval_pagecode_01;
		}
		else {
			ptr = defval_pagecode_01;
		}
		CopyMemory(drv->buf + cnt, ptr, min((leng - cnt), PC_01_SIZE));
		cnt += PC_01_SIZE;
		if (cnt > leng) {
			goto length_exceeded;
		}
		if (pcode == 0x01) {
			break;
		}
		/*FALLTHROUGH*/

	case 0x0d:	/* CD-ROM Device Parameters Page */
		if (pctrl == 1) {
			ptr = chgval_pagecode_0d;
		}
		else {
			ptr = defval_pagecode_0d;
		}
		CopyMemory(drv->buf + cnt, ptr, min((leng - cnt), PC_0D_SIZE));
		cnt += PC_0D_SIZE;
		if (cnt > leng) {
			goto length_exceeded;
		}
		if (pcode == 0x0d) {
			break;
		}
		/*FALLTHROUGH*/

	case 0x0e:	/* CD-ROM Audio Control Paramater Page */
		if (pctrl == 1) {
			ptr = chgval_pagecode_0e;
		}
		else {
			ptr = defval_pagecode_0e;
		}
		CopyMemory(drv->buf + cnt, ptr, min((leng - cnt), PC_0E_SIZE));
		cnt += PC_0E_SIZE;
		if (cnt > leng) {
			goto length_exceeded;
		}
		if (pcode == 0x0e) {
			break;
		}
		/*FALLTHROUGH*/

	case 0x2a:	/* CD-ROM Capabilities & Mechanical Status Page */
		if (pctrl == 1) {
			ptr = chgval_pagecode_2a;
		}
		else {
			ptr = defval_pagecode_2a;
		}
		CopyMemory(drv->buf + cnt, ptr, min((leng - cnt), PC_2A_SIZE));
		cnt += PC_2A_SIZE;
		if (cnt > leng) {
			goto length_exceeded;
		}
#if 0
		/*FALLTHROUGH*/

	case 0x00:
#endif
		break;

	default:
		ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_ILLEGAL_REQUEST);
		drv->asc = ATAPI_ASC_INVALID_FIELD_IN_CDB;
		senderror(drv);
		return;
	}

	drv->buf[0] = (UINT8)((cnt - 2) >> 8);
	drv->buf[1] = (UINT8)(cnt - 2);
	senddata(drv, cnt, leng);
	return;

length_exceeded:
	if (cnt >= 65536) {
		ATAPI_SET_SENSE_KEY(drv, ATAPI_SK_ILLEGAL_REQUEST);
		drv->asc = ATAPI_ASC_INVALID_FIELD_IN_CDB;
		senderror(drv);
		return;
	}

	drv->buf[0] = (UINT8)((leng - 2) >> 8);
	drv->buf[1] = (UINT8)(leng - 2);
	senddata(drv, cnt, leng);
}


static void atapi_cmd_readsubch(IDEDRV drv) {

	SXSIDEV	sxsi;
	UINT	leng;

	sxsi = sxsi_getptr(drv->sxsidrv);
	if ((sxsi == NULL) || (sxsi->devtype != SXSIDEV_CDROM) ||
		(!(sxsi->flag & SXSIFLAG_READY))) {
		senderror(drv);
		return;
	}
	leng = (drv->buf[7] << 8) + drv->buf[8];
	switch(drv->buf[3]) {
		case 0x01:			// CD-ROM current pos
			ZeroMemory(drv->buf, 16);
			drv->buf[4] = 0x01;
			senddata(drv, 16, leng);
			break;

		default:
			senderror(drv);
			break;
	}
}

static void atapi_cmd_readtoc(IDEDRV drv) {

	SXSIDEV	sxsi;
	UINT	leng;
	UINT	format;
	UINT	datasize;

	sxsi = sxsi_getptr(drv->sxsidrv);
	if ((sxsi == NULL) || (sxsi->devtype != SXSIDEV_CDROM) ||
		(!(sxsi->flag & SXSIFLAG_READY))) {
		senderror(drv);
		return;
	}

	leng = (drv->buf[7] << 8) + drv->buf[8];
	format = (drv->buf[9] >> 6);
	TRACEOUT(("atapi_cmd_readtoc fmt=%d leng=%d", format, leng));

	switch (format) {
	case 0: // track info
		datasize = sxsicd_gettocinfo(sxsi, drv->buf + 2);
		drv->buf[0] = (UINT8)(datasize >> 8);
		drv->buf[1] = (UINT8)(datasize >> 0);
		senddata(drv, datasize + 2, leng);
		break;

	case 1:	// multi session
		ZeroMemory(drv->buf, 12);
		drv->buf[1] = 0x0a;
		drv->buf[2] = 0x01;
		drv->buf[3] = 0x01;
		drv->buf[5] = 0x14;
		drv->buf[6] = 0x01;
		drv->buf[10] = 0x02;
		senddata(drv, 12, leng);
		break;

	default:
		senderror(drv);
		break;
	}
}

#endif	/* SUPPORT_IDEIO */
