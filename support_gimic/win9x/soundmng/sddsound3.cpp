/**
 * @file	sddsound3.cpp
 * @brief	DSound3 オーディオ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "sddsound3.h"
#include "soundmng.h"
#include "misc\extrom.h"
#include "common\wavefile.h"

#if !defined(__GNUC__)
#pragma comment(lib, "dsound.lib")
#endif	// !defined(__GNUC__)

#if defined(_M_IA64) || defined(_M_AMD64)
#define SOUNDBUFFERALIGN	(1 << 3)					/*!< バッファ アライメント */
#else
#define SOUNDBUFFERALIGN	(1 << 2)					/*!< バッファ アライメント */
#endif

#if 1
#define DSBUFFERDESC_SIZE	20							/*!< DirectX3 Structsize */
#else
#define DSBUFFERDESC_SIZE	sizeof(DSBUFFERDESC)		/*!< DSBUFFERDESC Structsize */
#endif

#ifndef DSBVOLUME_MAX
#define DSBVOLUME_MAX		0							/*!< ヴォリューム最大値 */
#endif
#ifndef DSBVOLUME_MIN
#define DSBVOLUME_MIN		(-10000)					/*!< ヴォリューム最小値 */
#endif

/**
 * コンストラクタ
 */
CSoundDeviceDSound3::CSoundDeviceDSound3()
	: m_lpDSound(NULL)
	, m_lpDSStream(NULL)
	, m_dwHalfBufferSize(0)
	, m_nStreamEvent(-1)
{
}

/**
 * デストラクタ
 */
CSoundDeviceDSound3::~CSoundDeviceDSound3()
{
}

/**
 * オープン
 * @param[in] lpDevice デバイス名
 * @param[in] hWnd ウィンドウ ハンドル
 * @retval true 成功
 * @retval false 失敗
 */
bool CSoundDeviceDSound3::Open(LPCTSTR lpDevice, HWND hWnd)
{
	if (hWnd == NULL)
	{
		return false;
	}

	// DirectSoundの初期化
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
	return true;
}

/**
 * クローズ
 */
void CSoundDeviceDSound3::Close()
{
	DestroyAllPCM();
	DestroyStream();

	if (m_lpDSound)
	{
		m_lpDSound->Release();
	}
}

/**
 * ストリームの作成
 * @param[in] nSamplingRate サンプリング レート
 * @param[in] nChannels チャネル数
 * @param[in] nBufferSize バッファ サイズ
 * @return バッファ サイズ
 */
UINT CSoundDeviceDSound3::CreateStream(UINT nSamplingRate, UINT nChannels, UINT nBufferSize)
{
	if (m_lpDSound == NULL)
	{
		return 0;
	}

	if (nBufferSize == 0)
	{
		nBufferSize = nSamplingRate / 10;
	}

//	m_nBufferSize = nBufferSize;
	m_dwHalfBufferSize = nBufferSize * nChannels * sizeof(short);

	PCMWAVEFORMAT pcmwf;
	ZeroMemory(&pcmwf, sizeof(pcmwf));
	pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
	pcmwf.wf.nChannels = nChannels;
	pcmwf.wf.nSamplesPerSec = nSamplingRate;
	pcmwf.wBitsPerSample = 16;
	pcmwf.wf.nBlockAlign = nChannels * (pcmwf.wBitsPerSample / 8);
	pcmwf.wf.nAvgBytesPerSec = nSamplingRate * pcmwf.wf.nBlockAlign;

	DSBUFFERDESC dsbdesc;
	ZeroMemory(&dsbdesc, sizeof(dsbdesc));
	dsbdesc.dwSize = sizeof(dsbdesc);
	dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
						DSBCAPS_CTRLFREQUENCY |
						DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;
	dsbdesc.lpwfxFormat = reinterpret_cast<LPWAVEFORMATEX>(&pcmwf);
	dsbdesc.dwBufferBytes = m_dwHalfBufferSize * 2;
	HRESULT r = m_lpDSound->CreateSoundBuffer(&dsbdesc, &m_lpDSStream, NULL);
	if (FAILED(r))
	{
		dsbdesc.dwSize = (sizeof(DWORD) * 4) + sizeof(LPWAVEFORMATEX);
		r = m_lpDSound->CreateSoundBuffer(&dsbdesc, &m_lpDSStream, NULL);
	}
	if (FAILED(r))
	{
		DestroyStream();
		return 0;
	}

	m_nStreamEvent = -1;
	return nBufferSize;
}

