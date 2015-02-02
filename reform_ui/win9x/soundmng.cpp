/**
 * @file	soundmng.cpp
 * @brief	Sound Manager (DirectSound3)
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/03/07 09:54:11 $
 */

#include "compiler.h"
#include <map>
#include <dsound.h>
#include "parts.h"
#include "wavefile.h"
#include "np2.h"
#include "soundmng.h"
#include "misc\extrom.h"
#include "sound.h"
#if defined(SUPPORT_ROMEO)
#include "devices\juliet.h"
#endif
#if defined(VERMOUTH_LIB)
#include "vermouth.h"
#endif
#if defined(MT32SOUND_DLL)
#include "mt32snd.h"
#endif

#if !defined(__GNUC__)
#pragma comment(lib, "dsound.lib")
#endif	// !defined(__GNUC__)

#if defined(_M_IA64) || defined(_M_AMD64)
#define	SOUNDBUFFERALIGN	(1 << 3)
#else
#define	SOUNDBUFFERALIGN	(1 << 2)
#endif

#if !defined(_WIN64)
#ifdef __cplusplus
extern "C"
{
#endif
void __fastcall satuation_s16mmx(SINT16 *dst, const SINT32 *src, UINT size);
#ifdef __cplusplus
}
#endif
#endif

#if 1
#define DSBUFFERDESC_SIZE	20			// DirectX3 Structsize
#else
#define DSBUFFERDESC_SIZE	sizeof(DSBUFFERDESC)
#endif

#ifndef DSBVOLUME_MAX
#define DSBVOLUME_MAX		0
#endif
#ifndef DSBVOLUME_MIN
#define DSBVOLUME_MIN		(-10000)
#endif

/**
 * @brief Direct Sound3 �N���X
 */
class CDSound3
{
public:
	static CDSound3* GetInstance();

	CDSound3();
	bool Initialize(HWND hWnd);
	void Deinitialize();

	UINT CreateStream(UINT rate, UINT ms);
	void ResetStream();
	void DestroyStream();
	void PlayStream();
	void StopStream();
	void Sync();
	void SetReverse(bool bReverse);
	void LoadPCM(SoundPCMNumber nNum, LPCTSTR lpFilename);
	void UnloadPCM(SoundPCMNumber nNum);
	void SetPCMVolume(SoundPCMNumber nNum, int nVolume);
	bool PlayPCM(SoundPCMNumber nNum, DWORD dwFlags);
	void StopPCM(SoundPCMNumber nNum);
	void EnableSound(SoundProc nProc);
	void DisableSound(SoundProc nProc);

private:
	static CDSound3 sm_instance;									//!< �B��̃C���X�^���X�ł�

	LPDIRECTSOUND m_lpDSound;										//!< Direct Sound �C���^�t�F�C�X
	UINT m_nMute;													//!< �~���[�g �t���O
	LPDIRECTSOUNDBUFFER m_lpDSStream;								//!< �X�g���[�� �o�b�t�@
	UINT m_dwHalfBufferSize;										//!< �o�b�t�@ �T�C�Y
	int m_nStreamEvent;												//!< �X�g���[�� �C�x���g
	std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER> m_pcm;			//!< PCM �o�b�t�@
	void (PARTSCALL * m_fnMix)(SINT16*, const SINT32*, UINT);		//!< �R�[���o�b�N

private:
	void EnableStream(bool bEnable);
	void FillStream(DWORD dwPosition);
	void DestroyAllPCM();
	void StopAllPCM();
	LPDIRECTSOUNDBUFFER CreateWaveBuffer(LPCTSTR lpFilename);
};

#if defined(VERMOUTH_LIB)
	MIDIMOD		vermouth_module = NULL;
#endif

//! �B��̃C���X�^���X�ł�
CDSound3 CDSound3::sm_instance;

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
CDSound3* CDSound3::GetInstance()
{
	return &sm_instance;
}

/**
 * �R���X�g���N�^
 */
CDSound3::CDSound3()
	: m_lpDSound(NULL)
	, m_nMute(0)
	, m_lpDSStream(NULL)
	, m_dwHalfBufferSize(0)
	, m_nStreamEvent(-1)
{
}

/**
 * ������
 * @param[in] hWnd �E�B���h�E �n���h��
 * @retval true ����
 * @retval false ���s
 */
