/**
 * @file	sddsound3.h
 * @brief	DSound3 オーディオ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include <map>
#include <dsound.h>
#include "sdbase.h"
#include "misc\threadbase.h"

/**
 * @brief Direct Sound3 クラス
 */
class CSoundDeviceDSound3 : public CSoundDeviceBase, protected CThreadBase
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
	virtual bool LoadPCM(UINT nNum, LPCTSTR lpFilename = NULL);
	virtual void SetPCMVolume(UINT nNum, int nVolume);
	virtual bool PlayPCM(UINT nNum, BOOL bLoop);
	virtual void StopPCM(UINT nNum);
	virtual void StopAllPCM();

protected:
	virtual bool Task();

private:
	LPDIRECTSOUND m_lpDSound;					//!< Direct Sound インタフェイス
	LPDIRECTSOUNDBUFFER m_lpDSStream;			//!< ストリーム バッファ
	UINT m_nChannels;							//!< チャネル数
	UINT m_nBufferSize;							//!< バッファ サイズ
	UINT m_dwHalfBufferSize;					//!< バッファ バイト
	HANDLE m_hEvents[2];						//!< イベント
	std::map<UINT, LPDIRECTSOUNDBUFFER> m_pcm;	//!< PCM バッファ

private:
	void FillStream(DWORD dwPosition);
	void UnloadPCM(UINT nNum);
	void DestroyAllPCM();
	LPDIRECTSOUNDBUFFER CreateWaveBuffer(LPCTSTR lpFilename);
};
