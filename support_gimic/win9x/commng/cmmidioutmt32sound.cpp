/**
 * @file	cmmidioutmt32sound.cpp
 * @brief	MIDI OUT MT32Sound クラスの動作の定義を行います
 */

#include "compiler.h"
#include "cmmidioutmt32sound.h"

#if defined(MT32SOUND_DLL)

#include "ext\mt32snd.h"

/**
 * インスタンスを作成
 */
CComMidiOutMT32Sound* CComMidiOutMT32Sound::CreateInstance()
{
	MT32Sound* pMT32Sound = MT32Sound::GetInstance();
	if (!pMT32Sound->Open())
	{
		return NULL;
	}
	return new CComMidiOutMT32Sound(pMT32Sound);
}

/**
 * コンストラクタ
 */
CComMidiOutMT32Sound::CComMidiOutMT32Sound(MT32Sound* pMT32Sound)
	: m_pMT32Sound(pMT32Sound)
{
	::sound_streamregist(m_pMT32Sound, reinterpret_cast<SOUNDCB>(GetPcm));
}

CComMidiOutMT32Sound::~CComMidiOutMT32Sound()
{
	m_pMT32Sound->Close();
}

void CComMidiOutMT32Sound::Short(UINT32 nMessage)
{
	sound_sync();
	m_pMT32Sound->ShortMsg(nMessage);
}

void CComMidiOutMT32Sound::Long(const UINT8* lpMessage, UINT cbMessage)
{
	sound_sync();
	m_pMT32Sound->LongMsg(lpMessage, cbMessage);
}

void SOUNDCALL CComMidiOutMT32Sound::GetPcm(MT32Sound* pMT32Sound, SINT32* lpBuffer, UINT nBufferCount)
{
	pMT32Sound->Mix(lpBuffer, nBufferCount);
}

#endif	// defined(MT32SOUND_DLL)