bool CDSound3::Initialize(HWND hWnd)
{
	// DirectSound�̏�����
	LPDIRECTSOUND lpDSound;
	if (FAILED(DirectSoundCreate(0, &lpDSound, 0)))
	{
		return false;
	}
	if (FAILED(lpDSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
	{
		if (FAILED(lpDSound->SetCooperativeLevel(hWnd, DSSCL_NORMAL)))
		{
			lpDSound->Release();
			return false;
		}
	}

	m_lpDSound = lpDSound;

#if defined(MT32SOUND_DLL)
	mt32sound_initialize();
#endif
	return true;
}

/**
 * ���
 */
void CDSound3::Deinitialize()
{
#if defined(MT32SOUND_DLL)
	mt32sound_deinitialize();
#endif
	DestroyAllPCM();
	DestroyStream();

	RELEASE(m_lpDSound);
}

/**
 * �T�E���h�L��
 * @param[in] nProc �v���V�[�W��
 */
void CDSound3::EnableSound(SoundProc nProc)
{
	const UINT nBit = 1 << nProc;
	if (!(m_nMute & nBit))
	{
		return;
	}
	m_nMute &= ~nBit;
	if (!m_nMute)
	{
		ResetStream();
		EnableStream(true);
	}
}

/**
 * �T�E���h����
 * @param[in] nProc �v���V�[�W��
 */
void CDSound3::DisableSound(SoundProc nProc)
{
	if (!m_nMute)
	{
		EnableStream(false);
		StopAllPCM();
	}
	m_nMute |= (1 << nProc);
}

/**
 * �X�g���[�����쐬
 * @param[in] rate �T���v�����O ���[�g
 * @param[in] ms �o�b�t�@��(�~���b)
 * @return �o�b�t�@��
 */
UINT CDSound3::CreateStream(UINT rate, UINT ms)
{
	if ((m_lpDSound == NULL) || ((rate != 11025) && (rate != 22050) && (rate != 44100)))
	{
		return 0;
	}

	if (ms < 40) {
		ms = 40;
	}
	else if (ms > 1000) {
		ms = 1000;
	}

	// �L�[�{�[�h�\���̃f�B���C�ݒ�
//	keydispr_delayinit((UINT8)((ms * 10 + 563) / 564));

	UINT samples = (rate * ms) / 2000;
	samples = (samples + SOUNDBUFFERALIGN - 1) & (~(SOUNDBUFFERALIGN - 1));
	m_dwHalfBufferSize = samples * 2 * sizeof(SINT16);
	SetReverse(false);

	PCMWAVEFORMAT pcmwf;
	ZeroMemory(&pcmwf, sizeof(pcmwf));
	pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
	pcmwf.wf.nChannels = 2;
	pcmwf.wf.nSamplesPerSec = rate;
	pcmwf.wBitsPerSample = 16;
	pcmwf.wf.nBlockAlign = 2 * sizeof(SINT16);
	pcmwf.wf.nAvgBytesPerSec = rate * 2 * sizeof(SINT16);

	int i;
	for (i = 0; i < 2; i++)
	{
		DSBUFFERDESC dsbdesc;
		ZeroMemory(&dsbdesc, sizeof(dsbdesc));
		dsbdesc.dwSize = i ? sizeof(dsbdesc) : DSBUFFERDESC_SIZE;
		dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
						DSBCAPS_CTRLFREQUENCY |
						DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;
		dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;
		dsbdesc.dwBufferBytes = m_dwHalfBufferSize * 2;
		if (SUCCEEDED(m_lpDSound->CreateSoundBuffer(&dsbdesc, &m_lpDSStream, NULL)))
		{
			break;
		}
	}
	if (i >= 2)
	{
		return 0;
	}

#if defined(VERMOUTH_LIB)
	vermouth_module = midimod_create(rate);
	midimod_loadall(vermouth_module);
#endif

#if defined(MT32SOUND_DLL)
	mt32sound_setrate(rate);
#endif

	m_nStreamEvent = -1;
	return samples;
}

/**
 * �X�g���[�������Z�b�g
 */
void CDSound3::ResetStream()
{
	if (m_lpDSStream)
	{
		LPVOID lpBlock1;
		DWORD cbBlock1;
		LPVOID lpBlock2;
		DWORD cbBlock2;
		if (SUCCEEDED(m_lpDSStream->Lock(0, m_dwHalfBufferSize * 2, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0)))
		{
			ZeroMemory(lpBlock1, cbBlock1);
			if ((lpBlock2) && (cbBlock2))
			{
				ZeroMemory(lpBlock2, cbBlock2);
			}
			m_lpDSStream->Unlock(lpBlock1, cbBlock1, lpBlock2, cbBlock2);
			m_lpDSStream->SetCurrentPosition(0);
			m_nStreamEvent = -1;
		}
	}
}

/**
 * �X�g���[����j��
 */
void CDSound3::DestroyStream()
{
	if (m_lpDSStream)
	{
#if defined(VERMOUTH_LIB)
		midimod_destroy(vermouth_module);
		vermouth_module = NULL;
#endif
#if defined(MT32SOUND_DLL)
		mt32sound_setrate(0);
#endif
		m_lpDSStream->Stop();
		m_lpDSStream->Release();
		m_lpDSStream = NULL;
	}
}

/**
 * �X�g���[���L���ݒ�
 * @param[in] bEnable �L���t���O
 */
void CDSound3::EnableStream(bool bEnable)
{
	if (m_lpDSStream)
	{
		if (bEnable)
		{
			m_lpDSStream->Play(0, 0, DSBPLAY_LOOPING);
		}
		else
		{
			m_lpDSStream->Stop();
		}
	}

#if defined(SUPPORT_ROMEO)
	CJuliet::GetInstance()->Mute(!bEnable);
#endif
}

/**
 * �X�g���[���̍Đ�
 */
void CDSound3::PlayStream()
{
	if (!m_nMute)
	{
		EnableStream(true);
	}
}

/**
 * �X�g���[���̒�~
 */
void CDSound3::StopStream()
{
	if (!m_nMute)
	{
		EnableStream(false);
	}
}

/**
 * �X�g���[�����X�V����
 * @param[in] dwPosition �X�V�ʒu
 */
void CDSound3::FillStream(DWORD dwPosition)
{
	const SINT32* lpSource= ::sound_pcmlock();

	LPVOID lpBlock1;
	DWORD cbBlock1;
	LPVOID lpBlock2;
	DWORD cbBlock2;
	HRESULT hr = m_lpDSStream->Lock(dwPosition, m_dwHalfBufferSize, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0);
	if (hr == DSERR_BUFFERLOST)
	{
		m_lpDSStream->Restore();
		hr = m_lpDSStream->Lock(dwPosition, m_dwHalfBufferSize, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0);
	}
	if (SUCCEEDED(hr))
	{
		if (lpSource)
		{
			(*m_fnMix)(static_cast<SINT16*>(lpBlock1), lpSource, cbBlock1);
		}
		else
		{
			ZeroMemory(lpBlock1, cbBlock1);
		}
		m_lpDSStream->Unlock(lpBlock1, cbBlock1, lpBlock2, cbBlock2);
	}
	::sound_pcmunlock(lpSource);
}

/**
 * ����
 */
void CDSound3::Sync()
{
	if (m_lpDSStream)
	{
		DWORD dwCurrentPlayCursor;
		DWORD dwCurrentWriteCursor;
		if (m_lpDSStream->GetCurrentPosition(&dwCurrentPlayCursor, &dwCurrentWriteCursor) == DS_OK)
		{
			if (dwCurrentPlayCursor >= m_dwHalfBufferSize)
			{
				if (m_nStreamEvent != 0)
				{
					m_nStreamEvent = 0;
					FillStream(0);
				}
			}
			else
			{
				if (m_nStreamEvent != 1)
				{
					m_nStreamEvent = 1;
					FillStream(m_dwHalfBufferSize);
				}
			}
		}
	}
}

/**
 * �p�����]��ݒ肷��
 * @param[in] bReverse ���]�t���O
 */
void CDSound3::SetReverse(bool bReverse)
{
	if (!bReverse)
	{
#if !defined(_WIN64)
		if (mmxflag)
		{
			m_fnMix = satuation_s16;
		}
		else {
			m_fnMix = satuation_s16mmx;
		}
#else
		m_fnMix = satuation_s16;
#endif
	}
	else
	{
		m_fnMix = satuation_s16x;
	}
}

/**
 * PCM �o�b�t�@��j������
 */
void CDSound3::DestroyAllPCM()
{
	for (std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.begin(); it != m_pcm.begin(); ++it)
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->Stop();
		lpDSBuffer->Release();
	}
	m_pcm.clear();
}

/**
 * PCM ���X�g�b�v
 */
void CDSound3::StopAllPCM()
{
	for (std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.begin(); it != m_pcm.begin(); ++it)
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->Stop();
	}
}

