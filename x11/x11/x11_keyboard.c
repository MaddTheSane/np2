/*	$Id: x11_keyboard.c,v 1.1 2004/02/06 16:52:48 monaka Exp $	*/

#include "compiler.h"

#include "np2.h"

#include "inputmng.h"
#include "kbdmng.h"

#include "x11/xnp2.h"
#include <X11/keysym.h>


#define	NC	0xff

static BYTE xkeyconv_jis[256] = {
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x00
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x08
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x10
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x18
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	 SPC,  ! ,  " ,  # ,  $ ,  % ,  & ,  '		; 0x20
		0x34,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	//	  ( ,  ) ,  * ,  + ,  , ,  - ,  . ,  /		; 0x28
		0x08,0x09,0x26,0x27,0x30,0x0b,0x31,0x32,
	//	  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7		; 0x30
		0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	//	  8 ,  9 ,  : ,  ; ,  < ,  = ,  > ,  ? 		; 0x38
		0x08,0x09,0x26,0x27,0x30,0x0b,0x31,0x32,
	//	  @ ,  A ,  B ,  C ,  D ,  E ,  F ,  G		; 0x40
		0x1a,0x1d,0x2d,0x2b,0x1f,0x12,0x20,0x21,
	//	  H ,  I ,  J ,  K ,  L ,  M ,  N ,  O		; 0x48
		0x22,0x17,0x23,0x24,0x25,0x2f,0x2e,0x18,
	//	  P ,  Q ,  R ,  S ,  T ,  U ,  V ,  W		; 0x50
		0x19,0x10,0x13,0x1e,0x14,0x16,0x2c,0x11,
	//	  X ,  Y ,  Z ,  [ ,  \ ,  ] ,  ^ ,  _		; 0x58
		0x2a,0x15,0x29,0x1b,0x0d,0x28,0x0c,0x33,
	//	  ` ,  a ,  b ,  c ,  d ,  e ,  f ,  g		; 0x60
		0x1a,0x1d,0x2d,0x2b,0x1f,0x12,0x20,0x21,
	//	  h ,  i ,  j ,  k ,  l ,  m ,  n ,  o		; 0x68
		0x22,0x17,0x23,0x24,0x25,0x2f,0x2e,0x18,
	//	  p ,  q ,  r ,  s ,  t ,  u ,  v ,  w		; 0x70
		0x19,0x10,0x13,0x1e,0x14,0x16,0x2c,0x11,
	//	  x ,  y ,  z ,  { ,  | ,  } ,  ~ ,   		; 0x78
		0x2a,0x15,0x29,0x1b,0x0d,0x28,0x0c,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x80
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x88
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x90
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x98
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xa8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xb0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xb8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xc0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xc8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xd8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xe0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xe8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC
};

static BYTE xkeyconv_ascii[256] = {
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x00
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x08
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x10
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x18
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	 SPC,  ! ,  " ,  # ,  $ ,  % ,  & ,  '		; 0x20
		0x34,0x01,0x27,0x03,0x04,0x05,0x07,0x06,
	//	  ( ,  ) ,  * ,  + ,  , ,  - ,  . ,  /		; 0x28
		0x09,0x0a,0x08,0x0c,0x30,0x0b,0x31,0x32,
	//	  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7		; 0x30
		0x0a,0x01,0x27,0x03,0x04,0x05,0x06,0x07,
	//	  8 ,  9 ,  : ,  ; ,  < ,  = ,  > ,  ? 		; 0x38
		0x08,0x09,0x26,0x0c,0x30,0x0b,0x31,0x32,
	//	  @ ,  A ,  B ,  C ,  D ,  E ,  F ,  G		; 0x40
		0x02,0x1d,0x2d,0x2b,0x1f,0x12,0x20,0x21,
	//	  H ,  I ,  J ,  K ,  L ,  M ,  N ,  O		; 0x48
		0x22,0x17,0x23,0x24,0x25,0x2f,0x2e,0x18,
	//	  P ,  Q ,  R ,  S ,  T ,  U ,  V ,  W		; 0x50
		0x19,0x10,0x13,0x1e,0x14,0x16,0x2c,0x11,
	//	  X ,  Y ,  Z ,  [ ,  \ ,  ] ,  ^ ,  _		; 0x58
		0x2a,0x15,0x29,0x1b,0x0d,0x28,0x06,0x0b,
	//	  ` ,  a ,  b ,  c ,  d ,  e ,  f ,  g		; 0x60
		0x1a,0x1d,0x2d,0x2b,0x1f,0x12,0x20,0x21,
	//	  h ,  i ,  j ,  k ,  l ,  m ,  n ,  o		; 0x68
		0x22,0x17,0x23,0x24,0x25,0x2f,0x2e,0x18,
	//	  p ,  q ,  r ,  s ,  t ,  u ,  v ,  w		; 0x70
		0x19,0x10,0x13,0x1e,0x14,0x16,0x2c,0x11,
	//	  x ,  y ,  z ,  { ,  | ,  } ,  ~ ,   		; 0x78
		0x2a,0x15,0x29,0x1b,0x0d,0x28,0x1a,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x80
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x88
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x90
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x98
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xa8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xb0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xb8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xc0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xc8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xd8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xe0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xe8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
};

