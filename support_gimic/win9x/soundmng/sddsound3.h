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
	LPDIRECTSOUND m_lpDSound;					//!< Direct Sound �C���^�t�F�C�X
	LPDIRECTSOUNDBUFFER m_lpDSStream;			//!< �X�g���[�� �o�b�t�@
	UINT m_dwHalfBufferSize;					//!< �o�b�t�@ �T�C�Y
	int m_nStreamEvent;							//!< �X�g���[�� �C�x���g
	std::map<UINT, LPDIRECTSOUNDBUFFER> m_pcm;	//!< PCM �o�b�t�@

private:
	void FillStream(DWORD dwPosition);
	void DestroyAllPCM();
	LPDIRECTSOUNDBUFFER CreateWaveBuffer(LPCTSTR lpFilename);
};