/**
 * PCM �f�[�^�ǂݍ���
 * @param[in] nNum PCM �ԍ�
 * @param[in] lpFilename �t�@�C����
 */
void CDSound3::LoadPCM(SoundPCMNumber nNum, LPCTSTR lpFilename)
{
	UnloadPCM(nNum);

	LPDIRECTSOUNDBUFFER lpDSBuffer = CreateWaveBuffer(lpFilename);
	if (lpDSBuffer)
	{
		m_pcm[nNum] = lpDSBuffer;
	}
}

/**
 * PCM �f�[�^�ǂݍ���
 * @param[in] lpFilename �t�@�C����
 * @return �o�b�t�@
 */
LPDIRECTSOUNDBUFFER CDSound3::CreateWaveBuffer(LPCTSTR lpFilename)
{
	LPDIRECTSOUNDBUFFER lpDSBuffer = NULL;
	CExtRom extrom;

	do
	{
		if (!extrom.Open(lpFilename))
		{
			break;
		}

		RIFF_HEADER riff;
		if (extrom.Read(&riff, sizeof(riff)) != sizeof(riff))
		{
			break;
		}
		if ((riff.sig != WAVE_SIG('R','I','F','F')) || (riff.fmt != WAVE_SIG('W','A','V','E')))
		{
			break;
		}

		bool bValid = false;
		WAVE_HEADER whead;
		WAVE_INFOS info;
		UINT nSize = 0;
		while (true /*CONSTCOND*/)
		{
			if (extrom.Read(&whead, sizeof(whead)) != sizeof(whead))
			{
				bValid = false;
				break;
			}
			nSize = LOADINTELDWORD(whead.size);
			if (whead.sig == WAVE_SIG('f','m','t',' '))
			{
				if (nSize >= sizeof(info))
				{
					if (extrom.Read(&info, sizeof(info)) != sizeof(info))
					{
						bValid = false;
						break;
					}
					nSize -= sizeof(info);
					bValid = true;
				}
			}
			else if (whead.sig == WAVE_SIG('d','a','t','a'))
			{
				break;
			}
			if (nSize)
			{
				extrom.Seek(nSize, FILE_CURRENT);
			}
		}
		if (!bValid)
		{
			break;
		}

		PCMWAVEFORMAT pcmwf;
		ZeroMemory(&pcmwf, sizeof(pcmwf));
		pcmwf.wf.wFormatTag = LOADINTELWORD(info.format);
		if (pcmwf.wf.wFormatTag != 1)
		{
			break;
		}
		pcmwf.wf.nChannels = LOADINTELWORD(info.channel);
		pcmwf.wf.nSamplesPerSec = LOADINTELDWORD(info.rate);
		pcmwf.wBitsPerSample = LOADINTELWORD(info.bit);
		pcmwf.wf.nBlockAlign = LOADINTELWORD(info.block);
		pcmwf.wf.nAvgBytesPerSec = LOADINTELDWORD(info.rps);

		DSBUFFERDESC dsbdesc;
		ZeroMemory(&dsbdesc, sizeof(dsbdesc));
		dsbdesc.dwSize = DSBUFFERDESC_SIZE;
		dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC | DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;
		dsbdesc.dwBufferBytes = nSize;
		dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;

		if (FAILED(m_lpDSound->CreateSoundBuffer(&dsbdesc, &lpDSBuffer, NULL)))
		{
			break;
		}

		LPVOID lpBlock1;
		DWORD cbBlock1;
		LPVOID lpBlock2;
		DWORD cbBlock2;
		HRESULT hr = lpDSBuffer->Lock(0, nSize, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0);
		if (hr == DSERR_BUFFERLOST)
		{
			lpDSBuffer->Restore();
			hr = lpDSBuffer->Lock(0, nSize, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0);
		}
		if (FAILED(hr))
		{
			lpDSBuffer->Release();
			lpDSBuffer = NULL;
			break;
		}

		extrom.Read(lpBlock1, cbBlock1);
		if ((lpBlock2) && (cbBlock2))
		{
			extrom.Read(lpBlock2, cbBlock2);
		}
		lpDSBuffer->Unlock(lpBlock1, cbBlock1, lpBlock2, cbBlock2);
	} while (0 /*CONSTCOND*/);

	return lpDSBuffer;
}

