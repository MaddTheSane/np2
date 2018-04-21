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
		"開く...", noIcon, "D", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"取り出し", noIcon, "E", noMark, plain
	}
};

resource 'MENU' (IDM_EDIT) {
	IDM_EDIT,
	textMenuProc,
	0x1AF,
	disabled,
	"編集",
	{	/* array: 6 elements */
		/* [1] */
		"カット", noIcon, "X", noMark, plain,
		/* [2] */
		"コピー", noIcon, "C", noMark, plain,
		/* [3] */
		"ペースト", noIcon, "V", noMark, plain,
		/* [4] */
		"消去", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"すべてを選択", noIcon, "A", noMark, plain
	}
};

resource 'MENU' (IDM_EMULATE) {
	IDM_EMULATE,
	textMenuProc,
	0x1D,
	enabled,
	"エミュレート",
	{	/* array: 5 elements */
		/* [1] */
		"リセット", noIcon, "R", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"設定...", noIcon, ";", noMark, plain,
		/* [4] */
		"新規ディスク...", noIcon, "N", noMark, plain,
		/* [5] */
		"フォント選択...", noIcon, noKey, noMark, plain
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
	"デバイス",
	{	/* array: 11 elements */
		/* [1] */
		"キーボード", noIcon, hierarchicalMenu, "�", plain,
		/* [2] */
		"サウンド", noIcon, hierarchicalMenu, "�", plain,
		/* [3] */
		"メモリ", noIcon, hierarchicalMenu, "�", plain,
		/* [4] */
		"マウス", noIcon, "M", noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"シリアルオプション...", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"MIDI 設定...", noIcon, noKey, noMark, plain,
		/* [9] */
		"MIDI 消音", noIcon, noKey, noMark, plain,
		/* [10] */
		"-", noIcon, noKey, noMark, plain,
		/* [11] */
		"サウンド設定...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SCREEN) {
	IDM_SCREEN,
	textMenuProc,
	0xBFDD,
	enabled,
	"スクリーン",
	{	/* array: 16 elements */
		/* [1] */
		"フルスクリーン", noIcon, "F", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"通常", noIcon, noKey, noMark, plain,
		/* [4] */
		"左90度回転", noIcon, noKey, noMark, plain,
		/* [5] */
		"右90度回転", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain,
		/* [7] */
		"VSync表\示", noIcon, noKey, noMark, plain,
		/* [8] */
		"Real Palettes", noIcon, noKey, noMark, plain,
		/* [9] */
		"ウェイト無し", noIcon, noKey, noMark, plain,
		/* [10] */
		"オートフレーム", noIcon, noKey, noMark, plain,
		/* [11] */
		"フルフレーム", noIcon, noKey, noMark, plain,
		/* [12] */
		"1/2 フレーム", noIcon, noKey, noMark, plain,
		/* [13] */
		"1/3 フレーム", noIcon, noKey, noMark, plain,
		/* [14] */
		"1/4 フレーム", noIcon, noKey, noMark, plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"スクリーン設定...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_HELP) {
	IDM_HELP,
	textMenuProc,
	0x5,
	enabled,
	"ヘルプ",
	{	/* array: 1 elements */
		/* [1] */
		"ねこープロジェクトIIx ヘルプ...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SOUND) {
	IDM_SOUND,
	textMenuProc,
	0x2FFEF,
	enabled,
	"サウンド",
	{	/* array: 18 elements */
		/* [1] */
		"ビープオフ", noIcon, noKey, noMark, plain,
		/* [2] */
		"ビープ音量小", noIcon, noKey, noMark, plain,
		/* [3] */
		"ビープ音量中", noIcon, noKey, noMark, plain,
		/* [4] */
		"ビープ音量大", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"サウンドボード不使用", noIcon, noKey, noMark, plain,
		/* [7] */
		"PC-9801-14", noIcon, noKey, noMark, plain,
		/* [8] */
		"PC-9801-26K", noIcon, noKey, noMark, plain,
		/* [9] */
		"PC-9801-86", noIcon, noKey, noMark, plain,
		/* [10] */
		"PC-9801-26 + 86", noIcon, noKey, noMark, plain,
		/* [11] */
		"PC-9801-86 + ちびおと", noIcon, noKey, noMark, plain,
		/* [12] */
		"PC-9801-118", noIcon, noKey, noMark, plain,
		/* [13] */
		"スピークボード", noIcon, noKey, noMark, plain,
		/* [14] */
		"スパークボード", noIcon, noKey, noMark, plain,
		/* [15] */
		"AMD98", noIcon, noKey, noMark, plain,
		/* [16] */
		"JAST SOUND", noIcon, noKey, noMark, plain,
		/* [17] */
		"-", noIcon, noKey, noMark, plain,
		/* [18] */
		"シーク音", noIcon, noKey, noMark, plain
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
		"開く...", noIcon, "O", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"取り外し", noIcon, noKey, noMark, plain
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
		"開く...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"取り外し", noIcon, noKey, noMark, plain
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
		"開く...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"取り外し", noIcon, noKey, noMark, plain
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
		"開く...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"取り外し", noIcon, noKey, noMark, plain
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
		"開く...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"取り外し", noIcon, noKey, noMark, plain
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
		"開く...", noIcon, "O", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"取り外し", noIcon, noKey, noMark, plain
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
		"開く...", noIcon, "D", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"取り出し", noIcon, "E", noMark, plain
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
		"SASI-1", noIcon, hierarchicalMenu, "�", plain,
		/* [2] */
		"SASI-2", noIcon, hierarchicalMenu, "�", plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain,
		/* [4] */
		"SCSI #0", noIcon, hierarchicalMenu, "�", plain,
		/* [5] */
		"SCSI #1", noIcon, hierarchicalMenu, "�", plain,
		/* [6] */
		"SCSI #2", noIcon, hierarchicalMenu, "�", plain,
		/* [7] */
		"SCSI #3", noIcon, hierarchicalMenu, "�", plain
	}
	/****** Extra bytes follow... ******/
	/* $"0000"                                               /* .. */
};

resource 'MENU' (IDM_KEYBOARD) {
	IDM_KEYBOARD,
	textMenuProc,
	0xFBEEF,
	enabled,
	"キーボード",
	{	/* array: 20 elements */
		/* [1] */
		"キーボード", noIcon, "K", noMark, plain,
		/* [2] */
		"JoyKey-1", noIcon, "J", noMark, plain,
		/* [3] */
		"JoyKey-2", noIcon, noKey, noMark, plain,
		/* [4] */
		"MouseKey", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"メカニカル SHIFT", noIcon, noKey, noMark, plain,
		/* [7] */
		"メカニカル CTRL", noIcon, noKey, noMark, plain,
		/* [8] */
		"メカニカル GRPH", noIcon, noKey, noMark, plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"F11 = カナ", noIcon, noKey, noMark, plain,
		/* [11] */
		"F11 = Stop", noIcon, noKey, noMark, plain,
		/* [12] */
		"F11 = テンキー [=]", noIcon, noKey, noMark, plain,
		/* [13] */
		"F11 = NFER", noIcon, noKey, noMark, plain,
		/* [14] */
		"F11 = ユーザー1", noIcon, noKey, noMark, plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"F12 = マウス", noIcon, noKey, noMark, plain,
		/* [17] */
		"F12 = Copy", noIcon, noKey, noMark, plain,
		/* [18] */
		"F12 = テンキー [,]", noIcon, noKey, noMark, plain,
		/* [19] */
		"F12 = XFER", noIcon, noKey, noMark, plain,
		/* [20] */
		"F12 = ユーザー2", noIcon, noKey, noMark, plain
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
		"ねこープロジェクトIIxについて...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_OTHER) {
	IDM_OTHER,
	textMenuProc,
	0xEFF,
	enabled,
	"その他",
	{	/* array: 12 elements */
		/* [1] */
		"BMP セーブ...", noIcon, noKey, noMark, plain,
		/* [2] */
		"S98 ログ...", noIcon, noKey, noMark, plain,
		/* [3] */
		"カレンダー設定...", noIcon, noKey, noMark, plain,
		/* [4] */
		"クロック表\示", noIcon, noKey, noMark, plain,
		/* [5] */
		"FPS表\示", noIcon, noKey, noMark, plain,
		/* [6] */
		"ジョイパッドボタン反転", noIcon, noKey, noMark, plain,
		/* [7] */
		"ジョイパッドボタン連射", noIcon, noKey, noMark, plain,
		/* [8] */
		"マウスボタン連射", noIcon, noKey, noMark, plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"サウンド録音...", noIcon, "S", noMark, plain,
		/* [11] */
		"キーディスプレイ", noIcon, noKey, noMark, plain,
		/* [12] */
		"ソ\フトウェアキーボード", noIcon, noKey, noMark, plain
	}
};
