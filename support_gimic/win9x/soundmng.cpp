/**
 * @file	soundmng.cpp
 * @brief	Sound Manager (DirectSound3)
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/03/07 09:54:11 $
 */

#include "compiler.h"
#include "soundmng.h"
#include "np2.h"
#if defined(SUPPORT_ROMEO)
#include "ext\externalchipmanager.h"
#endif
#if defined(MT32SOUND_DLL)
#include "ext\mt32snd.h"
#endif
#include "soundmng\sdbase.h"
#include "soundmng\sddsound3.h"
#include "common\parts.h"
#include "sound\sound.h"
#if defined(VERMOUTH_LIB)
#include "sound\vermouth\vermouth.h"
#endif

#if !defined(_WIN64)
#ifdef __cplusplus
extern "C"
{
#endif
/**
 * satuation
 * @param[out] dst �o�̓o�b�t�@
 * @param[in] src ���̓o�b�t�@
 * @param[in] size �T�C�Y
 */
void __fastcall satuation_s16mmx(SINT16 *dst, const SINT32 *src, UINT size);
#ifdef __cplusplus
}
#endif
#endif

/**
 * @brief �T�E���h �}�l�[�W�� �N���X
 */
class CSoundMng : public ISoundData
{
public:
	static CSoundMng* GetInstance();

	CSoundMng();
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
	bool PlayPCM(SoundPCMNumber nNum, BOOL bLoop);
	void StopPCM(SoundPCMNumber nNum);
	void EnableSound(SoundProc nProc);
	void DisableSound(SoundProc nProc);
	virtual UINT Get16(SINT16* lpBuffer, UINT nBufferCount);

private:
	static CSoundMng sm_instance;									//!< �B��̃C���X�^���X�ł�

	typedef void (PARTSCALL * FNMIX)(SINT16*, const SINT32*, UINT);	//!< satuation�֐��^�錾
	FNMIX m_fnMix;													//!< satuation�֐��|�C���^
	UINT m_nMute;													//!< �~���[�g �t���O
	CSoundDeviceDSound3 m_dsound3;									//!< Direct Sound
};

#if defined(VERMOUTH_LIB)
	MIDIMOD		vermouth_module = NULL;
#endif

//! �B��̃C���X�^���X�ł�
CSoundMng CSoundMng::sm_instance;

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline CSoundMng* CSoundMng::GetInstance()
{
	return &sm_instance;
}

/**
 * �R���X�g���N�^
 */
CSoundMng::CSoundMng()
	: m_nMute(0)
{
	SetReverse(false);
}

/**
 * ������
 * @param[in] hWnd �E�B���h�E �n���h��
 * @retval true ����
 * @retval false ���s
 */
inline bool CSoundMng::Initialize(HWND hWnd)
{
	if (!m_dsound3.Initialize(hWnd))
	{
		return false;
	}

#if defined(MT32SOUND_DLL)
	MT32Sound::GetInstance()->Initialize();
#endif
	return true;
}

/**
 * ���
 */
inline void CSoundMng::Deinitialize()
{
	m_dsound3.Deinitialize();

#if defined(MT32SOUND_DLL)
	MT32Sound::GetInstance()->Deinitialize();
#endif
}

/**
 * �T�E���h�L��
 * @param[in] nProc �v���V�[�W��
 */
void CSoundMng::EnableSound(SoundProc nProc)
{
	const UINT nBit = 1 << nProc;
	if (!(m_nMute & nBit))
	{
		return;
	}
	m_nMute &= ~nBit;
	if (!m_nMute)
	{
		m_dsound3.ResetStream();
		m_dsound3.PlayStream();
	}
}

/**
 * �T�E���h����
 * @param[in] nProc �v���V�[�W��
 */
void CSoundMng::DisableSound(SoundProc nProc)
{
	if (!m_nMute)
	{
		m_dsound3.StopStream();
		m_dsound3.StopAllPCM();
	}
	m_nMute |= (1 << nProc);
}

/**
 * �X�g���[�����쐬
 * @param[in] rate �T���v�����O ���[�g
 * @param[in] ms �o�b�t�@��(�~���b)
 * @return �o�b�t�@��
 */
UINT CSoundMng::CreateStream(UINT rate, UINT ms)
{
	const UINT nBuffer = m_dsound3.CreateStream(rate, ms);
	if (nBuffer == 0)
	{
		return 0;
	}

#if defined(VERMOUTH_LIB)
	vermouth_module = midimod_create(rate);
	midimod_loadall(vermouth_module);
#endif

#if defined(MT32SOUND_DLL)
	MT32Sound::GetInstance()->SetRate(rate);
#endif

	m_dsound3.SetStreamData(this);

	return nBuffer;
}

/**
 * �X�g���[�������Z�b�g
 */
inline void CSoundMng::ResetStream()
{
	m_dsound3.ResetStream();
}

/**
 * �X�g���[����j��
 */
void CSoundMng::DestroyStream()
{
	m_dsound3.DestroyStream();

#if defined(VERMOUTH_LIB)
	midimod_destroy(vermouth_module);
	vermouth_module = NULL;
#endif
#if defined(MT32SOUND_DLL)
	MT32Sound::GetInstance()->SetRate(0);
#endif
}

/**
 * �X�g���[���̍Đ�
 */
void CSoundMng::PlayStream()
{
	if (!m_nMute)
	{
		m_dsound3.PlayStream();

#if defined(SUPPORT_ROMEO)
		CExternalChipManager::GetInstance()->Mute(false);
#endif
	}
}

/**
 * �X�g���[���̒�~
 */
void CSoundMng::StopStream()
{
	if (!m_nMute)
	{
		m_dsound3.StopStream();

#if defined(SUPPORT_ROMEO)
		CExternalChipManager::GetInstance()->Mute(true);
#endif
	}
}

