#include	"compiler.h"
#include	"commng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"mpu98ii.h"


enum {
	MIDI_STOP			= 0xfc,

	MIDIIN_AVAIL		= 0x80,
	MIDIOUT_BUSY		= 0x40,

	MPU_INT				= 0xfd,
	MPU_ACK         	= 0xfe,

	MIDITIMEOUTCLOCK	= 3000,
	MIDITIMEOUTCLOCK2	= 300
};

enum {
	MIDIE_STEP		= 0x01,
	MIDIE_2NDPARA	= 0x02,
	MIDIE_EVENT		= 0x04,
	MIDIE_DATA		= 0x08,
	MIDIE_F9DATA	= 0x10,
	MIDIE_F9CMD		= 0x20,
	MIDIE_F9PARA	= 0x40
};

enum {
	MPU1FLAG_A		= 0x01,
	MPU1FLAG_B		= 0x02,
	MPU1FLAG_F9		= 0x04
};


	_MPU98II	mpu98;
	COMMNG		cm_mpu98;


static const UINT8 mpuirqnum[4] = {3, 5, 6, 12};

static const UINT8 fd_step1[4][4] = {{0, 0, 0, 0}, {1, 0, 0, 0},
									{1, 0, 1, 0}, {1, 1, 1, 0}};


static void makeintclock(void) {

	UINT32	l;

	l = mpu98.tempo * 2 * mpu98.tempos / 0x40;
	if (l < 5*2) {
		l = 5*2;
	}
	l *= mpu98.timebase;								//	*12
	mpu98.stepclock = (pccore.realclock * 5 / l);		//	/12
}

static void sendallclocks(REG8 data) {

	REG8	quarter;
	int		i;

	quarter = data >> 2;
	if (!quarter) {
		quarter = 64;
	}
	for (i=0; i<4; i++) {
		mpu98.fd_step[i] = quarter + fd_step1[data & 3][i];
	}
	mpu98.fd_remain = 0;
}

static void setrecvdata(REG8 data) {

	MPURECV	*r;

	r = &mpu98.r;
	if (r->cnt < MPU98_RECVBUFS) {
		r->buf[(r->pos + r->cnt) & (MPU98_RECVBUFS - 1)] = data;
		r->cnt++;
	}
}

static void mpu98ii_int(void) {

	pic_setirq(mpu98.irqnum);
}

static void ch_step(void) {

	int		i;
	REG8	bit;

	if (mpu98.flag1 & MPU1FLAG_F9) {
		if (mpu98.f9.step) {
			mpu98.f9.step--;
		}
	}
	for (i=0, bit=1; i<8; bit<<=1, i++) {
		if (mpu98.intch & bit) {
			if (mpu98.ch[i].step) {
				mpu98.ch[i].step--;
			}
		}
	}
}

static BOOL ch_nextsearch(void) {

	int		i;
	REG8	bit;

ch_nextsearch_more:
	if (mpu98.intreq == 9) {
		if (mpu98.flag1 & MPU1FLAG_F9) {
			if (!mpu98.f9.step) {
				setrecvdata(0xf9);
				mpu98ii_int();
				mpu98.f9.datas = 0;
				mpu98.f9.remain = MPU98_EXCVBUFS;
				mpu98.recvevent |= MIDIE_F9DATA;
				return(TRUE);
			}
		}
		mpu98.intreq = 7;
	}
	bit = 1 << mpu98.intreq;
	for (; bit; bit>>=1) {
		if (mpu98.intch & bit) {
			MPUCH *ch;
			ch = mpu98.ch + mpu98.intreq;
			if (!ch->step) {
				if ((ch->datas) && (ch->remain == 0)) {
					if (cm_mpu98 == NULL) {
						cm_mpu98 = commng_create(COMCREATE_MPU98II);
					}
					if (ch->data[0] == MIDI_STOP) {
						ch->datas = 0;
						cm_mpu98->write(cm_mpu98, MIDI_STOP);
						setrecvdata(MIDI_STOP);
						mpu98ii_int();
						return(TRUE);
					}
					for (i=0; i<ch->datas; i++) {
						cm_mpu98->write(cm_mpu98, ch->data[i]);
					}
					ch->datas = 0;
				}
				setrecvdata((REG8)(0xf0 + mpu98.intreq));
				mpu98ii_int();
				mpu98.recvevent |= MIDIE_STEP;
				return(TRUE);
			}
		}
		mpu98.intreq--;
	}
	mpu98.remainstep--;
	if (mpu98.remainstep) {
		ch_step();
		mpu98.intreq = 9;
		goto ch_nextsearch_more;
	}
	return(FALSE);
}