/**
 * ストリームをリセット
 */
void CSoundDeviceDSound3::ResetStream()
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
 * ストリームを破棄
 */
void CSoundDeviceDSound3::DestroyStream()
{
	if (m_lpDSStream)
	{
		m_lpDSStream->Stop();
		m_lpDSStream->Release();
		m_lpDSStream = NULL;
	}
}

/**
 * ストリームの再生
 * @retval true 成功
 * @retval false 失敗
 */
bool CSoundDeviceDSound3::PlayStream()
{
	if (m_lpDSStream)
	{
		m_lpDSStream->Play(0, 0, DSBPLAY_LOOPING);
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * ストリームの停止
 */
void CSoundDeviceDSound3::StopStream()
{
	if (m_lpDSStream)
	{
		m_lpDSStream->Stop();
	}
}

/**
 * ストリームを更新する
 * @param[in] dwPosition 更新位置
 */
void CSoundDeviceDSound3::FillStream(DWORD dwPosition)
{
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
		if (m_pSoundData)
		{
			m_pSoundData->Get16(static_cast<SINT16*>(lpBlock1), cbBlock1 / 4);
		}
		else
		{
			ZeroMemory(lpBlock1, cbBlock1);
		}
		m_lpDSStream->Unlock(lpBlock1, cbBlock1, lpBlock2, cbBlock2);
	}
}

/**
 * 同期
 */
void CSoundDeviceDSound3::SyncStream()
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
 * PCM バッファを破棄する
 */
void CSoundDeviceDSound3::DestroyAllPCM()
{
	for (std::map<UINT, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.begin(); it != m_pcm.begin(); ++it)
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->Stop();
		lpDSBuffer->Release();
	}
	m_pcm.clear();
}

/**
 * PCM をストップ
 */
void CSoundDeviceDSound3::StopAllPCM()
{
	for (std::map<UINT, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.begin(); it != m_pcm.begin(); ++it)
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->Stop();
	}
}

/**
 * PCM データ読み込み
 * @param[in] nNum PCM 番号
 * @param[in] lpFilename ファイル名
 * @retval true 成功
 * @retval false 失敗
 */
bool CSoundDeviceDSound3::LoadPCM(UINT nNum, LPCTSTR lpFilename)
{
	UnloadPCM(nNum);

	LPDIRECTSOUNDBUFFER lpDSBuffer = CreateWaveBuffer(lpFilename);
	if (lpDSBuffer)
	{
		m_pcm[nNum] = lpDSBuffer;
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * PCM データ読み込み
 * @param[in] lpFilename ファイル名
 * @return バッファ
 */
LPDIRECTSOUNDBUFFER CSoundDeviceDSound3::CreateWaveBuffer(LPCTSTR lpFilename)
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
 * PCM をアンロード
 * @param[in] nNum PCM 番号
 */
void CSoundDeviceDSound3::UnloadPCM(UINT nNum)
{
	std::map<UINT, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
	if (it != m_pcm.end())
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		m_pcm.erase(it);

		lpDSBuffer->Stop();
		lpDSBuffer->Release();
	}
}

/**
 * PCM ヴォリューム設定
 * @param[in] nNum PCM 番号
 * @param[in] nVolume ヴォリューム
 */
void CSoundDeviceDSound3::SetPCMVolume(UINT nNum, int nVolume)
{
	std::map<UINT, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
	if (it != m_pcm.end())
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->SetVolume((((DSBVOLUME_MAX - DSBVOLUME_MIN) * nVolume) / 100) + DSBVOLUME_MIN);
	}
}

/**
 * PCM 再生
 * @param[in] nNum PCM 番号
 * @param[in] bLoop ループ フラグ
 * @retval true 成功
 * @retval false 失敗
 */
bool CSoundDeviceDSound3::PlayPCM(UINT nNum, BOOL bLoop)
{
	std::map<UINT, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
	if (it != m_pcm.end())
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
//		lpDSBuffer->SetCurrentPosition(0);
		lpDSBuffer->Play(0, 0, (bLoop) ? DSBPLAY_LOOPING : 0);
		return true;
	}
	return false;
}

/**
 * PCM 停止
 * @param[in] nNum PCM 番号
 */
void CSoundDeviceDSound3::StopPCM(UINT nNum)
{
	std::map<UINT, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
	if (it != m_pcm.end())
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->Stop();
	}
}
