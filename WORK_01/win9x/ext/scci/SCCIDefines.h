//----------------------------------------------------------------------
//	SCCI Sound Interfaces defines
//----------------------------------------------------------------------
#pragma once

// Sound chip list
enum SC_CHIP_TYPE {
	SC_TYPE_NONE	= 0,
	SC_TYPE_YM2608,
	SC_TYPE_YM2151,
	SC_TYPE_YM2610,
	SC_TYPE_YM2203,
	SC_TYPE_YM2612,
	SC_TYPE_AY8910,
	SC_TYPE_SN76489,
	SC_TYPE_YM3812,
	SC_TYPE_YMF262,
	SC_TYPE_YM2413,
	SC_TYPE_YM3526,
	SC_TYPE_YMF288,
	SC_TYPE_SCC,
	SC_TYPE_SCCS,
	SC_TYPE_Y8950,
	SC_TYPE_YM2164,		// OPP:OPM�Ƃ̓n�[�h�E�F�ALFO�̐��䂪�Ⴄ
	SC_TYPE_YM2414,		// OPZ:OPM�ƃs���R���p�`
	SC_TYPE_AY8930,		// APSG:�g��PSG
	SC_TYPE_YM2149,		// SSG:PSG�Ƃ�DAC���Ⴄ(YM3439�Ƃ͓���Ƃ݂Ă����Ǝv��)
	SC_TYPE_YMZ294,		// SSGL:SSG�Ƃ�DAC���Ⴄ(YMZ284�Ƃ͓���Ƃ݂Ă����Ǝv��)
	SC_TYPE_SN76496,	// DCSG:76489�Ƃ̓m�C�Y�W�F�l���[�^�̐��������Ⴄ
	SC_TYPE_YM2420,		// OPLL2:OPLL�Ƃ�Fnum�̐ݒ���@���Ⴄ�B���͓����B
	SC_TYPE_YMF281,		// OPLLP:OPLL�Ƃ͓���ROM���F���Ⴄ�B����͓����B
	SC_TYPE_YMF276,		// OPN2L:OPN2/OPN2C�Ƃ�DAC���Ⴄ
	SC_TYPE_YM2610B,	// OPNB-B:OPNB�Ƃ�FM����ch�����Ⴄ�B
	SC_TYPE_YMF286,		// OPNB-C:OPNB�Ƃ�DAC���Ⴄ�B
	SC_TYPE_YM2602,		// 315-5124: 76489/76496�Ƃ̓m�C�Y�W�F�l���[�^�̐��������Ⴄ�BPOWON���ɔ��U���Ȃ��B
	SC_TYPE_UM3567,		// OPLL�̃R�s�[�i�i������DIP24�Ȃ̂ł��̂܂܃��v���[�X�ł��Ȃ��j
	SC_TYPE_YMF274,		// OPL4:���얢��
	SC_TYPE_YM3806,		// OPQ:����\��
	SC_TYPE_YM2163,		// DSG:���쒆
	SC_TYPE_YM7129,		// OPK2:���쒆
	SC_TYPE_YMZ280,		// PCM8:ADPCM8ch:����\��
	SC_TYPE_YMZ705,		// SSGS:SSG*2set+ADPCM8ch:���쒆
	SC_TYPE_YMZ735,		// FMS:FM8ch+ADPCM8ch:���쒆
	SC_TYPE_YM2423,		// YM2413�̉��F�Ⴂ
	SC_TYPE_SPC700,		// SPC700
	SC_TYPE_NBV4,		// NBV4�p
	SC_TYPE_AYB02,		// AYB02�p
	SC_TYPE_8253,		// i8253�i�y�ь݊��`�b�v�p�j
	SC_TYPE_OTHER,		// ���̑��f�o�C�X�p�A�A�h���X��A0-A3�œ��삷��
	SC_TYPE_UNKNOWN,	// �J���f�o�C�X����
	SC_TYPE_MAX
};

// Sound chip clock list
enum SC_CHIP_CLOCK {
	SC_CLOCK_NONE		= 0,
	SC_CLOCK_1789773	= 1789773,	// SSG,OPN,OPM,SN76489 etc
	SC_CLOCK_1996800	= 1996800,	// SSG,OPN,OPM,SN76489 etc
	SC_CLOCK_2000000	= 2000000,	// SSG,OPN,OPM,SN76489 etc
	SC_CLOCK_2048000	= 2048000,	// SSGLP(4096/2|6144/3)
	SC_CLOCK_3579545	= 3579545,	// SSG,OPN,OPM,SN76489 etc
	SC_CLOCK_3993600	= 3993600,	// OPN(88)
	SC_CLOCK_4000000	= 4000000,	// SSF,OPN,OPM etc
	SC_CLOCK_7159090	= 7159090,	// OPN,OPNA,OPNB,OPN2,OPN3L etc
	SC_CLOCK_7670454	= 7670454,	// YM-2612 etc
	SC_CLOCK_7987200	= 7987200,	// OPNA(88)
	SC_CLOCK_8000000	= 8000000,	// OPNB etc
	SC_CLOCK_12500000	= 12500000, // RF5C164
	SC_CLOCK_14318180	= 14318180, // OPL2
	SC_CLOCK_16934400	= 16934400, // YMF271
	SC_CLOCK_23011361	= 23011361, // PWM
};

// Sound chip location
enum SC_CHIP_LOCATION {
	SC_LOCATION_MONO	= 0,
	SC_LOCATION_LEFT	= 1,
	SC_LOCATION_RIGHT	= 2,
	SC_LOCATION_STEREO	= 3
};

#define	SC_WAIT_REG			(0xffffffff)	// �E�F�C�ƃR�}���h���M�i�f�[�^�͑��M����R�}���h���j
#define SC_FLUSH_REG		(0xfffffffe)	// �������݃f�[�^�t���b�V���҂�
#define SC_DIRECT_BUS		(0x80000000)	// �A�h���X�o�X�_�C���N�g���[�h

