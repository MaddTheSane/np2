//
// FDC μPD765A
//


#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fdd/fddfile.h"

enum {
	FDC_DMACH2HD	= 2,
	FDC_DMACH2DD	= 3
};

static const UINT8 FDCCMD_TABLE[32] = {
						0, 0, 8, 2, 1, 8, 8, 1, 0, 8, 1, 0, 8, 5, 0, 2,
						0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 8, 0, 0};


#define FDC_FORCEREADY (1)
#define	FDC_DELAYERROR7


void fdc_intwait(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		g_fdc.intreq = TRUE;
		if (g_fdc.chgreg & 1) {
			pic_setirq(0x0b);
		}
		else {
			pic_setirq(0x0a);
		}
	}
}

void fdc_interrupt(void) {

	nevent_set(NEVENT_FDCINT, 512, fdc_intwait, NEVENT_ABSOLUTE);
}

static void fdc_interruptreset(void) {

	g_fdc.intreq = FALSE;
}

static BOOL fdc_isfdcinterrupt(void) {

	return(g_fdc.intreq);
}

REG8 DMACCALL fdc_dmafunc(REG8 func) {

//	TRACEOUT(("fdc_dmafunc = %d", func));
	switch(func) {
		case DMAEXT_START:
			return(1);

		case DMAEXT_END:				// TC
			g_fdc.tc = 1;
			break;
	}
	return(0);
}

static void fdc_dmaready(REG8 enable) {

	if (g_fdc.chgreg & 1) {
		dmac.dmach[FDC_DMACH2HD].ready = enable;
	}
	else {
		dmac.dmach[FDC_DMACH2DD].ready = enable;
	}
}


// ----------------------------------------------------------------------

void fdcsend_error7(void) {

	g_fdc.tc = 0;
	g_fdc.event = FDCEVENT_BUFSEND;
	g_fdc.bufp = 0;
	g_fdc.bufcnt = 7;
	g_fdc.buf[0] = (UINT8)(g_fdc.stat[g_fdc.us] >>  0);
	g_fdc.buf[1] = (UINT8)(g_fdc.stat[g_fdc.us] >>  8);
	g_fdc.buf[2] = (UINT8)(g_fdc.stat[g_fdc.us] >> 16);
	g_fdc.buf[3] = g_fdc.C;
	g_fdc.buf[4] = g_fdc.H;
	g_fdc.buf[5] = g_fdc.R;
	g_fdc.buf[6] = g_fdc.N;
	g_fdc.status = FDCSTAT_RQM | FDCSTAT_CB | FDCSTAT_DIO;
	g_fdc.stat[g_fdc.us] = 0;										// ver0.29
	fdc_dmaready(0);
	dmac_check();
	fdc_interrupt();
}

void fdcsend_success7(void) {

	g_fdc.tc = 0;
	g_fdc.event = FDCEVENT_BUFSEND;
	g_fdc.bufp = 0;
	g_fdc.bufcnt = 7;
	g_fdc.buf[0] = (g_fdc.hd << 2) | g_fdc.us;
	g_fdc.buf[1] = 0;
	g_fdc.buf[2] = 0;
	g_fdc.buf[3] = g_fdc.C;
	g_fdc.buf[4] = g_fdc.H;
	g_fdc.buf[5] = g_fdc.R;
	g_fdc.buf[6] = g_fdc.N;
	g_fdc.status = FDCSTAT_RQM | FDCSTAT_CB | FDCSTAT_DIO;
	g_fdc.stat[g_fdc.us] = 0;										// ver0.29
	fdc_dmaready(0);
	dmac_check();
	fdc_interrupt();
}

#if 0
// FDCのタイムアウト			まぁ本当はこんなんじゃダメだけど…	ver0.29
void fdctimeoutproc(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		g_fdc.stat[g_fdc.us] = FDCRLT_IC0 | FDCRLT_EN | (g_fdc.hd << 2) | g_fdc.us;
		fdcsend_error7();
	}
}

static void fdc_timeoutset(void) {

	nevent_setbyms(NEVENT_FDCTIMEOUT, 166, fdctimeoutproc, NEVENT_ABSOLUTE);
}
#endif



