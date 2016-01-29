/**
 * @file	sddsound3.h
 * @brief	DSound3 オーディオ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include <map>
#include <dsound.h>
#include "sdbase.h"

/**
 * @brief Direct Sound3 クラス
 */
class CSoundDeviceDSound3 : public CSoundDeviceBase
{
public:
	CSoundDeviceDSound3();
	virtual ~CSoundDeviceDSound3();
	virtual bool Open(LPCTSTR lpDevice = NULL, HWND hWnd = NULL);
	virtual void Close();
	virtual UINT CreateStream(UINT nSamplingRate, UINT nChannels, UINT nBufferSize = 0);
	virtual void DestroyStream();
	virtual void ResetStream();
	virtual bool PlayStream();
	virtual void StopStream();
	virtual void SyncStream();
	virtual bool LoadPCM(UINT nNum, LPCTSTR lpFilename);
	virtual void UnloadPCM(UINT nNum);
	virtual void SetPCMVolume(UINT nNum, int nVolume);
	virtual bool PlayPCM(UINT nNum, BOOL bLoop);
	virtual void StopPCM(UINT nNum);
	virtual void StopAllPCM();

private:
	LPDIRECTSOUND m_lpDSound;					//!< Direct Sound インタフェイス
	LPDIRECTSOUNDBUFFER m_lpDSStream;			//!< ストリーム バッファ
	UINT m_dwHalfBufferSize;					//!< バッファ サイズ
	int m_nStreamEvent;							//!< ストリーム イベント
	std::map<UINT, LPDIRECTSOUNDBUFFER> m_pcm;	//!< PCM バッファ

private:
	void FillStream(DWORD dwPosition);
	void DestroyAllPCM();
	LPDIRECTSOUNDBUFFER CreateWaveBuffer(LPCTSTR lpFilename);
};
