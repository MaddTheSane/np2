/**
 *	@file	joymng.cpp
 *	@brief	�W���C�p�b�h���͂̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "np2.h"
#include "joymng.h"
#include "keystat.h"

#if !defined(__GNUC__)
#pragma comment(lib, "winmm.lib")
#endif	// !defined(__GNUC__)

/**
 * �r�b�g��`
 */
enum
{
	JOY_LEFT_BIT	= 0x04,		//!< ��
	JOY_RIGHT_BIT	= 0x08,		//!< �E
	JOY_UP_BIT		= 0x01,		//!< ��
	JOY_DOWN_BIT	= 0x02,		//!< ��
	JOY_BTN1_BIT	= 0x10,		//!< �{�^��1
	JOY_BTN2_BIT	= 0x20		//!< �{�^��2
};

static REG8 s_sFlag = 0xff;					//!< �p�b�h���
static UINT8 s_sJoyPad1ButtonBit[4];		//!< �p�b�h�Ɋ��蓖�Ă��{�^�� �r�b�g

/**
 * ������
 */
void joymng_initialize()
{
	JOYINFO ji;
	if ((!::joyGetNumDevs()) || (::joyGetPos(JOYSTICKID1, &ji) == JOYERR_UNPLUGGED))
	{
		np2oscfg.JOYPAD1 |= 2;
	}
	for (int i = 0; i < 4; i++)
	{
		s_sJoyPad1ButtonBit[i] = 0xff ^ ((np2oscfg.JOY1BTN[i] & 3) << ((np2oscfg.JOY1BTN[i] & 4) ? 4 : 6));
	}
}

/**
 * ����
 */
void joymng_sync()
{
	np2oscfg.JOYPAD1 &= 0x7f;
	s_sFlag = 0xff;
}

/**
 * ��ԓǂݍ���
 * @return ���
 */
REG8 joymng_getstat()
{
	JOYINFO ji;
	if ((np2oscfg.JOYPAD1 == 1) && (::joyGetPos(JOYSTICKID1, &ji) == JOYERR_NOERROR))
	{
		np2oscfg.JOYPAD1 |= 0x80;
		s_sFlag = 0xff;
		if (ji.wXpos < 0x4000U)
		{
			s_sFlag &= ~JOY_LEFT_BIT;
		}
		else if (ji.wXpos > 0xc000U)
		{
			s_sFlag &= ~JOY_RIGHT_BIT;
		}
		if (ji.wYpos < 0x4000U)
		{
			s_sFlag &= ~JOY_UP_BIT;
		}
		else if (ji.wYpos > 0xc000U)
		{
			s_sFlag &= ~JOY_DOWN_BIT;
		}
		if (ji.wButtons & JOY_BUTTON1)
		{
			s_sFlag &= s_sJoyPad1ButtonBit[0];
		}
		if (ji.wButtons & JOY_BUTTON2)
		{
			s_sFlag &= s_sJoyPad1ButtonBit[1];
		}
#if 0
		if (ji.wButtons & JOY_BUTTON3)
		{
			s_sFlag &= s_sJoyPad1ButtonBit[2];
		}
		if (ji.wButtons & JOY_BUTTON4)
		{
			s_sFlag &= s_sJoyPad1ButtonBit[3];
		}
#else
		static const UINT8 s_key[2] = {0xff, 0xff};
		for (int i = 0; i < 2; i++)
		{
			const UINT8 data = s_key[i];
			if (data == 0xff)
			{
				continue;
			}
			keystat_sync(data, 0xf6, (ji.wButtons & (JOY_BUTTON3 << i)) ? TRUE : FALSE);
		}
#endif
	}
	return s_sFlag;
}

// s_sFlag	bit:0		up
// 			bit:1		down
// 			bit:2		left
// 			bit:3		right
// 			bit:4		trigger1 (rapid)
// 			bit:5		trigger2 (rapid)
// 			bit:6		trigger1
// 			bit:7		trigger2