static BOOL FDC_DriveCheck(BOOL protectcheck) {

	if (!fddfile[g_fdc.us].fname[0]) {
		g_fdc.stat[g_fdc.us] = FDCRLT_IC0 | FDCRLT_NR | (g_fdc.hd << 2) | g_fdc.us;
		fdcsend_error7();
		return(FALSE);
	}
	else if ((protectcheck) && (fddfile[g_fdc.us].protect)) {
		g_fdc.stat[g_fdc.us] = FDCRLT_IC0 | FDCRLT_NW | (g_fdc.hd << 2) | g_fdc.us;
		fdcsend_error7();
		return(FALSE);
	}
	return(TRUE);
}

// ----------------------------------------------------------------------

static void get_mtmfsk(void) {

	g_fdc.mt = (g_fdc.cmd >> 7) & 1;
	g_fdc.mf = g_fdc.cmd & 0x40;							// ver0.29
	g_fdc.sk = (g_fdc.cmd >> 5) & 1;
}

static void get_hdus(void) {

	g_fdc.hd = (g_fdc.cmds[0] >> 2) & 1;
	g_fdc.us = g_fdc.cmds[0] & 3;
}

static void get_chrn(void) {

	g_fdc.C = g_fdc.cmds[1];
	g_fdc.H = g_fdc.cmds[2];
	g_fdc.R = g_fdc.cmds[3];
	g_fdc.N = g_fdc.cmds[4];
}

static void get_eotgsldtl(void) {

	g_fdc.eot = g_fdc.cmds[5];
	g_fdc.gpl = g_fdc.cmds[6];
	g_fdc.dtl = g_fdc.cmds[7];
}

// --------------------------------------------------------------------------

static void FDC_Invalid(void) {							// cmd: xx

	g_fdc.event = FDCEVENT_BUFSEND;
	g_fdc.bufcnt = 1;
	g_fdc.bufp = 0;
	g_fdc.buf[0] = 0x80;
	g_fdc.status = FDCSTAT_RQM | FDCSTAT_CB | FDCSTAT_DIO;
}

#if 0
static void FDC_ReadDiagnostic(void) {					// cmd: 02

	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
			get_hdus();
			get_chrn();
			get_eotgsldtl();
			g_fdc.stat[g_fdc.us] = (g_fdc.hd << 2) | g_fdc.us;

			if (FDC_DriveCheck(FALSE)) {
				g_fdc.event = FDCEVENT_BUFSEND;
//				g_fdc.bufcnt = makedianosedata();
				g_fdc.bufp = 0;
			}
			break;

		default:
			g_fdc.event = FDCEVENT_NEUTRAL;
			break;
	}
}
#endif

static void FDC_Specify(void) {							// cmd: 03

	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
			g_fdc.srt = g_fdc.cmds[0] >> 4;
			g_fdc.hut = g_fdc.cmds[0] & 0x0f;
			g_fdc.hlt = g_fdc.cmds[1] >> 1;
			g_fdc.nd = g_fdc.cmds[1] & 1;
			g_fdc.stat[g_fdc.us] = (g_fdc.hd << 2) | g_fdc.us;
			break;
	}
	g_fdc.event = FDCEVENT_NEUTRAL;
	g_fdc.status = FDCSTAT_RQM;
}

static void FDC_SenseDeviceStatus(void) {				// cmd: 04

	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
			get_hdus();
			g_fdc.buf[0] = (g_fdc.hd << 2) | g_fdc.us;
			g_fdc.stat[g_fdc.us] = (g_fdc.hd << 2) | g_fdc.us;
			if (g_fdc.equip & (1 << g_fdc.us)) {
				g_fdc.buf[0] |= 0x08;
				if (!g_fdc.treg[g_fdc.us]) {
					g_fdc.buf[0] |= 0x10;
				}
				if (fddfile[g_fdc.us].fname[0]) {
					g_fdc.buf[0] |= 0x20;
				}
				if (fddfile[g_fdc.us].protect) {
					g_fdc.buf[0] |= 0x40;
				}
			}
			else {
				g_fdc.buf[0] |= 0x80;
			}
//			TRACEOUT(("FDC_SenseDeviceStatus %.2x", g_fdc.buf[0]));
			g_fdc.event = FDCEVENT_BUFSEND;
			g_fdc.bufcnt = 1;
			g_fdc.bufp = 0;
			g_fdc.status = FDCSTAT_RQM | FDCSTAT_CB | FDCSTAT_DIO;
			break;

		default:
			g_fdc.event = FDCEVENT_NEUTRAL;
			g_fdc.status = FDCSTAT_RQM;
			break;
	}
}

