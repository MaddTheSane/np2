/**
 * @file	sddsound3.h
 * @brief	DSound3 �I�[�f�B�I �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <map>
#include <dsound.h>
#include "sdbase.h"

/**
 * @brief Direct Sound3 �N���X
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
	virtual bool LoadPCM(UINT nNum, LPCTSTR lpFilename = NULL);
	virtual void SetPCMVolume(UINT nNum, int nVolume);
	virtual bool PlayPCM(UINT nNum, BOOL bLoop);
	virtual void StopPCM(UINT nNum);
	virtual void StopAllPCM();

private:
	LPDIRECTSOUND m_lpDSound;					//!< Direct Sound �C���^�t�F�C�X
	LPDIRECTSOUNDBUFFER m_lpDSStream;			//!< �X�g���[�� �o�b�t�@
	UINT m_dwHalfBufferSize;					//!< �o�b�t�@ �T�C�Y
	int m_nStreamEvent;							//!< �X�g���[�� �C�x���g
	std::map<UINT, LPDIRECTSOUNDBUFFER> m_pcm;	//!< PCM �o�b�t�@

private:
	void FillStream(DWORD dwPosition);
	void UnloadPCM(UINT nNum);
	void DestroyAllPCM();
	LPDIRECTSOUNDBUFFER CreateWaveBuffer(LPCTSTR lpFilename);
};