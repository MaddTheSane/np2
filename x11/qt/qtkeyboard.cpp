/*
 * Copyright (c) 2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"

#include "np2.h"
#include "pccore.h"
#include "iocore.h"

#include "mousemng.h"	// XXX

#include "qt/qtkeyboard.h"

#include <qkeycode.h>


#define	NC	KEYBOARD_KC_NC

static BYTE get_data(int keysym, BYTE down);

static BYTE shift_stat;


//
// Event
//
void
emulationWindow::keyPressEvent(QKeyEvent *ev)
{
	int keysym = ev->key();

	if ((keysym == Qt::Key_F12) && (np2oscfg.F12KEY == 0)) {
		mouse_running(M_XOR);	// XXX
		return;
	}

	BYTE data = get_data(keysym, 0x80);
	if (data != NC) {
		if ((data & 0x80) == 0) {
			keystat_senddata(data);
		} else {
			BYTE s = (shift_stat & 0x80) | 0x70;
			keystat_senddata(s);
			keystat_senddata(data & 0x7f);
			keystat_senddata(s ^ 0x80);
		}
	}
}

void
emulationWindow::keyReleaseEvent(QKeyEvent *ev)
{
	int keysym = ev->key();

	if ((keysym == Qt::Key_F12) && (np2oscfg.F12KEY == 0)) {
		return;
	}

	BYTE data = get_data(keysym, 0x00);
	if (data != NC) {
		keystat_senddata(data | 0x80);
	}
}


//
// kbdmng
//
static const BYTE keyconv_jis[256] = {
	// 0x00:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x08:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x10:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x18:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x20: SPC,  ! ,  " ,  # ,  $ ,  % ,  & ,  '
		0x34,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	// 0x28:  ( ,  ) ,  * ,  + ,  , ,  - ,  . ,  /
		0x08,0x09,0x27,0x26,0x30,0x0b,0x31,0x32,
	// 0x30:  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7
		0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	// 0x38:  8 ,  9 ,  : ,  ; ,  < ,  = ,  > ,  ?
		0x08,0x09,0x27,0x26,0x30,0x0b,0x31,0x32,
	// 0x40:  @ ,  A ,  B ,  C ,  D ,  E ,  F ,  G
		0x1a,0x1d,0x2d,0x2b,0x1f,0x12,0x20,0x21,
	// 0x48:  H ,  I ,  J ,  K ,  L ,  M ,  N ,  O
		0x22,0x17,0x23,0x24,0x25,0x2f,0x2e,0x18,
	// 0x50:  P ,  Q ,  R ,  S ,  T ,  U ,  V ,  W
		0x19,0x10,0x13,0x1e,0x14,0x16,0x2c,0x11,
	// 0x58:  X ,  Y ,  Z ,  [ ,  \ ,  ] ,  ^ ,  _
		0x2a,0x15,0x29,0x1b,0x0d,0x28,0x0c,0x33,
	// 0x60:  `
		0x1a,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x68:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x70:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x78:                 { ,  | ,  } ,  ~ ,
		  NC,  NC,  NC,0x1b,0x0d,0x28,0x0c,  NC,
	// 0x80:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x88:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x90:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x98:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xa0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xa8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xb0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xb8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xc0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xc8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xd0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xd8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xe0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xe8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xf0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xf8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
};

static const BYTE keyconv_misc[256] = {				// 0x10xx
	// 0x00: ESC, TAB,BTAB,  BS, RET,ENTR, INS, DEL,
		0x00,0x0f,  NC,0x0e,0x1c,0x1c,0x38,0x39,
	// 0x08:PAUS,PRNT,SYSQ, CLR,
		0x60,0x62,0x62,0x47,  NC,  NC,  NC,  NC,
	// 0x10:HOME, END,  вл,  вм,  вк,  вн,RLDN,RLUP,
		0x3e,0x3f,0x3b,0x3a,0x3c,0x3d,0x37,0x3f,
	// 0x18:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x20:SHFT,CTRL,META, ALT,CAPS,NUML,SCRL,
		0x70,0x74,0x51,0x51,0x71,  NC,0x71,  NC,
	// 0x28:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x30: f.1, f.2, f.3, f.4, f.5, f.6, f.7, f.8,
		0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
	// 0x38: f.9,f.10,f.11,f.12,f.13,f.14,f.15
		0x6a,0x6b,0x73,  NC,  NC,  NC,  NC,  NC,
	// 0x40:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x48:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x50:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x58:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x60:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x68:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x70:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x78:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x80:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x88:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x90:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0x98:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xa0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xa8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xb0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xb8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xc0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xc8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xd0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xd8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xe0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xe8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xf0:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	// 0xf8:
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
};

static const BYTE *keyconv;


BOOL
kbdmng_init(void)
{

	keyconv = keyconv_jis;

	return SUCCESS;
}

static BYTE
get_data(int keysym, BYTE down)
{
	BYTE data;

	if (keysym & ~0xff) {
		if ((keysym & 0xffffff00) != 0x00001000) {
			data = NC;
		} else if (keysym == Qt::Key_F12) {
			data = kbdmng_getf12key();
		} else {
			data = keyconv_misc[keysym & 0xff];
			if (data == 0x70) {
				shift_stat = down;
			}
		}
	} else {
		if ((keysym == Qt::Key_AsciiTilde)
		 && shift_stat
		 && (np2oscfg.KEYBOARD == KEY_KEY106)) {
			data = 0x0a;	/* Shift + '0' -> '0', not '~' */
		} else {
			data = keyconv[keysym];
		}
	}

	return data;
}
