/**
 * @file	cmmidi.h
 * @brief	MIDI クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "cmbase.h"

extern const TCHAR cmmidi_midimapper[];
#if defined(VERMOUTH_LIB)
extern const TCHAR cmmidi_vermouth[];
#endif
#if defined(MT32SOUND_DLL)
extern const TCHAR cmmidi_mt32sound[];
#endif
extern LPCTSTR cmmidi_mdlname[12];

void cmmidi_initailize(void);
void cmmidi_recvdata(HMIDIIN hdr, UINT32 data);
void cmmidi_recvexcv(HMIDIIN hdr, MIDIHDR *data);

struct _cmmidi;
class CComMidiOut;

/**
 * @brief commng MIDI デバイス クラス
 */
class CComMidi : public CComBase
{
public:
	static CComMidi* CreateInstance(LPCTSTR lpMidiOut, LPCTSTR lpMidiIn, LPCTSTR lpModule);

protected:
	CComMidi();
	virtual ~CComMidi();
	virtual UINT Read(UINT8* pData);
	virtual UINT Write(UINT8 cData);
	virtual UINT8 GetStat();
	virtual INTPTR Message(UINT msg, INTPTR param);

private:
	_cmmidi* m_pMidi;

	bool Initialize(LPCTSTR lpMidiOut, LPCTSTR lpMidiIn, LPCTSTR lpModule);
};
