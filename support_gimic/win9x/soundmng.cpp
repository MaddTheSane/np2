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
 * @param[out] dst 出力バッファ
 * @param[in] src 入力バッファ
 * @param[in] size サイズ
 */
void __fastcall satuation_s16mmx(SINT16 *dst, const SINT32 *src, UINT size);
#ifdef __cplusplus
}
#endif
#endif

/**
 * @brief サウンド マネージャ クラス
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
	static CSoundMng sm_instance;									//!< 唯一のインスタンスです

	typedef void (PARTSCALL * FNMIX)(SINT16*, const SINT32*, UINT);	//!< satuation関数型宣言
	FNMIX m_fnMix;													//!< satuation関数ポインタ
	UINT m_nMute;													//!< ミュート フラグ
	CSoundDeviceDSound3 m_dsound3;									//!< Direct Sound
};

#if defined(VERMOUTH_LIB)
	MIDIMOD		vermouth_module = NULL;
#endif

//! 唯一のインスタンスです
CSoundMng CSoundMng::sm_instance;

/**
 * インスタンスを得る
 * @return インスタンス
 */
inline CSoundMng* CSoundMng::GetInstance()
{
	return &sm_instance;
}

/**
 * コンストラクタ
 */
CSoundMng::CSoundMng()
	: m_nMute(0)
{
	SetReverse(false);
}

/**
 * 初期化
 * @param[in] hWnd ウィンドウ ハンドル
 * @retval true 成功
 * @retval false 失敗
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
 * 解放
 */
inline void CSoundMng::Deinitialize()
{
	m_dsound3.Deinitialize();

#if defined(MT32SOUND_DLL)
	MT32Sound::GetInstance()->Deinitialize();
#endif
}

/**
 * サウンド有効
 * @param[in] nProc プロシージャ
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
 * サウンド無効
 * @param[in] nProc プロシージャ
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
 * ストリームを作成
 * @param[in] rate サンプリング レート
 * @param[in] ms バッファ長(ミリ秒)
 * @return バッファ数
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
 * ストリームをリセット
 */
inline void CSoundMng::ResetStream()
{
	m_dsound3.ResetStream();
}

/**
 * ストリームを破棄
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
 * ストリームの再生
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
 * ストリームの停止
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
 * ストリームを得る
 * @param[out] lpBuffer バッファ
 * @param[in] nBufferCount バッファ カウント
 * @return サンプル数
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
 * 同期
 */
inline void CSoundMng::Sync()
{
	m_dsound3.Sync();
}

/**
 * パン反転を設定する
 * @param[in] bReverse 反転フラグ
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
 * PCM データ読み込み
 * @param[in] nNum PCM 番号
 * @param[in] lpFilename ファイル名
 */
inline void CSoundMng::LoadPCM(SoundPCMNumber nNum, LPCTSTR lpFilename)
{
	m_dsound3.LoadPCM(nNum, lpFilename);
}

/**
 * PCM をアンロード
 * @param[in] nNum PCM 番号
 */
inline void CSoundMng::UnloadPCM(SoundPCMNumber nNum)
{
	m_dsound3.UnloadPCM(nNum);
}

/**
 * PCM ヴォリューム設定
 * @param[in] nNum PCM 番号
 * @param[in] nVolume ヴォリューム
 */
inline void CSoundMng::SetPCMVolume(SoundPCMNumber nNum, int nVolume)
{
	m_dsound3.SetPCMVolume(nNum, nVolume);
}

/**
 * PCM 再生
 * @param[in] nNum PCM 番号
 * @param[in] bLoop ループ フラグ
 * @retval true 成功
 * @retval false 失敗
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
 * PCM 停止
 * @param[in] nNum PCM 番号
 */
inline void CSoundMng::StopPCM(SoundPCMNumber nNum)
{
	m_dsound3.StopPCM(nNum);
}

// ---- C ラッパー

/**
 * ストリーム作成
 * @param[in] rate サンプリング レート
 * @param[in] ms バッファ長(ミリ秒)
 * @return バッファ サイズ
 */
UINT soundmng_create(UINT rate, UINT ms)
{
	return CSoundMng::GetInstance()->CreateStream(rate, ms);
}

/**
 * ストリーム リセット
 */
void soundmng_reset(void)
{
	CSoundMng::GetInstance()->ResetStream();
}

/**
 * ストリーム破棄
 */
void soundmng_destroy(void)
{
	CSoundMng::GetInstance()->DestroyStream();
}

/**
 * ストリーム開始
 */
void soundmng_play(void)
{
	CSoundMng::GetInstance()->PlayStream();
}

/**
 * ストリーム停止
 */
void soundmng_stop(void)
{
	CSoundMng::GetInstance()->StopStream();
}

/**
 * ストリーム同期
 */
void soundmng_sync(void)
{
	CSoundMng::GetInstance()->Sync();
}

/**
 * ストリーム パン反転設定
 * @param[in] bReverse 反転
 */
void soundmng_setreverse(BOOL bReverse)
{
	CSoundMng::GetInstance()->SetReverse((bReverse) ? true : false);
}

/**
 * PCM ロード
 * @param[in] nNum PCM 番号
 * @param[in] lpFilename ファイル名
 */
void soundmng_pcmload(SoundPCMNumber nNum, LPCTSTR lpFilename)
{
	CSoundMng::GetInstance()->LoadPCM(nNum, lpFilename);
}

/**
 * PCM ヴォリューム設定
 * @param[in] nNum PCM 番号
 * @param[in] nVolume ヴォリューム
 */
void soundmng_pcmvolume(SoundPCMNumber nNum, int nVolume)
{
	CSoundMng::GetInstance()->SetPCMVolume(nNum, nVolume);
}

/**
 * PCM 再生
 * @param[in] nNum PCM 番号
 * @param[in] bLoop ループ
 * @retval SUCCESS 成功
 * @retval FAILURE 失敗
 */
BRESULT soundmng_pcmplay(enum SoundPCMNumber nNum, BOOL bLoop)
{
	return (CSoundMng::GetInstance()->PlayPCM(nNum, bLoop)) ? SUCCESS : FAILURE;
}

/**
 * PCM 停止
 * @param[in] nNum PCM 番号
 */
void soundmng_pcmstop(enum SoundPCMNumber nNum)
{
	CSoundMng::GetInstance()->StopPCM(nNum);
}

/**
 * 初期化
 * @retval SUCCESS 成功
 * @retval FAILURE 失敗
 */
BRESULT soundmng_initialize(void)
{
#if defined(SUPPORT_ROMEO)
	CExternalChipManager::GetInstance()->Initialize();
#endif	// defined(SUPPORT_ROMEO)

	return (CSoundMng::GetInstance()->Initialize(g_hWndMain)) ? SUCCESS : FAILURE;
}

/**
 * 解放
 */
void soundmng_deinitialize(void)
{
#if defined(SUPPORT_ROMEO)
	CExternalChipManager::GetInstance()->Deinitialize();
#endif	// defined(SUPPORT_ROMEO)

	CSoundMng::GetInstance()->Deinitialize();
}

/**
 * サウンド有効
 * @param[in] nProc プロシージャ
 */
void soundmng_enable(SoundProc nProc)
{
	CSoundMng::GetInstance()->EnableSound(nProc);
}

/**
 * サウンド無効
 * @param[in] nProc プロシージャ
 */
void soundmng_disable(SoundProc nProc)
{
	CSoundMng::GetInstance()->DisableSound(nProc);
}
