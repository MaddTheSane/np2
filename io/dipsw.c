#include	"compiler.h"
#include	"sysmng.h"
#include	"i286.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"beep.h"


void IOOUTCALL dipsw_w8(UINT port, BYTE value) {

	BYTE	set;
	UINT	update;

	update = 0;
	switch(port & 0x0f00) {
		case 0x0400:
			if (!(iflags[value] & 0x04)) {
				set = (value ^ 0x04) & 0xbc;
				if ((np2cfg.dipsw[0] ^ set) & 0xbc) {
					np2cfg.dipsw[0] &= ~(0xbc);
					np2cfg.dipsw[0] |= set;
					update |= SYS_UPDATECFG;
				}
			}
			break;

		case 0x0500:
			if (!(iflags[value] & 0x04)) {
				set = value & 0xaf;
				if ((np2cfg.dipsw[1] ^ set) & 0xaf) {
					np2cfg.dipsw[1] &= ~(0xaf);
					np2cfg.dipsw[1] |= set;
					update |= SYS_UPDATECFG;
				}
			}
			break;

		case 0x0600:
			if (!(iflags[value] & 0x04)) {
				set = (value ^ 0x08) & 0x3f;
				if ((np2cfg.dipsw[2] ^ set) & 0x3f) {
					np2cfg.dipsw[2] &= ~(0x3f);
					np2cfg.dipsw[2] |= set;
					update |= SYS_UPDATECFG;
				}
			}
			break;

		case 0x0700:
			if (!(iflags[value] & 0x04)) {
				set = (value & 0x20) >> 1;
				if ((np2cfg.dipsw[1] ^ set) & 0x10) {
					np2cfg.dipsw[1] ^= 0x10;
					update |= SYS_UPDATECFG;
				}
				set = (value >> 2) & 0x03;
				if (np2cfg.BEEP_VOL != set) {
					np2cfg.BEEP_VOL = set;
					beep_setvol(set);
					update |= SYS_UPDATECFG;
				}
			}
			break;

		case 0x0e00:
			if (!(iflags[value] & 0x04)) {
				set = (value & 0x10) << 3;
				if ((np2cfg.dipsw[2] ^ set) & 0x80) {
					np2cfg.dipsw[2] ^= 0x80;
					update |= SYS_UPDATECFG;
				}
			}
			break;
	}
	sysmng_update(update);
}

BYTE IOINPCALL dipsw_r8(UINT port) {

	BYTE	ret = 0xff;

	switch(port & 0x0f00) {
		case 0x0400:
			ret = (np2cfg.dipsw[0] ^ 0x04) & 0xbc;
			if (iflags[ret] & 0x04) {
				ret |= 0x01;
			}
			break;

		case 0x0500:
			ret = np2cfg.dipsw[1] & 0xaf;
			if (iflags[ret] & 0x04) {
				ret |= 0x10;
			}
			break;

		case 0x0600:
			ret = (np2cfg.dipsw[2] ^ 0x02) & 0x33;
			if (iflags[ret] & 0x04) {
				ret |= 0x80;
			}
			break;

		case 0x0700:
			ret = ((np2cfg.dipsw[1] & 0x10) << 1) |
					((np2cfg.BEEP_VOL & 0x03) << 2);
			if (iflags[ret] & 0x04) {
				ret |= 0x80;
			}
			break;

		case 0x0e00:
			ret = (np2cfg.dipsw[2] & 0x80) >> 3;
			if (iflags[ret] & 0x04) {
				ret |= 0x80;
			}
			break;
	}
	return(ret);
}

