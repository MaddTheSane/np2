#include "resource.h"
#include <Carbon/Carbon.r>

resource 'MENU' (IDM_FDD1) {
	IDM_FDD1,
	textMenuProc,
	0x5,
	enabled,
	"FDD1",
	{	/* array: 3 elements */
		/* [1] */
		"ŠJ‚­...", noIcon, "D", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"æ‚èo‚µ", noIcon, "E", noMark, plain
	}
};

resource 'MENU' (IDM_EDIT) {
	IDM_EDIT,
	textMenuProc,
	0x1AF,
	disabled,
	"•ÒW",
	{	/* array: 6 elements */
		/* [1] */
		"ƒJƒbƒg", noIcon, "X", noMark, plain,
		/* [2] */
		"ƒRƒs[", noIcon, "C", noMark, plain,
		/* [3] */
		"ƒy[ƒXƒg", noIcon, "V", noMark, plain,
		/* [4] */
		"Á‹", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"‚·‚×‚Ä‚ğ‘I‘ğ", noIcon, "A", noMark, plain
	}
};

resource 'MENU' (IDM_EMULATE) {
	IDM_EMULATE,
	textMenuProc,
	0x1D,
	enabled,
	"ƒGƒ~ƒ…ƒŒ[ƒg",
	{	/* array: 5 elements */
		/* [1] */
		"ƒŠƒZƒbƒg", noIcon, "R", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"İ’è...", noIcon, ";", noMark, plain,
		/* [4] */
		"V‹KƒfƒBƒXƒN...", noIcon, "N", noMark, plain,
		/* [5] */
		"ƒtƒHƒ“ƒg‘I‘ğ...", noIcon, noKey, noMark, plain
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
	"ƒfƒoƒCƒX",
	{	/* array: 11 elements */
		/* [1] */
		"ƒL[ƒ{[ƒh", noIcon, hierarchicalMenu, "‹", plain,
		/* [2] */
		"ƒTƒEƒ“ƒh", noIcon, hierarchicalMenu, "Œ", plain,
		/* [3] */
		"ƒƒ‚ƒŠ", noIcon, hierarchicalMenu, "", plain,
		/* [4] */
		"ƒ}ƒEƒX", noIcon, "M", noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"ƒVƒŠƒAƒ‹ƒIƒvƒVƒ‡ƒ“...", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"MIDI İ’è...", noIcon, noKey, noMark, plain,
		/* [9] */
		"MIDI Á‰¹", noIcon, noKey, noMark, plain,
		/* [10] */
		"-", noIcon, noKey, noMark, plain,
		/* [11] */
		"ƒTƒEƒ“ƒhİ’è...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SCREEN) {
	IDM_SCREEN,
	textMenuProc,
	0xBFDD,
	enabled,
	"ƒXƒNƒŠ[ƒ“",
	{	/* array: 16 elements */
		/* [1] */
		"ƒtƒ‹ƒXƒNƒŠ[ƒ“", noIcon, "F", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"’Êí", noIcon, noKey, noMark, plain,
		/* [4] */
		"¶90“x‰ñ“]", noIcon, noKey, noMark, plain,
		/* [5] */
		"‰E90“x‰ñ“]", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain,
		/* [7] */
		"VSync•\\¦", noIcon, noKey, noMark, plain,
		/* [8] */
		"Real Palettes", noIcon, noKey, noMark, plain,
		/* [9] */
		"ƒEƒFƒCƒg–³‚µ", noIcon, noKey, noMark, plain,
		/* [10] */
		"ƒI[ƒgƒtƒŒ[ƒ€", noIcon, noKey, noMark, plain,
		/* [11] */
		"ƒtƒ‹ƒtƒŒ[ƒ€", noIcon, noKey, noMark, plain,
		/* [12] */
		"1/2 ƒtƒŒ[ƒ€", noIcon, noKey, noMark, plain,
		/* [13] */
		"1/3 ƒtƒŒ[ƒ€", noIcon, noKey, noMark, plain,
		/* [14] */
		"1/4 ƒtƒŒ[ƒ€", noIcon, noKey, noMark, plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"ƒXƒNƒŠ[ƒ“İ’è...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_HELP) {
	IDM_HELP,
	textMenuProc,
	0x5,
	enabled,
	"ƒwƒ‹ƒv",
	{	/* array: 1 elements */
		/* [1] */
		"‚Ë‚±[ƒvƒƒWƒFƒNƒgIIx ƒwƒ‹ƒv...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SOUND) {
	IDM_SOUND,
	textMenuProc,
	0x2FFEF,
	enabled,
	"ƒTƒEƒ“ƒh",
	{	/* array: 18 elements */
		/* [1] */
		"ƒr[ƒvƒIƒt", noIcon, noKey, noMark, plain,
		/* [2] */
		"ƒr[ƒv‰¹—Ê¬", noIcon, noKey, noMark, plain,
		/* [3] */
		"ƒr[ƒv‰¹—Ê’†", noIcon, noKey, noMark, plain,
		/* [4] */
		"ƒr[ƒv‰¹—Ê‘å", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"ƒTƒEƒ“ƒhƒ{[ƒh•sg—p", noIcon, noKey, noMark, plain,
		/* [7] */
		"PC-9801-14", noIcon, noKey, noMark, plain,
		/* [8] */
		"PC-9801-26K", noIcon, noKey, noMark, plain,
		/* [9] */
		"PC-9801-86", noIcon, noKey, noMark, plain,
		/* [10] */
		"PC-9801-26 + 86", noIcon, noKey, noMark, plain,
		/* [11] */
		"PC-9801-86 + ‚¿‚Ñ‚¨‚Æ", noIcon, noKey, noMark, plain,
		/* [12] */
		"PC-9801-118", noIcon, noKey, noMark, plain,
		/* [13] */
		"ƒXƒs[ƒNƒ{[ƒh", noIcon, noKey, noMark, plain,
		/* [14] */
		"ƒXƒp[ƒNƒ{[ƒh", noIcon, noKey, noMark, plain,
		/* [15] */
		"AMD98", noIcon, noKey, noMark, plain,
		/* [16] */
		"JAST SOUND", noIcon, noKey, noMark, plain,
		/* [17] */
		"-", noIcon, noKey, noMark, plain,
		/* [18] */
		"ƒV[ƒN‰¹", noIcon, noKey, noMark, plain
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
		"ŠJ‚­...", noIcon, "O", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"æ‚èŠO‚µ", noIcon, noKey, noMark, plain
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
		"ŠJ‚­...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"æ‚èŠO‚µ", noIcon, noKey, noMark, plain
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
		"ŠJ‚­...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"æ‚èŠO‚µ", noIcon, noKey, noMark, plain
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
		"ŠJ‚­...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"æ‚èŠO‚µ", noIcon, noKey, noMark, plain
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
		"ŠJ‚­...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"æ‚èŠO‚µ", noIcon, noKey, noMark, plain
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
		"ŠJ‚­...", noIcon, "O", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"æ‚èŠO‚µ", noIcon, noKey, noMark, plain
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
		"ŠJ‚­...", noIcon, "D", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"æ‚èo‚µ", noIcon, "E", noMark, plain
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
		"SASI-1", noIcon, hierarchicalMenu, "‡", plain,
		/* [2] */
		"SASI-2", noIcon, hierarchicalMenu, "ˆ", plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain,
		/* [4] */
		"SCSI #0", noIcon, hierarchicalMenu, "‘", plain,
		/* [5] */
		"SCSI #1", noIcon, hierarchicalMenu, "’", plain,
		/* [6] */
		"SCSI #2", noIcon, hierarchicalMenu, "“", plain,
		/* [7] */
		"SCSI #3", noIcon, hierarchicalMenu, "”", plain
	}
	/****** Extra bytes follow... ******/
	/* $"0000"                                               /* .. */
};

resource 'MENU' (IDM_KEYBOARD) {
	IDM_KEYBOARD,
	textMenuProc,
	0xFBEEF,
	enabled,
	"ƒL[ƒ{[ƒh",
	{	/* array: 20 elements */
		/* [1] */
		"ƒL[ƒ{[ƒh", noIcon, "K", noMark, plain,
		/* [2] */
		"JoyKey-1", noIcon, "J", noMark, plain,
		/* [3] */
		"JoyKey-2", noIcon, noKey, noMark, plain,
		/* [4] */
		"MouseKey", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"ƒƒJƒjƒJƒ‹ SHIFT", noIcon, noKey, noMark, plain,
		/* [7] */
		"ƒƒJƒjƒJƒ‹ CTRL", noIcon, noKey, noMark, plain,
		/* [8] */
		"ƒƒJƒjƒJƒ‹ GRPH", noIcon, noKey, noMark, plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"F11 = ƒJƒi", noIcon, noKey, noMark, plain,
		/* [11] */
		"F11 = Stop", noIcon, noKey, noMark, plain,
		/* [12] */
		"F11 = ƒeƒ“ƒL[ [=]", noIcon, noKey, noMark, plain,
		/* [13] */
		"F11 = NFER", noIcon, noKey, noMark, plain,
		/* [14] */
		"F11 = ƒ†[ƒU[1", noIcon, noKey, noMark, plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"F12 = ƒ}ƒEƒX", noIcon, noKey, noMark, plain,
		/* [17] */
		"F12 = Copy", noIcon, noKey, noMark, plain,
		/* [18] */
		"F12 = ƒeƒ“ƒL[ [,]", noIcon, noKey, noMark, plain,
		/* [19] */
		"F12 = XFER", noIcon, noKey, noMark, plain,
		/* [20] */
		"F12 = ƒ†[ƒU[2", noIcon, noKey, noMark, plain
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
		"‚Ë‚±[ƒvƒƒWƒFƒNƒgIIx‚É‚Â‚¢‚Ä...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_OTHER) {
	IDM_OTHER,
	textMenuProc,
	0xEFF,
	enabled,
	"‚»‚Ì‘¼",
	{	/* array: 12 elements */
		/* [1] */
		"BMP ƒZ[ƒu...", noIcon, noKey, noMark, plain,
		/* [2] */
		"S98 ƒƒO...", noIcon, noKey, noMark, plain,
		/* [3] */
		"ƒJƒŒƒ“ƒ_[İ’è...", noIcon, noKey, noMark, plain,
		/* [4] */
		"ƒNƒƒbƒN•\\¦", noIcon, noKey, noMark, plain,
		/* [5] */
		"FPS•\\¦", noIcon, noKey, noMark, plain,
		/* [6] */
		"ƒWƒ‡ƒCƒpƒbƒhƒ{ƒ^ƒ“”½“]", noIcon, noKey, noMark, plain,
		/* [7] */
		"ƒWƒ‡ƒCƒpƒbƒhƒ{ƒ^ƒ“˜AË", noIcon, noKey, noMark, plain,
		/* [8] */
		"ƒ}ƒEƒXƒ{ƒ^ƒ“˜AË", noIcon, noKey, noMark, plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"ƒTƒEƒ“ƒh˜^‰¹...", noIcon, "S", noMark, plain,
		/* [11] */
		"ƒL[ƒfƒBƒXƒvƒŒƒC", noIcon, noKey, noMark, plain,
		/* [12] */
		"ƒ\\ƒtƒgƒEƒFƒAƒL[ƒ{[ƒh", noIcon, noKey, noMark, plain
	}
};