void midiint(NEVENTITEM item) {

	nevent_set(NEVENT_MIDIINT, mpu98.stepclock, midiint, NEVENT_RELATIVE);

	if (mpu98.flag1 & MPU1FLAG_A) {
		if (!mpu98.fd_remain) {
			mpu98.fd_remain = mpu98.fd_step[mpu98.fd_cnt & 3];
			mpu98.fd_cnt++;
		}
		mpu98.fd_remain--;
		if (!mpu98.fd_remain) {
			setrecvdata(MPU_INT);
			mpu98ii_int();
		}
	}
	if (mpu98.flag1 & MPU1FLAG_B) {
		if (!mpu98.remainstep++) {
			ch_step();
			mpu98.intreq = 9;
			ch_nextsearch();
		}
	}
	(void)item;
}

void midiwaitout(NEVENTITEM item) {

//	TRACE_("midi ready", 0);
	mpu98.status &= ~MIDIOUT_BUSY;
	(void)item;
}

static void midiwait(SINT32 waitclock) {

	if (!nevent_iswork(NEVENT_MIDIWAIT)) {
		mpu98.status |= MIDIOUT_BUSY;
		nevent_set(NEVENT_MIDIWAIT, waitclock, midiwaitout, NEVENT_ABSOLUTE);
	}
}