/**
 * �X�g���[���𓾂�
 * @param[out] lpBuffer �o�b�t�@
 * @param[in] nBufferCount �o�b�t�@ �J�E���g
 * @return �T���v����
 */
UINT CSoundMng::Get16(SINT16* lpBuffer, UINT nBufferCount)
{
	const SINT32* lpSource = ::sound_pcmlock();
	if (lpSource)
	{
		(*m_fnMix)(lpBuffer, lpSource, nBufferCount * 4);
		::sound_pcmunlock(lpSource);
		return nBufferCount;
	}
	else
	{
		return 0;
	}
}

/**
 * ����
 */
inline void CSoundMng::Sync()
{
	m_dsound3.Sync();
}

/**
 * �p�����]��ݒ肷��
 * @param[in] bReverse ���]�t���O
 */
void CSoundMng::SetReverse(bool bReverse)
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
 * PCM �f�[�^�ǂݍ���
 * @param[in] nNum PCM �ԍ�
 * @param[in] lpFilename �t�@�C����
 */
inline void CSoundMng::LoadPCM(SoundPCMNumber nNum, LPCTSTR lpFilename)
{
	m_dsound3.LoadPCM(nNum, lpFilename);
}

/**
 * PCM ���A�����[�h
 * @param[in] nNum PCM �ԍ�
 */
inline void CSoundMng::UnloadPCM(SoundPCMNumber nNum)
{
	m_dsound3.UnloadPCM(nNum);
}

/**
 * PCM ���H�����[���ݒ�
 * @param[in] nNum PCM �ԍ�
 * @param[in] nVolume ���H�����[��
 */
inline void CSoundMng::SetPCMVolume(SoundPCMNumber nNum, int nVolume)
{
	m_dsound3.SetPCMVolume(nNum, nVolume);
}

/**
 * PCM �Đ�
 * @param[in] nNum PCM �ԍ�
 * @param[in] bLoop ���[�v �t���O
 * @retval true ����
 * @retval false ���s
 */
inline bool CSoundMng::PlayPCM(SoundPCMNumber nNum, BOOL bLoop)
{
	if (!m_nMute)
	{
		return m_dsound3.PlayPCM(nNum, bLoop);
	}
	return false;
}

/**
 * PCM ��~
 * @param[in] nNum PCM �ԍ�
 */
inline void CSoundMng::StopPCM(SoundPCMNumber nNum)
{
	m_dsound3.StopPCM(nNum);
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
	return CSoundMng::GetInstance()->CreateStream(rate, ms);
}

/**
 * �X�g���[�� ���Z�b�g
 */
void soundmng_reset(void)
{
	CSoundMng::GetInstance()->ResetStream();
}

/**
 * �X�g���[���j��
 */
void soundmng_destroy(void)
{
	CSoundMng::GetInstance()->DestroyStream();
}

/**
 * �X�g���[���J�n
 */
void soundmng_play(void)
{
	CSoundMng::GetInstance()->PlayStream();
}

/**
 * �X�g���[����~
 */
void soundmng_stop(void)
{
	CSoundMng::GetInstance()->StopStream();
}

/**
 * �X�g���[������
 */
void soundmng_sync(void)
{
	CSoundMng::GetInstance()->Sync();
}

/**
 * �X�g���[�� �p�����]�ݒ�
 * @param[in] bReverse ���]
 */
void soundmng_setreverse(BOOL bReverse)
{
	CSoundMng::GetInstance()->SetReverse((bReverse) ? true : false);
}

/**
 * PCM ���[�h
 * @param[in] nNum PCM �ԍ�
 * @param[in] lpFilename �t�@�C����
 */
void soundmng_pcmload(SoundPCMNumber nNum, LPCTSTR lpFilename)
{
	CSoundMng::GetInstance()->LoadPCM(nNum, lpFilename);
}

/**
 * PCM ���H�����[���ݒ�
 * @param[in] nNum PCM �ԍ�
 * @param[in] nVolume ���H�����[��
 */
void soundmng_pcmvolume(SoundPCMNumber nNum, int nVolume)
{
	CSoundMng::GetInstance()->SetPCMVolume(nNum, nVolume);
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
	return (CSoundMng::GetInstance()->PlayPCM(nNum, bLoop)) ? SUCCESS : FAILURE;
}

/**
 * PCM ��~
 * @param[in] nNum PCM �ԍ�
 */
void soundmng_pcmstop(enum SoundPCMNumber nNum)
{
	CSoundMng::GetInstance()->StopPCM(nNum);
}

/**
 * ������
 * @retval SUCCESS ����
 * @retval FAILURE ���s
 */
BRESULT soundmng_initialize(void)
{
#if defined(SUPPORT_ROMEO)
	CExternalChipManager::GetInstance()->Initialize();
#endif	// defined(SUPPORT_ROMEO)

	return (CSoundMng::GetInstance()->Initialize(g_hWndMain)) ? SUCCESS : FAILURE;
}

/**
 * ���
 */
void soundmng_deinitialize(void)
{
#if defined(SUPPORT_ROMEO)
	CExternalChipManager::GetInstance()->Deinitialize();
#endif	// defined(SUPPORT_ROMEO)

	CSoundMng::GetInstance()->Deinitialize();
}

/**
 * �T�E���h�L��
 * @param[in] nProc �v���V�[�W��
 */
void soundmng_enable(SoundProc nProc)
{
	CSoundMng::GetInstance()->EnableSound(nProc);
}

/**
 * �T�E���h����
 * @param[in] nProc �v���V�[�W��
 */
void soundmng_disable(SoundProc nProc)
{
	CSoundMng::GetInstance()->DisableSound(nProc);
}