static BYTE xkeyconv_misc[256] = {
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x00
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	  BS, TAB,  LF, CLR,    , RET,    ,   		; 0x08
		0x0e,0x0f,0x1c,0x47,  NC,0x1c,  NC,  NC,
	//	    ,    ,    ,PAUS,SCRL,SYSQ,    ,  		; 0x10
		  NC,  NC,  NC,0x60,0x33,0x62,  NC,  NC,
	//	    ,    ,    , ESC,    ,    ,    ,   		; 0x18
		  NC,  NC,  NC,0x00,  NC,  NC,  NC,  NC,
	//	    ,KANJ,MUHE,HENM,HENK,RONM,HIRA,KATA		; 0x20
		  NC,  NC,0x51,0x35,0x35,0x72,0x72,0x72,
	//	HIKA,ZENK,HANK,ZNHN,    ,KANA,    ,   		; 0x28
		0x72,  NC,  NC,  NC,  NC,0x72,  NC,  NC,
	//	ALNU,    ,    ,    ,    ,    ,    ,    		; 0x30
		0x71,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,ZKOU,MKOU,   		; 0x38
		  NC,  NC,  NC,  NC,  NC,0x35,0x35,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x40
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x48
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	HOME,  вл,  вм,  вк,  вн,RLDN,RLUP, END		; 0x50
		0x3e,0x3b,0x3a,0x3c,0x3d,0x37,0x36,0x3f,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x58
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,PRNT, INS,    ,    ,    ,    ,    		; 0x60
		  NC,0x62,0x38,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,BREA,    ,    ,    ,   		; 0x68
		  NC,  NC,  NC,0x60,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x70
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x78
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	<SPC,    ,    ,    ,    ,    ,    ,    		; 0x80
		0x34,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,<TAB,    ,    ,    ,<ENT,    ,  		; 0x88
		  NC,0x0f,  NC,  NC,  NC,0x1c,  NC,  NC,
	//	    ,    ,    ,    ,    ,<HOM,<вл>,<вм>		; 0x90
		  NC,  NC,  NC,  NC,  NC,0x3e,0x3b,0x3a,
	//	<вк>,<вн>,<RDN,<RUP,<END,    ,<INS,<DEL		; 0x98
		0x3c,0x3d,0x37,0x36,0x3f,  NC,0x38,0x39,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    , <*>, <+>, <,>, <->, <.>, </>		; 0xa8
		  NC,  NC,0x45,0x49,0x4f,0x40,0x50,0x41,
	//	 <0>, <1>, <2>, <3>, <4>, <5>, <6>, <7>		; 0xb0
		0x4e,0x4a,0x4b,0x4c,0x46,0x47,0x48,0x42,
	//	 <8>, <9>,    ,    ,    ,    , f.1, f.2		; 0xb8
		0x43,0x44,  NC,  NC,  NC,  NC,0x62,0x63,
	//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9,f.10		; 0xc0
		0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,
	//	f.11,f.12,f.13,f.14,f.15,    ,    ,   		; 0xc8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xd8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,SFTL,SFTR,CTLL,CTLR,CAPS,    ,METL		; 0xe0
		  NC,0x70,0x70,0x74,0x74,0x71,  NC,0x73,
	//	METR,ALTL,ALTR,    ,    ,    ,    ,    		; 0xe8
		0x73,0x73,0x73,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    , DEL		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,0x3a
};

static BYTE *xkeyconv = xkeyconv_jis;
static BYTE shift_stat = 0x00;


BOOL
kbdmng_init(void)
{

	shift_stat = 0x00;
	if (np2oscfg.KEYBOARD == KEY_KEY101)
		xkeyconv = xkeyconv_ascii;
	else
		xkeyconv = xkeyconv_jis;

	return SUCCESS;
}

static BYTE
get_data(KeySym keysym, BYTE down)
{
	BYTE data;

	if (keysym & ~0xff) {
		if (keysym == XK_VoidSymbol) {
			data = NC;
		} else if (keysym == XK_F12) {
			data = kbdmng_getf12key();
		} else if ((keysym & 0xff00) == 0xff00) {
			data = xkeyconv_misc[keysym & 0xff];
			if (data == 0x70) {
				shift_stat = down;
			}
		} else {
			data = NC;
		}
	} else {
		if ((keysym == XK_asciitilde)
		 && shift_stat
		 && (np2oscfg.KEYBOARD == KEY_KEY106)) {
			data = 0x0a;	/* Shift + '0' -> '0', not '~' */
		} else {
			data = xkeyconv[keysym];
		}
	}

	return data;
}

void
x11kbd_keydown(KeySym keysym)
{
	BYTE data;

	data = get_data(keysym, 0x80);
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
x11kbd_keyup(KeySym keysym)
{
	BYTE data;

	data = get_data(keysym, 0x00);
	if (data != NC) {
		keystat_senddata(data | 0x80);
	}
}
