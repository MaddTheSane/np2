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
		"�J��...", noIcon, "D", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"���o��", noIcon, "E", noMark, plain
	}
};

resource 'MENU' (IDM_EDIT) {
	IDM_EDIT,
	textMenuProc,
	0x1AF,
	disabled,
	"�ҏW",
	{	/* array: 6 elements */
		/* [1] */
		"�J�b�g", noIcon, "X", noMark, plain,
		/* [2] */
		"�R�s�[", noIcon, "C", noMark, plain,
		/* [3] */
		"�y�[�X�g", noIcon, "V", noMark, plain,
		/* [4] */
		"����", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"���ׂĂ�I��", noIcon, "A", noMark, plain
	}
};

resource 'MENU' (IDM_EMULATE) {
	IDM_EMULATE,
	textMenuProc,
	0x1D,
	enabled,
	"�G�~�����[�g",
	{	/* array: 5 elements */
		/* [1] */
		"���Z�b�g", noIcon, "R", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"�ݒ�...", noIcon, ";", noMark, plain,
		/* [4] */
		"�V�K�f�B�X�N...", noIcon, "N", noMark, plain,
		/* [5] */
		"�t�H���g�I��...", noIcon, noKey, noMark, plain
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
	"�f�o�C�X",
	{	/* array: 11 elements */
		/* [1] */
		"�L�[�{�[�h", noIcon, hierarchicalMenu, "�", plain,
		/* [2] */
		"�T�E���h", noIcon, hierarchicalMenu, "�", plain,
		/* [3] */
		"������", noIcon, hierarchicalMenu, "�", plain,
		/* [4] */
		"�}�E�X", noIcon, "M", noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"�V���A���I�v�V����...", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"MIDI �ݒ�...", noIcon, noKey, noMark, plain,
		/* [9] */
		"MIDI ����", noIcon, noKey, noMark, plain,
		/* [10] */
		"-", noIcon, noKey, noMark, plain,
		/* [11] */
		"�T�E���h�ݒ�...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SCREEN) {
	IDM_SCREEN,
	textMenuProc,
	0xBFDD,
	enabled,
	"�X�N���[��",
	{	/* array: 16 elements */
		/* [1] */
		"�t���X�N���[��", noIcon, "F", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"�ʏ�", noIcon, noKey, noMark, plain,
		/* [4] */
		"��90�x��]", noIcon, noKey, noMark, plain,
		/* [5] */
		"�E90�x��]", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain,
		/* [7] */
		"VSync�\\��", noIcon, noKey, noMark, plain,
		/* [8] */
		"Real Palettes", noIcon, noKey, noMark, plain,
		/* [9] */
		"�E�F�C�g����", noIcon, noKey, noMark, plain,
		/* [10] */
		"�I�[�g�t���[��", noIcon, noKey, noMark, plain,
		/* [11] */
		"�t���t���[��", noIcon, noKey, noMark, plain,
		/* [12] */
		"1/2 �t���[��", noIcon, noKey, noMark, plain,
		/* [13] */
		"1/3 �t���[��", noIcon, noKey, noMark, plain,
		/* [14] */
		"1/4 �t���[��", noIcon, noKey, noMark, plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"�X�N���[���ݒ�...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_HELP) {
	IDM_HELP,
	textMenuProc,
	0x5,
	enabled,
	"�w���v",
	{	/* array: 1 elements */
		/* [1] */
		"�˂��[�v���W�F�N�gIIx �w���v...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_SOUND) {
	IDM_SOUND,
	textMenuProc,
	0x2FFEF,
	enabled,
	"�T�E���h",
	{	/* array: 18 elements */
		/* [1] */
		"�r�[�v�I�t", noIcon, noKey, noMark, plain,
		/* [2] */
		"�r�[�v���ʏ�", noIcon, noKey, noMark, plain,
		/* [3] */
		"�r�[�v���ʒ�", noIcon, noKey, noMark, plain,
		/* [4] */
		"�r�[�v���ʑ�", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"�T�E���h�{�[�h�s�g�p", noIcon, noKey, noMark, plain,
		/* [7] */
		"PC-9801-14", noIcon, noKey, noMark, plain,
		/* [8] */
		"PC-9801-26K", noIcon, noKey, noMark, plain,
		/* [9] */
		"PC-9801-86", noIcon, noKey, noMark, plain,
		/* [10] */
		"PC-9801-26 + 86", noIcon, noKey, noMark, plain,
		/* [11] */
		"PC-9801-86 + ���т���", noIcon, noKey, noMark, plain,
		/* [12] */
		"PC-9801-118", noIcon, noKey, noMark, plain,
		/* [13] */
		"�X�s�[�N�{�[�h", noIcon, noKey, noMark, plain,
		/* [14] */
		"�X�p�[�N�{�[�h", noIcon, noKey, noMark, plain,
		/* [15] */
		"AMD98", noIcon, noKey, noMark, plain,
		/* [16] */
		"JAST SOUND", noIcon, noKey, noMark, plain,
		/* [17] */
		"-", noIcon, noKey, noMark, plain,
		/* [18] */
		"�V�[�N��", noIcon, noKey, noMark, plain
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
		"�J��...", noIcon, "O", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"���O��", noIcon, noKey, noMark, plain
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
		"�J��...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"���O��", noIcon, noKey, noMark, plain
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
		"�J��...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"���O��", noIcon, noKey, noMark, plain
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
		"�J��...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"���O��", noIcon, noKey, noMark, plain
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
		"�J��...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"���O��", noIcon, noKey, noMark, plain
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
		"�J��...", noIcon, "O", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"���O��", noIcon, noKey, noMark, plain
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
		"�J��...", noIcon, "D", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"���o��", noIcon, "E", noMark, plain
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
	"�L�[�{�[�h",
	{	/* array: 20 elements */
		/* [1] */
		"�L�[�{�[�h", noIcon, "K", noMark, plain,
		/* [2] */
		"JoyKey-1", noIcon, "J", noMark, plain,
		/* [3] */
		"JoyKey-2", noIcon, noKey, noMark, plain,
		/* [4] */
		"MouseKey", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"���J�j�J�� SHIFT", noIcon, noKey, noMark, plain,
		/* [7] */
		"���J�j�J�� CTRL", noIcon, noKey, noMark, plain,
		/* [8] */
		"���J�j�J�� GRPH", noIcon, noKey, noMark, plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"F11 = �J�i", noIcon, noKey, noMark, plain,
		/* [11] */
		"F11 = Stop", noIcon, noKey, noMark, plain,
		/* [12] */
		"F11 = �e���L�[ [=]", noIcon, noKey, noMark, plain,
		/* [13] */
		"F11 = NFER", noIcon, noKey, noMark, plain,
		/* [14] */
		"F11 = ���[�U�[1", noIcon, noKey, noMark, plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"F12 = �}�E�X", noIcon, noKey, noMark, plain,
		/* [17] */
		"F12 = Copy", noIcon, noKey, noMark, plain,
		/* [18] */
		"F12 = �e���L�[ [,]", noIcon, noKey, noMark, plain,
		/* [19] */
		"F12 = XFER", noIcon, noKey, noMark, plain,
		/* [20] */
		"F12 = ���[�U�[2", noIcon, noKey, noMark, plain
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
		"�˂��[�v���W�F�N�gIIx�ɂ���...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (IDM_OTHER) {
	IDM_OTHER,
	textMenuProc,
	0xEFF,
	enabled,
	"���̑�",
	{	/* array: 12 elements */
		/* [1] */
		"BMP �Z�[�u...", noIcon, noKey, noMark, plain,
		/* [2] */
		"S98 ���O...", noIcon, noKey, noMark, plain,
		/* [3] */
		"�J�����_�[�ݒ�...", noIcon, noKey, noMark, plain,
		/* [4] */
		"�N���b�N�\\��", noIcon, noKey, noMark, plain,
		/* [5] */
		"FPS�\\��", noIcon, noKey, noMark, plain,
		/* [6] */
		"�W���C�p�b�h�{�^�����]", noIcon, noKey, noMark, plain,
		/* [7] */
		"�W���C�p�b�h�{�^���A��", noIcon, noKey, noMark, plain,
		/* [8] */
		"�}�E�X�{�^���A��", noIcon, noKey, noMark, plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"�T�E���h�^��...", noIcon, "S", noMark, plain,
		/* [11] */
		"�L�[�f�B�X�v���C", noIcon, noKey, noMark, plain,
		/* [12] */
		"�\\�t�g�E�F�A�L�[�{�[�h", noIcon, noKey, noMark, plain
	}
};