/**
 * PCM ���A�����[�h
 * @param[in] nNum PCM �ԍ�
 */
void CDSound3::UnloadPCM(SoundPCMNumber nNum)
{
	std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
	if (it != m_pcm.end())
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		m_pcm.erase(it);

		lpDSBuffer->Stop();
		lpDSBuffer->Release();
	}
}

/**
 * PCM ���H�����[���ݒ�
 * @param[in] nNum PCM �ԍ�
 * @param[in] nVolume ���H�����[��
 */
void CDSound3::SetPCMVolume(SoundPCMNumber nNum, int nVolume)
{
	std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
	if (it != m_pcm.end())
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->SetVolume((((DSBVOLUME_MAX - DSBVOLUME_MIN) * nVolume) / 100) + DSBVOLUME_MIN);
	}
}

/**
 * PCM �Đ�
 * @param[in] nNum PCM �ԍ�
 * @param[in] dwFlags �Đ��t���O
 * @retval true ����
 * @retval false ���s
 */
bool CDSound3::PlayPCM(SoundPCMNumber nNum, DWORD dwFlags)
{
	if (!m_nMute)
	{
		std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
		if (it != m_pcm.end())
		{
			LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
//			lpDSBuffer->SetCurrentPosition(0);
			lpDSBuffer->Play(0, 0, dwFlags);
			return true;
		}
	}
	return false;
}