static BOOL sendcmd(REG8 cmd) {

	REG8	work;

	mpu98.cmd = cmd;
	switch(cmd & 0xf0) {
		case 0xe0:				// send 2nddata
			mpu98.recvevent |= MIDIE_2NDPARA;
			return(TRUE);

		case 0xa0:				// recv data
			return(TRUE);

		case 0xc0:				// timebase
			work = cmd & 0x0f;
			if ((!work) || (work > MPU98_MAXTIMEBASE)) {
				return(FALSE);
			}
			mpu98.timebase = work;
			makeintclock();
			return(TRUE);
	}

	switch(cmd) {
		case 0xd0:				// send short
		case 0xd1:
		case 0xd2:
		case 0xd3:
		case 0xd4:
		case 0xd5:
		case 0xd6:
		case 0xd7:
		case 0xdf:				// send long
			break;

		case 0xff:				// reset
			cm_mpu98->msg(cm_mpu98, COMMSG_MIDIRESET, 0);
			mpu98.intch = 0;
			mpu98.recvevent = 0;
			mpu98.intreq = 0;
			mpu98.flag1 = 0;
			mpu98.remainstep = 0;
			ZeroMemory(mpu98.ch, sizeof(mpu98.ch));
			ZeroMemory(&mpu98.f9, sizeof(mpu98.f9));
			nevent_reset(NEVENT_MIDIINT);

			mpu98.tempo = 120;
			mpu98.tempos = 0x40;
			makeintclock();
			break;

		case 0x3f:				// uart mode on
			mpu98.mode = 1;
			cm_mpu98->msg(cm_mpu98, COMMSG_MIDIRESET, 0);
			break;

		case 0x94:				// disable clock to host
			mpu98.flag1 &= ~MPU1FLAG_A;
			if (!(mpu98.flag1 & MPU1FLAG_B)) {
				nevent_reset(NEVENT_MIDIINT);
			}
			break;

		case 0x05:
			mpu98.flag1 &= ~MPU1FLAG_B;
			mpu98.recvevent = 0;
			mpu98.intreq = 0;
			ZeroMemory(mpu98.ch, sizeof(mpu98.ch));
			ZeroMemory(&mpu98.f9, sizeof(mpu98.f9));
			if (!(mpu98.flag1 & MPU1FLAG_A)) {
				nevent_reset(NEVENT_MIDIINT);
			}
			break;

		case 0x95:				// enable clock to host
			mpu98.flag1 |= MPU1FLAG_A;
			if (!nevent_iswork(NEVENT_MIDIINT)) {
				nevent_set(NEVENT_MIDIINT, mpu98.stepclock,
											midiint, NEVENT_ABSOLUTE);
			}
			break;

		case 0x0a:
			mpu98.flag1 |= MPU1FLAG_B;
			mpu98.remainstep = 0;
			if (!nevent_iswork(NEVENT_MIDIINT)) {
				nevent_set(NEVENT_MIDIINT, mpu98.stepclock,
											midiint, NEVENT_ABSOLUTE);
			}
			break;

		case 0x8e:				// disable f9
			mpu98.flag1 &= ~MPU1FLAG_F9;
			break;

		case 0x8f:				// enable f9
			mpu98.flag1 |= MPU1FLAG_F9;
			break;

#if 0							// 面倒なのでコマンドチェックしない…
		case 0x01:				// send MIDI stop
		case 0x02:				// send MIDI start
		case 0x03:				// send MIDI continue

		case 0x15:				// stop recording, playback and MIDI

		case 0x32:				// ?
		case 0x34:				// return timing bytes in stop mode
		case 0x35:				// enable mode messages to PC
		case 0x38:				// enable sys common messages to PC
		case 0x39:				// Enable real time messages to PC
		case 0x3c:				// use CLS sync
		case 0x3d:				// use SMPTE sync

		case 0x80:				// use MIDI sync
		case 0x81:				// use FSK sync
		case 0x82:				// use MIDI sync
		case 0x83:				// enable metronome without accents
		case 0x84:				// disable metronome
		case 0x8a:				// disable data in stopped mode
		case 0x8b:				// enable data in stop mode
		case 0x8c:				// disable measure end messages to host

		case 0x91:				// enable ext MIDI ctrl
		case 0x97:				// enable system exclusive messages to PC
			break;

		default:
			return(FALSE);
#endif
	}
	return(TRUE);
}

static void group_ex(REG8 cmd, REG8 data) {

	switch(cmd) {
		case 0xe0:				// tempo
			mpu98.tempo = data;
			mpu98.tempos = 0x40;
			makeintclock();
			break;

		case 0xe1:				// ? 相対テンポっぽいけど…
			mpu98.tempos = data;
			makeintclock();
			break;

		case 0xe2:				// ?
			break;

		case 0xe4:				// clocks/click
			break;

		case 0xe6:				// beats/measure
//			TRACE_("beat/measure:", data);
			break;

		case 0xe7:				// send all clocks to host
			sendallclocks(data);
			break;

		case 0xec:				// channel mask?
			mpu98.intch = data;
			break;
	}
}

