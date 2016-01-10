/**
 * @file	cmmidioutvst.h
 * @brief	MIDI OUT VST �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "cmmidiout.h"
#include "sound.h"
#include "vsthost\vstbuffer.h"
#include "vsthost\vsteffect.h"
#include "vsthost\vsteffectwnd.h"
#include "vsthost\vstmidievent.h"

/**
 * @brief MIDI OUT VST �N���X
 */
class CComMidiOutVst : public CComMidiOut
{
public:
	static CComMidiOutVst* CreateInstance();

	CComMidiOutVst();
	virtual ~CComMidiOutVst();
	virtual void Short(UINT32 nMessage);
	virtual void Long(const UINT8* lpMessage, UINT cbMessage);

private:
	UINT m_nBlockSize;
	UINT m_nIndex;
	CVstEffect m_effect;
	CVstEffectWnd m_wnd;
	CVstMidiEvent m_event;
	CVstBuffer m_input;
	CVstBuffer m_output;

	bool Initialize(LPCTSTR lpPath);
	static void SOUNDCALL GetPcm(CComMidiOutVst*, SINT32* lpBuffer, UINT nBufferCount);
	void Process(SINT32* lpBuffer, UINT nBufferCount);
};
