/**
 * @file	cmmidiout.h
 * @brief	MIDI OUT ���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief MIDI OUT ���N���X
 */
class CComMidiOut
{
public:
	/**
	 * �f�X�g���N�^
	 */
	virtual ~CComMidiOut()
	{
	}

	/**
	 * �V���[�g ���b�Z�[�W
	 */
	virtual void Short(UINT32 nMessage)
	{
	}

	/**
	 * �����O ���b�Z�[�W
	 */
	virtual void Long(const UINT8* lpMessage, UINT cbMessage)
	{
	}
};