static void senddat(REG8 data) {

	MPUCH	*ch;

	if (mpu98.recvevent & MIDIE_2NDPARA) {
		mpu98.recvevent ^= MIDIE_2NDPARA;
		group_ex(mpu98.cmd, data);
		if (mpu98.recvevent & MIDIE_EVENT) {
			mpu98.recvevent ^= MIDIE_EVENT;
			ch_nextsearch();
		}
		return;
	}
	if (mpu98.recvevent & MIDIE_STEP) {
		mpu98.recvevent ^= MIDIE_STEP;
		ch = mpu98.ch + mpu98.intreq;
		ch->datas = 0;
		if (data < 0xf0) {
			mpu98.recvevent ^= MIDIE_EVENT;
			ch->step = data;
		}
		else {
			ch->step = 0xf0;
			ch->remain = 0;
			ch->datas = 0;
			ch_nextsearch();
		}
		return;
	}
	if (mpu98.recvevent & MIDIE_EVENT) {
		MPUCH *ch;
		mpu98.recvevent ^= MIDIE_EVENT;
		mpu98.recvevent |= MIDIE_DATA;
		ch = mpu98.ch + mpu98.intreq;
		switch(data & 0xf0) {
			case 0xc0:
			case 0xd0:
				ch->remain = 2;
				ch->rstat = data;
				break;

			case 0x80:
			case 0x90:
			case 0xa0:
			case 0xb0:
			case 0xe0:
				ch->remain = 3;
				ch->rstat = data;
				break;

			case 0xf0:
				ch->remain = 1;
				break;

			default:
				ch->data[0] = ch->rstat;
				ch->datas = 1;
				ch->remain = 2;
				if ((ch->rstat & 0xe0) == 0xc0) {
					ch->remain--;
				}
				break;
		}
	}
	if (mpu98.recvevent & MIDIE_DATA) {
		MPUCH *ch;
		ch = mpu98.ch + mpu98.intreq;
		if (ch->remain) {
			ch->data[ch->datas] = data;
			ch->datas++;
			ch->remain--;
		}
		if (!ch->remain) {
			mpu98.recvevent ^= MIDIE_DATA;
			ch_nextsearch();
		}
		return;
	}

	if (mpu98.recvevent & MIDIE_F9DATA) {
		switch(mpu98.f9.cmd) {
			case 0xdf:				// long message
				if (mpu98.f9.remain) {
					mpu98.f9.remain--;
					mpu98.f9.data[mpu98.f9.datas++] = data;
				}
				if (data == 0xf7) {
					int		i;
					for (i=0; i<mpu98.f9.datas; i++) {
						cm_mpu98->write(cm_mpu98, mpu98.f9.data[i]);
					}
					mpu98.f9.datas = 0;
					mpu98.f9.remain = 0;
					mpu98.f9.cmd = 0xf8;
					return;
				}
				break;

			default:
				mpu98.recvevent ^= MIDIE_F9DATA;
				mpu98.recvevent |= MIDIE_F9CMD;
				mpu98.f9.step = data;
				break;
		}
		return;
	}
	if (mpu98.recvevent & MIDIE_F9CMD) {
		mpu98.recvevent ^= MIDIE_F9CMD;
		if (data == MIDI_STOP) {
			cm_mpu98->write(cm_mpu98, MIDI_STOP);
			setrecvdata(MIDI_STOP);
			mpu98ii_int();
		}
		else {
			mpu98.f9.cmd = data;
			if ((data & 0xf0) == 0xe0) {
				mpu98.recvevent |= MIDIE_F9PARA;
			}
			else {
				ch_nextsearch();
			}
		}
		return;
	}
	if (mpu98.recvevent & MIDIE_F9PARA) {
		mpu98.recvevent ^= MIDIE_F9PARA;
		group_ex(mpu98.f9.cmd, data);
		ch_nextsearch();
		return;
	}
}


static void IOOUTCALL mpu98ii_o0(UINT port, REG8 dat) {

	UINT	sent;

	if (cm_mpu98 == NULL) {
		cm_mpu98 = commng_create(COMCREATE_MPU98II);
	}
	if (cm_mpu98->connect != COMCONNECT_OFF) {

		if (mpu98.mode) {
			sent = cm_mpu98->write(cm_mpu98, (BYTE)dat);
		}
		else {
			if ((mpu98.cmd == 0xd0) || (mpu98.cmd == 0xdf)) {
				sent = cm_mpu98->write(cm_mpu98, (BYTE)dat);
			}
			else {
				senddat(dat);
				sent = 1;
			}
		}
		if (sent) {
			midiwait(mpu98.xferclock * sent);
		}
	}
	(void)port;
}