static BRESULT writesector(void) {

	g_fdc.stat[g_fdc.us] = (g_fdc.hd << 2) | g_fdc.us;
	if (!FDC_DriveCheck(TRUE)) {
		return(FAILURE);
	}
	if (fdd_write()) {
		g_fdc.stat[g_fdc.us] = g_fdc.us | (g_fdc.hd << 2) | FDCRLT_IC0 | FDCRLT_ND;
		fdcsend_error7();
		return(FAILURE);
	}
	g_fdc.event = FDCEVENT_BUFRECV;
	g_fdc.bufcnt = 128 << g_fdc.N;
	g_fdc.bufp = 0;
	g_fdc.status = FDCSTAT_RQM | FDCSTAT_NDM | FDCSTAT_CB;
	fdc_dmaready(1);
	dmac_check();
	return(SUCCESS);
}

static void FDC_WriteData(void) {						// cmd: 05
														// cmd: 09
	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
			get_hdus();
			get_chrn();
			get_eotgsldtl();
			g_fdc.stat[g_fdc.us] = (g_fdc.hd << 2) | g_fdc.us;
			if (FDC_DriveCheck(TRUE)) {
				g_fdc.event = FDCEVENT_BUFRECV;
				g_fdc.bufcnt = 128 << g_fdc.N;
				g_fdc.bufp = 0;
#if 1															// ver0.27 ??
				g_fdc.status = FDCSTAT_NDM | FDCSTAT_CB;
				if (!(g_fdc.ctrlreg & 0x10)) {
					g_fdc.status |= FDCSTAT_RQM;
				}
#else
				g_fdc.status = FDCSTAT_RQM | FDCSTAT_NDM | FDCSTAT_CB;
#endif
				fdc_dmaready(1);
				dmac_check();
			}
			break;

		case FDCEVENT_BUFRECV:
			if (writesector()) {
				return;
			}
			if (g_fdc.tc) {
				fdcsend_success7();
				return;
			}
			if (g_fdc.R++ == g_fdc.eot) {
				g_fdc.stat[g_fdc.us] = g_fdc.us | (g_fdc.hd << 2) |
													FDCRLT_IC0 | FDCRLT_EN;
				fdcsend_error7();
				break;
			}
			break;

		default:
			g_fdc.event = FDCEVENT_NEUTRAL;
			g_fdc.status = FDCSTAT_RQM;
			break;
	}
}

static void readsector(void) {

	g_fdc.stat[g_fdc.us] = (g_fdc.hd << 2) | g_fdc.us;
	if (!FDC_DriveCheck(FALSE)) {
		return;
	}
	if (fdd_read()) {
		g_fdc.stat[g_fdc.us] = g_fdc.us | (g_fdc.hd << 2) | FDCRLT_IC0 | FDCRLT_ND;
		fdcsend_error7();
		return;
	}

	g_fdc.event = FDCEVENT_BUFSEND2;
	g_fdc.bufp = 0;
#if 1															// ver0.27 ??
	g_fdc.status = FDCSTAT_NDM | FDCSTAT_CB;
	if (!(g_fdc.ctrlreg & 0x10)) {
		g_fdc.status |= FDCSTAT_RQM | FDCSTAT_DIO;
	}
#else
	g_fdc.status = FDCSTAT_RQM | FDCSTAT_DIO | FDCSTAT_NDM | FDCSTAT_CB;
#endif
	fdc_dmaready(1);
	dmac_check();
}

