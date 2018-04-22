#include "resource.h"
#include <Carbon/Carbon.r>

resource 'MENU' (IDM_EDIT) {
	IDM_EDIT,
	textMenuProc,
	0xBD,
	disabled,
	"Edit",
	{	/* array: 8 elements */
		/* [1] */
		"Undo", noIcon, "Z", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Cut", noIcon, "X", noMark, plain,
		/* [4] */
		"Copy", noIcon, "C", noMark, plain,
		/* [5] */
		"Paste", noIcon, "V", noMark, plain,
		/* [6] */
		"Clear", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"Select All", noIcon, "A", noMark, plain
	}
};

resource 'MENU' (IDM_EMULATE) {
	IDM_EMULATE,
	textMenuProc,
	0x1D,
	enabled,
	"Emulate",
	{	/* array: 5 elements */
		/* [1] */
		"Reset", noIcon, "R", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Configure...", noIcon, ";", noMark, plain,
		/* [4] */
		"New Disk...", noIcon, "N", noMark, plain,
		/* [5] */
		"Font...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (130) {
	IDM_EMULATE,
	textMenuProc,
	0x5D,
	enabled,
	"Emulate",
	{	/* array: 7 elements */
		/* [1] */
		"Reset", noIcon, "R", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Configure...", noIcon, ";", noMark, plain,
		/* [4] */
		"New Disk...", noIcon, "N", noMark, plain,
		/* [5] */
		"Font...", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain,
		/* [7] */
		"Quit", noIcon, "Q", noMark, plain
	}
};

resource 'MENU' (IDM_DEVICE) {
	IDM_DEVICE,
	textMenuProc,
	0x58F,
	enabled,
	"Device",
	{	/* array: 11 elements */
		/* [1] */
		"Keyboard", noIcon, hierarchicalMenu, "ã", plain,
		/* [2] */
		"Sound", noIcon, hierarchicalMenu, "å", plain,
		/* [3] */
		"Memory", noIcon, hierarchicalMenu, "ç", plain,
		/* [4] */
		"Mouse", noIcon, "M", noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"Serial options...", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"MIDI options...", noIcon, noKey, noMark, plain,
		/* [9] */
		"MIDI Panic", noIcon, noKey, noMark, plain,
		/* [10] */
		"-", noIcon, noKey, noMark, plain,
		/* [11] */
		"Sound options...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SCREEN) {
	IDM_SCREEN,
	textMenuProc,
	0xBFDD,
	enabled,
	"Screen",
	{	/* array: 16 elements */
		/* [1] */
		"Fullscreen", noIcon, "F", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Normal", noIcon, noKey, noMark, plain,
		/* [4] */
		"Rotate Left", noIcon, noKey, noMark, plain,
		/* [5] */
		"Rotate Right", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain,
		/* [7] */
		"Display VSync", noIcon, noKey, noMark, plain,
		/* [8] */
		"Real Palettes", noIcon, noKey, noMark, plain,
		/* [9] */
		"No Wait", noIcon, noKey, noMark, plain,
		/* [10] */
		"Auto frame", noIcon, noKey, noMark, plain,
		/* [11] */
		"Full frame", noIcon, noKey, noMark, plain,
		/* [12] */
		"1/2 frame", noIcon, noKey, noMark, plain,
		/* [13] */
		"1/3 frame", noIcon, noKey, noMark, plain,
		/* [14] */
		"1/4 frame", noIcon, noKey, noMark, plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"Screen options...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_HELP) {
	IDM_HELP,
	textMenuProc,
	0x1,
	enabled,
	"Help",
	{	/* array: 1 elements */
		/* [1] */
		"Neko Project IIx Help", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SOUND) {
	IDM_SOUND,
	textMenuProc,
	0x2FFEF,
	enabled,
	"Sound",
	{	/* array: 18 elements */
		/* [1] */
		"Beep off", noIcon, noKey, noMark, plain,
		/* [2] */
		"Beep low", noIcon, noKey, noMark, plain,
		/* [3] */
		"Beep mid", noIcon, noKey, noMark, plain,
		/* [4] */
		"Beep high", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"Disable boards", noIcon, noKey, noMark, plain,
		/* [7] */
		"PC-9801-14", noIcon, noKey, noMark, plain,
		/* [8] */
		"PC-9801-26K", noIcon, noKey, noMark, plain,
		/* [9] */
		"PC-9801-86", noIcon, noKey, noMark, plain,
		/* [10] */
		"PC-9801-26 + 86", noIcon, noKey, noMark, plain,
		/* [11] */
		"PC-9801-86 + chibi-oto", noIcon, noKey, noMark, plain,
		/* [12] */
		"PC-9801-118", noIcon, noKey, noMark, plain,
		/* [13] */
		"Speak board", noIcon, noKey, noMark, plain,
		/* [14] */
		"Spark board", noIcon, noKey, noMark, plain,
		/* [15] */
		"AMD98", noIcon, noKey, noMark, plain,
		/* [16] */
		"JastSound", noIcon, noKey, noMark, plain,
		/* [17] */
		"-", noIcon, noKey, noMark, plain,
		/* [18] */
		"Seek Sound", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SCSI0) {
	IDM_SCSI0,
	textMenuProc,
	0x5,
	enabled,
	"SCSI0",
	{	/* array: 3 elements */
		/* [1] */
		"Open...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Remove", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SCSI1) {
	IDM_SCSI1,
	textMenuProc,
	0x5,
	enabled,
	"SCSI1",
	{	/* array: 3 elements */
		/* [1] */
		"Open...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Remove", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SCSI2) {
	IDM_SCSI2,
	textMenuProc,
	0x5,
	enabled,
	"SCSI2",
	{	/* array: 3 elements */
		/* [1] */
		"Open...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Remove", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SCSI3) {
	IDM_SCSI3,
	textMenuProc,
	0x5,
	enabled,
	"SCSI3",
	{	/* array: 3 elements */
		/* [1] */
		"Open...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Remove", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SASI1) {
	IDM_SASI1,
	textMenuProc,
	0x5,
	enabled,
	"SASI1",
	{	/* array: 3 elements */
		/* [1] */
		"Open...", noIcon, "O", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Remove", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SASI2) {
	IDM_SASI2,
	textMenuProc,
	0x5,
	enabled,
	"SASI2",
	{	/* array: 3 elements */
		/* [1] */
		"Open...", noIcon, "O", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Remove", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_FDD1) {
	IDM_FDD1,
	textMenuProc,
	0x5,
	enabled,
	"FDD1",
	{	/* array: 3 elements */
		/* [1] */
		"Open...", noIcon, "D", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Eject", noIcon, "E", noMark, plain
	}
};

resource 'MENU' (IDM_FDD2) {
	IDM_FDD2,
	textMenuProc,
	0x5,
	enabled,
	"FDD2",
	{	/* array: 3 elements */
		/* [1] */
		"Open...", noIcon, "D", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Eject", noIcon, "E", noMark, plain
	}
};

resource 'MENU' (IDM_HDD) {
	IDM_HDD,
	textMenuProc,
	0x7B,
	enabled,
	"HDD",
	{	/* array: 7 elements */
		/* [1] */
		"SASI-1", noIcon, hierarchicalMenu, "á", plain,
		/* [2] */
		"SASI-2", noIcon, hierarchicalMenu, "à", plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain,
		/* [4] */
		"SCSI #0", noIcon, hierarchicalMenu, "ë", plain,
		/* [5] */
		"SCSI #1", noIcon, hierarchicalMenu, "í", plain,
		/* [6] */
		"SCSI #2", noIcon, hierarchicalMenu, "ì", plain,
		/* [7] */
		"SCSI #3", noIcon, hierarchicalMenu, "î", plain
	}
};

resource 'MENU' (IDM_KEYBOARD) {
	IDM_KEYBOARD,
	textMenuProc,
	0xFBEEF,
	enabled,
	"Keyboard",
	{	/* array: 20 elements */
		/* [1] */
		"Keyboard", noIcon, "K", noMark, plain,
		/* [2] */
		"JoyKey-1", noIcon, "J", noMark, plain,
		/* [3] */
		"JoyKey-2", noIcon, noKey, noMark, plain,
		/* [4] */
		"MouseKey", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"Mechanical SHIFT", noIcon, noKey, noMark, plain,
		/* [7] */
		"Mechanical CTRL", noIcon, noKey, noMark, plain,
		/* [8] */
		"Mechanical GRPH", noIcon, noKey, noMark, plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"F11 = KANA", noIcon, noKey, noMark, plain,
		/* [11] */
		"F11 = Stop", noIcon, noKey, noMark, plain,
		/* [12] */
		"F11 = 10Key [=]", noIcon, noKey, noMark, plain,
		/* [13] */
		"F11 = NFER", noIcon, noKey, noMark, plain,
		/* [14] */
		"F11 = User1", noIcon, noKey, noMark, plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"F12 = Mouse", noIcon, noKey, noMark, plain,
		/* [17] */
		"F12 = Copy", noIcon, noKey, noMark, plain,
		/* [18] */
		"F12 = 10Key [,]", noIcon, noKey, noMark, plain,
		/* [19] */
		"F12 = XFER", noIcon, noKey, noMark, plain,
		/* [20] */
		"F12 = User2", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_MEMORY) {
	IDM_MEMORY,
	textMenuProc,
	0xF,
	enabled,
	"Memory",
	{	/* array: 5 elements */
		/* [1] */
		"640KB", noIcon, noKey, noMark, plain,
		/* [2] */
		"1.6MB", noIcon, noKey, noMark, plain,
		/* [3] */
		"3.6MB", noIcon, noKey, noMark, plain,
		/* [4] */
		"7.6MB", noIcon, noKey, noMark, plain,
		/* [5] */
		"", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_APPLE) {
	IDM_APPLE,
	textMenuProc,
	0x1,
	enabled,
	apple,
	{	/* array: 2 elements */
		/* [1] */
		"About Neko Project IIx...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_OTHER) {
	IDM_OTHER,
	textMenuProc,
	0xEFF,
	enabled,
	"Other",
	{	/* array: 12 elements */
		/* [1] */
		"Save BMP...", noIcon, noKey, noMark, plain,
		/* [2] */
		"S98 logging...", noIcon, noKey, noMark, plain,
		/* [3] */
		"Calendar...", noIcon, noKey, noMark, plain,
		/* [4] */
		"Clock Display", noIcon, noKey, noMark, plain,
		/* [5] */
		"Frame Display", noIcon, noKey, noMark, plain,
		/* [6] */
		"Reverse Joystick", noIcon, noKey, noMark, plain,
		/* [7] */
		"Rapid Joystick", noIcon, noKey, noMark, plain,
		/* [8] */
		"Rapid Mouse button", noIcon, noKey, noMark, plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"Sound Recording...", noIcon, "S", noMark, plain,
		/* [11] */
		"Key Display", noIcon, noKey, noMark, plain,
		/* [12] */
		"Soft Keyboard", noIcon, noKey, noMark, plain
	}
};
