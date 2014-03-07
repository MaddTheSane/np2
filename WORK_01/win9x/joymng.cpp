#include	"compiler.h"
#include	"np2.h"
#include	"joymng.h"
#include	"menu.h"


enum {
	JOY_LEFT_BIT	= 0x04,
	JOY_RIGHT_BIT	= 0x08,
	JOY_UP_BIT		= 0x01,
	JOY_DOWN_BIT	= 0x02,
	JOY_BTN1_BIT	= 0x10,
	JOY_BTN2_BIT	= 0x20
};

static	REG8	joyflag = 0xff;
static	UINT8	joypad1btn[4];


void joymng_initialize(void) {

	JOYINFO		ji;
	int			i;

	if ((!joyGetNumDevs()) ||
		(joyGetPos(JOYSTICKID1, &ji) == JOYERR_UNPLUGGED)) {
		np2oscfg.JOYPAD1 |= 2;
	}
	for (i=0; i<4; i++) {
		joypad1btn[i] = 0xff ^
			((np2oscfg.JOY1BTN[i] & 3) << ((np2oscfg.JOY1BTN[i] & 4)?4:6));
	}
}

void joymng_sync(void) {

	np2oscfg.JOYPAD1 &= 0x7f;
	joyflag = 0xff;
}

REG8 joymng_getstat(void) {

	JOYINFO		ji;

	if ((np2oscfg.JOYPAD1 == 1) &&
		(joyGetPos(JOYSTICKID1, &ji) == JOYERR_NOERROR)) {
		np2oscfg.JOYPAD1 |= 0x80;
		joyflag = 0xff;
		if (ji.wXpos < 0x4000U) {
			joyflag &= ~JOY_LEFT_BIT;
		}
		else if (ji.wXpos > 0xc000U) {
			joyflag &= ~JOY_RIGHT_BIT;
		}
		if (ji.wYpos < 0x4000U) {
			joyflag &= ~JOY_UP_BIT;
		}
		else if (ji.wYpos > 0xc000U) {
			joyflag &= ~JOY_DOWN_BIT;
		}
		if (ji.wButtons & JOY_BUTTON1) {
			joyflag &= joypad1btn[0];							// ver0.28
		}
		if (ji.wButtons & JOY_BUTTON2) {
			joyflag &= joypad1btn[1];							// ver0.28
		}
		if (ji.wButtons & JOY_BUTTON3) {
			joyflag &= joypad1btn[2];							// ver0.28
		}
		if (ji.wButtons & JOY_BUTTON4) {
			joyflag &= joypad1btn[3];							// ver0.28
		}
	}
	return(joyflag);
}

// joyflag	bit:0		up
// 			bit:1		down
// 			bit:2		left
// 			bit:3		right
// 			bit:4		trigger1 (rapid)
// 			bit:5		trigger2 (rapid)
// 			bit:6		trigger1
// 			bit:7		trigger2