static void FDC_ReadData(void) {						// cmd: 06
														// cmd: 0c
	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
			get_hdus();
			get_chrn();
			get_eotgsldtl();
			readsector();
			break;

		case FDCEVENT_NEXTDATA:
			g_fdc.bufcnt = 0;
			if (g_fdc.R++ == g_fdc.eot) {
				g_fdc.stat[g_fdc.us] = g_fdc.us | (g_fdc.hd << 2) |
													FDCRLT_IC0 | FDCRLT_EN;
				fdcsend_error7();
				break;
			}
			readsector();
			break;

#ifdef FDC_DELAYERROR7
		case FDCEVENT_BUSY:
			break;
#endif

		default:
			g_fdc.event = FDCEVENT_NEUTRAL;
			g_fdc.status = FDCSTAT_RQM;
			break;
	}
}

static void FDC_Recalibrate(void) {						// cmd: 07

	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
			get_hdus();
			g_fdc.ncn = 0;
			g_fdc.stat[g_fdc.us] = (g_fdc.hd << 2) | g_fdc.us;
			g_fdc.stat[g_fdc.us] |= FDCRLT_SE;
			if (!(g_fdc.equip & (1 << g_fdc.us))) {
				g_fdc.stat[g_fdc.us] |= FDCRLT_NR | FDCRLT_IC0;
			}
			else if (!fddfile[g_fdc.us].fname[0]) {
				g_fdc.stat[g_fdc.us] |= FDCRLT_NR;
			}
			else {
				fdd_seek();
			}
			fdc_interrupt();
			break;
	}
	g_fdc.event = FDCEVENT_NEUTRAL;
	g_fdc.status = FDCSTAT_RQM;
}

static void FDC_SenceintStatus(void) {					// cmd: 08

	int		i;

	g_fdc.event = FDCEVENT_BUFSEND;
	g_fdc.bufp = 0;
	g_fdc.bufcnt = 0;
	g_fdc.status = FDCSTAT_RQM | FDCSTAT_CB | FDCSTAT_DIO;

	if (fdc_isfdcinterrupt()) {
		i = 0;
		if (g_fdc.stat[g_fdc.us]) {
			g_fdc.buf[0] = (UINT8)g_fdc.stat[g_fdc.us];
			g_fdc.buf[1] = g_fdc.treg[g_fdc.us];
			g_fdc.bufcnt = 2;
			g_fdc.stat[g_fdc.us] = 0;
//			TRACEOUT(("fdc stat - %d [%.2x]", g_fdc.us, g_fdc.buf[0]));
		}
		else {
			for (; i<4; i++) {
				if (g_fdc.stat[i]) {
					g_fdc.buf[0] = (UINT8)g_fdc.stat[i];
					g_fdc.buf[1] = g_fdc.treg[i];
					g_fdc.bufcnt = 2;
					g_fdc.stat[i] = 0;
//					TRACEOUT(("fdc stat - %d [%.2x]", i, g_fdc.buf[0]));
					break;
				}
			}
		}
		for (; i<4; i++) {
			if (g_fdc.stat[i]) {
				break;
			}
		}
		if (i >= 4) {
			fdc_interruptreset();
		}
	}
	if (!g_fdc.bufcnt) {
		g_fdc.buf[0] = FDCRLT_IC1;
		g_fdc.bufcnt = 1;
	}
}

static void FDC_ReadID(void) {							// cmd: 0a

	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
			g_fdc.mf = g_fdc.cmd & 0x40;
			get_hdus();
			if (fdd_readid() == SUCCESS) {
				fdcsend_success7();
			}
			else {
				g_fdc.stat[g_fdc.us] = g_fdc.us | (g_fdc.hd << 2) |
													FDCRLT_IC0 | FDCRLT_MA;
				fdcsend_error7();
			}
			break;
	}
}

