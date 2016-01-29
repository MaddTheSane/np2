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
	bool Initialize(HWND hWnd);
	void Deinitialize();
	UINT CreateStream(UINT rate, UINT ms);
	void ResetStream();
	void DestroyStream();
	void PlayStream();
	void StopStream();
	void Sync();
	void LoadPCM(UINT nNum, LPCTSTR lpFilename);
	void UnloadPCM(UINT nNum);
	void SetPCMVolume(UINT nNum, int nVolume);
	bool PlayPCM(UINT nNum, BOOL bLoop);
	void StopPCM(UINT nNum);
	void StopAllPCM();

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
