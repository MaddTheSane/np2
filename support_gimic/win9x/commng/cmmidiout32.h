/**
 * @file	cmmidiout32.h
 * @brief	MIDI OUT win32 �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>
#include "cmmidiout.h"

/**
 * @brief MIDI OUT win32 �N���X
 */
class CComMidiOut32 : public CComMidiOut
{
public:
	static CComMidiOut32* CreateInstance(LPCTSTR lpMidiOut);

	CComMidiOut32(HMIDIOUT hMidiOut);
	virtual ~CComMidiOut32();
	virtual void Short(UINT32 nMessage);
	virtual void Long(const UINT8* lpMessage, UINT cbMessage);
	static bool GetId(LPCTSTR lpMidiOut, UINT* pId);

private:
	HMIDIOUT m_hMidiOut;
	bool m_bWaitingSentExclusive;
	MIDIHDR m_midihdr;
	std::vector<char> m_excvbuf;

	void WaitSentExclusive();
};