/**
 * PCM ��~
 * @param[in] nNum PCM �ԍ�
 */
void CDSound3::StopPCM(SoundPCMNumber nNum)
{
	if (!m_nMute)
	{
		std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
		if (it != m_pcm.end())
		{
			LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
			lpDSBuffer->Stop();
		}
	}
}

// ---- C ���b�p�[

/**
 * �X�g���[���쐬
 * @param[in] rate �T���v�����O ���[�g
 * @param[in] ms �o�b�t�@��(�~���b)
 * @return �o�b�t�@ �T�C�Y
 */
UINT soundmng_create(UINT rate, UINT ms)
{
	return CDSound3::GetInstance()->CreateStream(rate, ms);
}

/**
 * �X�g���[�� ���Z�b�g
 */
void soundmng_reset(void)
{
	CDSound3::GetInstance()->ResetStream();
}

/**
 * �X�g���[���j��
 */
void soundmng_destroy(void)
{
	CDSound3::GetInstance()->DestroyStream();
}

/**
 * �X�g���[���J�n
 */
void soundmng_play(void)
{
	CDSound3::GetInstance()->PlayStream();
}

/**
 * �X�g���[����~
 */
void soundmng_stop(void)
{
	CDSound3::GetInstance()->StopStream();
}

/**
 * �X�g���[������
 */
void soundmng_sync(void)
{
	CDSound3::GetInstance()->Sync();
}

/**
 * �X�g���[�� �p�����]�ݒ�
 * @param[in] bReverse ���]
 */
void soundmng_setreverse(BOOL bReverse)
{
	CDSound3::GetInstance()->SetReverse((bReverse) ? true : false);
}

/**
 * PCM ���[�h
 * @param[in] nNum PCM �ԍ�
 * @param[in] lpFilename �t�@�C����
 */
void soundmng_pcmload(SoundPCMNumber nNum, LPCTSTR lpFilename)
{
	CDSound3::GetInstance()->LoadPCM(nNum, lpFilename);
}

/**
 * PCM ���H�����[���ݒ�
 * @param[in] nNum PCM �ԍ�
 * @param[in] nVolume ���H�����[��
 */
void soundmng_pcmvolume(SoundPCMNumber nNum, int nVolume)
{
	CDSound3::GetInstance()->SetPCMVolume(nNum, nVolume);
}

/**
 * PCM �Đ�
 * @param[in] nNum PCM �ԍ�
 * @param[in] bLoop ���[�v
 * @retval SUCCESS ����
 * @retval FAILURE ���s
 */
BRESULT soundmng_pcmplay(enum SoundPCMNumber nNum, BOOL bLoop)
{
	return (CDSound3::GetInstance()->PlayPCM(nNum, (bLoop) ? DSBPLAY_LOOPING : 0)) ? SUCCESS : FAILURE;
}

/**
 * PCM ��~
 * @param[in] nNum PCM �ԍ�
 */
void soundmng_pcmstop(enum SoundPCMNumber nNum)
{
	CDSound3::GetInstance()->StopPCM(nNum);
}

/**
 * ������
 * @retval SUCCESS ����
 * @retval FAILURE ���s
 */
BRESULT soundmng_initialize(void)
{
	return (CDSound3::GetInstance()->Initialize(g_hWndMain)) ? SUCCESS : FAILURE;
}

/**
 * ���
 */
void soundmng_deinitialize(void)
{
	CDSound3::GetInstance()->Deinitialize();
}

/**
 * �T�E���h�L��
 * @param[in] nProc �v���V�[�W��
 */
void soundmng_enable(SoundProc nProc)
{
	CDSound3::GetInstance()->EnableSound(nProc);
}

/**
 * �T�E���h����
 * @param[in] nProc �v���V�[�W��
 */
void soundmng_disable(SoundProc nProc)
{
	CDSound3::GetInstance()->DisableSound(nProc);
}