static void FDC_WriteID(void) {							// cmd: 0d

	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
//			TRACE_("FDC_WriteID FDCEVENT_CMDRECV", 0);
			get_hdus();
			g_fdc.N = g_fdc.cmds[1];
			g_fdc.sc = g_fdc.cmds[2];
			g_fdc.gpl = g_fdc.cmds[3];
			g_fdc.d = g_fdc.cmds[4];
			if (FDC_DriveCheck(TRUE)) {
//				TRACE_("FDC_WriteID FDC_DriveCheck", 0);
				if (fdd_formatinit()) {
//					TRACE_("FDC_WriteID fdd_formatinit", 0);
					fdcsend_error7();
					break;
				}
//				TRACE_("FDC_WriteID FDCEVENT_BUFRECV", 0);
				g_fdc.event = FDCEVENT_BUFRECV;
				g_fdc.bufcnt = 4;
				g_fdc.bufp = 0;
#if 1															// ver0.27 ??
				g_fdc.status = FDCSTAT_NDM | FDCSTAT_CB;
				if (!(g_fdc.ctrlreg & 0x10)) {
					g_fdc.status |= FDCSTAT_RQM;
				}
#else
				g_fdc.status = FDCSTAT_RQM | FDCSTAT_NDM | FDCSTAT_CB;
#endif
				fdc_dmaready(1);
				dmac_check();
			}
			break;

		case FDCEVENT_BUFRECV:
			if (fdd_formating(g_fdc.buf)) {
				fdcsend_error7();
				break;
			}
			if ((g_fdc.tc) || (!fdd_isformating())) {
				fdcsend_success7();
				return;
			}
			g_fdc.event = FDCEVENT_BUFRECV;
			g_fdc.bufcnt = 4;
			g_fdc.bufp = 0;
#if 1															// ver0.27 ??
			g_fdc.status = FDCSTAT_NDM | FDCSTAT_CB;
			if (!(g_fdc.ctrlreg & 0x10)) {
				g_fdc.status |= FDCSTAT_RQM;
			}
#else
			g_fdc.status = FDCSTAT_RQM | FDCSTAT_NDM | FDCSTAT_CB;
#endif
			break;

		default:
			g_fdc.event = FDCEVENT_NEUTRAL;
			g_fdc.status = FDCSTAT_RQM;
			break;
	}
}

static void FDC_Seek(void) {							// cmd: 0f

	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
			get_hdus();
			g_fdc.ncn = g_fdc.cmds[1];
			g_fdc.stat[g_fdc.us] = (g_fdc.hd << 2) | g_fdc.us;
			g_fdc.stat[g_fdc.us] |= FDCRLT_SE;
			if ((!(g_fdc.equip & (1 << g_fdc.us))) ||
				(!fddfile[g_fdc.us].fname[0])) {
				g_fdc.stat[g_fdc.us] |= FDCRLT_NR | FDCRLT_IC0;
			}
			else {
				fdd_seek();
			}
			fdc_interrupt();
			break;
	}
	g_fdc.event = FDCEVENT_NEUTRAL;
	g_fdc.status = FDCSTAT_RQM;
}

#if 0
static void FDC_ScanEqual(void) {						// cmd: 11, 19, 1d

	switch(g_fdc.event) {
		case FDCEVENT_CMDRECV:
			get_hdus();
			get_chrn();
			g_fdc.eot = g_fdc.cmds[5];
			g_fdc.gpl = g_fdc.cmds[6];
			g_fdc.stp = g_fdc.cmds[7];
			break;
	}
}
#endif

// --------------------------------------------------------------------------

typedef void (*FDCOPE)(void);

static const FDCOPE FDC_Ope[0x20] = {
				FDC_Invalid,
				FDC_Invalid,
				FDC_ReadData,			// FDC_ReadDiagnostic,
				FDC_Specify,
				FDC_SenseDeviceStatus,
				FDC_WriteData,
				FDC_ReadData,
				FDC_Recalibrate,
				FDC_SenceintStatus,
				FDC_WriteData,
				FDC_ReadID,
				FDC_Invalid,
				FDC_ReadData,
				FDC_WriteID,
				FDC_Invalid,
				FDC_Seek,
				FDC_Invalid,					// 10
				FDC_Invalid,			// FDC_ScanEqual,
				FDC_Invalid,
				FDC_Invalid,
				FDC_Invalid,
				FDC_Invalid,
				FDC_Invalid,
				FDC_Invalid,
				FDC_Invalid,
				FDC_Invalid,			// FDC_ScanEqual,
				FDC_Invalid,
				FDC_Invalid,
				FDC_Invalid,
				FDC_Invalid,			// FDC_ScanEqual,
				FDC_Invalid,
				FDC_Invalid};