static void IOOUTCALL mpu98ii_o2(UINT port, REG8 dat) {

	if (cm_mpu98 == NULL) {
		cm_mpu98 = commng_create(COMCREATE_MPU98II);
	}
	if (cm_mpu98->connect != COMCONNECT_OFF) {
		if (!mpu98.mode) {
			if (sendcmd(dat)) {
				setrecvdata(MPU_ACK);
				mpu98ii_int();
				switch(dat) {
					case 0xac:			// get mpu major version?
						setrecvdata(1);
						break;
					case 0xad:			// get mpu minor version?
						setrecvdata(0);
						break;
				}
			}
		}
		else {
			if (dat == 0xff) {
				mpu98.mode = 0;
				setrecvdata(MPU_ACK);
			}
		}
		midiwait(pccore.realclock / 10000);
	}
	(void)port;
}

static REG8 IOINPCALL mpu98ii_i0(UINT port) {

	if (cm_mpu98 == NULL) {
		cm_mpu98 = commng_create(COMCREATE_MPU98II);
	}
	if (cm_mpu98->connect != COMCONNECT_OFF) {
		if (mpu98.r.cnt) {
			mpu98.r.cnt--;
			if (mpu98.r.cnt) {
				mpu98ii_int();
			}
			else {
				pic_resetirq(mpu98.irqnum);
			}
			mpu98.data = mpu98.r.buf[mpu98.r.pos];
			mpu98.r.pos = (mpu98.r.pos + 1) & (MPU98_RECVBUFS - 1);
		}
		return(mpu98.data);
	}
	(void)port;
	return(0xff);
}

static REG8 IOINPCALL mpu98ii_i2(UINT port) {

	REG8	ret;

	if (cm_mpu98 == NULL) {
		cm_mpu98 = commng_create(COMCREATE_MPU98II);
	}
	if (cm_mpu98->connect != COMCONNECT_OFF) {
		ret = mpu98.status;
		if (!mpu98.r.cnt) {
			ret |= MIDIIN_AVAIL;
		}
		return(ret);
	}
	(void)port;
	return(0xff);
}


// ---- I/F

void mpu98ii_construct(void) {

	cm_mpu98 = NULL;
}

void mpu98ii_destruct(void) {

	commng_destroy(cm_mpu98);
	cm_mpu98 = NULL;
}

void mpu98ii_reset(void) {

	commng_destroy(cm_mpu98);
	cm_mpu98 = NULL;

	ZeroMemory(&mpu98, sizeof(mpu98));
	mpu98.data = MPU_ACK;
	mpu98.tempo = 120;
	mpu98.tempos = 0x40;
	mpu98.timebase = 2;
	mpu98.port = 0xc0d0 | ((np2cfg.mpuopt & 0xf0) << 6);
	mpu98.irqnum = mpuirqnum[np2cfg.mpuopt & 3];
//	pic_registext(mpu98.irqnum);
}

void mpu98ii_bind(void) {

	UINT	port;

	mpu98.xferclock = pccore.realclock / 3125;
	makeintclock();
	port = mpu98.port;
	iocore_attachout(port, mpu98ii_o0);
	iocore_attachinp(port, mpu98ii_i0);
	port |= 2;
	iocore_attachout(port, mpu98ii_o2);
	iocore_attachinp(port, mpu98ii_i2);
}

void mpu98ii_callback(void) {

	BYTE	data;

	if (cm_mpu98) {
		while((mpu98.r.cnt < MPU98_RECVBUFS) &&
			(cm_mpu98->read(cm_mpu98, &data))) {
			if (!mpu98.r.cnt) {
				mpu98ii_int();
			}
			setrecvdata(data);
		}
	}
}

void mpu98ii_midipanic(void) {

	if (cm_mpu98) {
		cm_mpu98->msg(cm_mpu98, COMMSG_MIDIRESET, 0);
	}
}