// --------------------------------------------------------------------------

static void fdcstatusreset(void) {

	g_fdc.event = FDCEVENT_NEUTRAL;
	g_fdc.status = FDCSTAT_RQM;
}

void DMACCALL fdc_datawrite(REG8 data) {

//	if ((g_fdc.status & (FDCSTAT_RQM | FDCSTAT_DIO)) == FDCSTAT_RQM) {
		switch(g_fdc.event) {
			case FDCEVENT_BUFRECV:
//				TRACE_("write", g_fdc.bufp);
				g_fdc.buf[g_fdc.bufp++] = data;
				if ((!(--g_fdc.bufcnt)) || (g_fdc.tc)) {
					g_fdc.status &= ~FDCSTAT_RQM;
					FDC_Ope[g_fdc.cmd & 0x1f]();
				}
				break;

			case FDCEVENT_CMDRECV:
				g_fdc.cmds[g_fdc.cmdp++] = data;
				if (!(--g_fdc.cmdcnt)) {
					g_fdc.status &= ~FDCSTAT_RQM;
					FDC_Ope[g_fdc.cmd & 0x1f]();
				}
				break;

			default:
				g_fdc.cmd = data;
				get_mtmfsk();
				if (FDCCMD_TABLE[data & 0x1f]) {
					g_fdc.event = FDCEVENT_CMDRECV;
					g_fdc.cmdp = 0;
					g_fdc.cmdcnt = FDCCMD_TABLE[data & 0x1f];
					g_fdc.status = FDCSTAT_RQM | FDCSTAT_CB;
				}
				else {
					g_fdc.status &= ~FDCSTAT_RQM;
					FDC_Ope[g_fdc.cmd & 0x1f]();
				}
				break;
		}
//	}
}

REG8 DMACCALL fdc_dataread(void) {

//	if ((g_fdc.status & (FDCSTAT_RQM | FDCSTAT_DIO))
//									== (FDCSTAT_RQM | FDCSTAT_DIO)) {
		switch(g_fdc.event) {
			case FDCEVENT_BUFSEND:
				g_fdc.lastdata = g_fdc.buf[g_fdc.bufp++];
				if (!(--g_fdc.bufcnt)) {
					g_fdc.event = FDCEVENT_NEUTRAL;
					g_fdc.status = FDCSTAT_RQM;
				}
				break;

			case FDCEVENT_BUFSEND2:
				if (g_fdc.bufcnt) {
					g_fdc.lastdata = g_fdc.buf[g_fdc.bufp++];
					g_fdc.bufcnt--;
				}
				if (g_fdc.tc) {
					if (!g_fdc.bufcnt) {						// ver0.26
						g_fdc.R++;
						if ((g_fdc.cmd & 0x80) && fdd_seeksector()) {
							g_fdc.C += g_fdc.hd;
							g_fdc.H = g_fdc.hd ^ 1;
							g_fdc.R = 1;
						}
					}
					fdcsend_success7();
				}
				if (!g_fdc.bufcnt) {
					g_fdc.event = FDCEVENT_NEXTDATA;
					g_fdc.status &= ~(FDCSTAT_RQM | FDCSTAT_NDM);
					FDC_Ope[g_fdc.cmd & 0x1f]();
				}
				break;
		}
//	}
	return(g_fdc.lastdata);
}


// ---- I/O

static void IOOUTCALL fdc_o92(UINT port, REG8 dat) {

//	TRACEOUT(("fdc out %.2x %.2x [%.4x:%.4x]", port, dat, CPU_CS, CPU_IP));

	if (((port >> 4) ^ g_fdc.chgreg) & 1) {
		return;
	}
	if ((g_fdc.status & (FDCSTAT_RQM | FDCSTAT_DIO)) == FDCSTAT_RQM) {
		fdc_datawrite(dat);
	}
}

static void IOOUTCALL fdc_o94(UINT port, REG8 dat) {

//	TRACEOUT(("fdc out %.2x %.2x [%.4x:%.4x]", port, dat, CPU_CS, CPU_IP));

	if (((port >> 4) ^ g_fdc.chgreg) & 1) {
		return;
	}
	if ((g_fdc.ctrlreg ^ dat) & 0x10) {
		fdcstatusreset();
		fdc_dmaready(0);
		dmac_check();
	}
	g_fdc.ctrlreg = dat;
}

static REG8 IOINPCALL fdc_i90(UINT port) {

//	TRACEOUT(("fdc in %.2x %.2x [%.4x:%.4x]", port, g_fdc.status,
//															CPU_CS, CPU_IP));

	if (((port >> 4) ^ g_fdc.chgreg) & 1) {
		return(0xff);
	}
	return(g_fdc.status);
}

static REG8 IOINPCALL fdc_i92(UINT port) {

	REG8	ret;

	if (((port >> 4) ^ g_fdc.chgreg) & 1) {
		return(0xff);
	}
	if ((g_fdc.status & (FDCSTAT_RQM | FDCSTAT_DIO))
										== (FDCSTAT_RQM | FDCSTAT_DIO)) {
		ret = fdc_dataread();
	}
	else {
		ret = g_fdc.lastdata;
	}
//	TRACEOUT(("fdc in %.2x %.2x [%.4x:%.4x]", port, ret, CPU_CS, CPU_IP));
	return(ret);
}

static REG8 IOINPCALL fdc_i94(UINT port) {

	if (((port >> 4) ^ g_fdc.chgreg) & 1) {
		return(0xff);
	}
	if (port & 0x10) {		// 94
		return(0x40);
	}
	else {					// CC
		return(0x70);		// readyを立てるるる
	}
}


static void IOOUTCALL fdc_obe(UINT port, REG8 dat) {

	g_fdc.chgreg = dat;
	if (g_fdc.chgreg & 2) {
		CTRL_FDMEDIA = DISKTYPE_2HD;
	}
	else {
		CTRL_FDMEDIA = DISKTYPE_2DD;
	}
	(void)port;
}

static REG8 IOINPCALL fdc_ibe(UINT port) {

	(void)port;
	return((g_fdc.chgreg & 3) | 8);
}

static void IOOUTCALL fdc_o4be(UINT port, REG8 dat) {

	g_fdc.reg144 = dat;
	if (dat & 0x10) {
		g_fdc.rpm[(dat >> 5) & 3] = dat & 1;
	}
	(void)port;
}

static REG8 IOINPCALL fdc_i4be(UINT port) {

	(void)port;
	return(g_fdc.rpm[(g_fdc.reg144 >> 5) & 3] | 0xf0);
}


// ---- I/F

static const IOOUT fdco90[4] = {
					NULL,		fdc_o92,	fdc_o94,	NULL};
static const IOINP fdci90[4] = {
					fdc_i90,	fdc_i92,	fdc_i94,	NULL};
static const IOOUT fdcobe[1] = {fdc_obe};
static const IOINP fdcibe[1] = {fdc_ibe};

void fdc_reset(const NP2CFG *pConfig) {

	memset(&g_fdc, 0, sizeof(g_fdc));
	g_fdc.equip = pConfig->fddequip;
#if defined(SUPPORT_PC9821)
	g_fdc.support144 = 1;
#else
	g_fdc.support144 = pConfig->usefd144;
#endif
	fdcstatusreset();
	dmac_attach(DMADEV_2HD, FDC_DMACH2HD);
	dmac_attach(DMADEV_2DD, FDC_DMACH2DD);
	CTRL_FDMEDIA = DISKTYPE_2HD;
	g_fdc.chgreg = 3;
}

void fdc_bind(void) {

	iocore_attachcmnoutex(0x0090, 0x00f9, fdco90, 4);
	iocore_attachcmninpex(0x0090, 0x00f9, fdci90, 4);
	iocore_attachcmnoutex(0x00c8, 0x00f9, fdco90, 4);
	iocore_attachcmninpex(0x00c8, 0x00f9, fdci90, 4);

	if (g_fdc.support144) {
		iocore_attachout(0x04be, fdc_o4be);
		iocore_attachinp(0x04be, fdc_i4be);
	}
	iocore_attachsysoutex(0x00be, 0x0cff, fdcobe, 1);
	iocore_attachsysinpex(0x00be, 0x0cff, fdcibe, 1);
}

