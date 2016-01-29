/**
 * @file	sdbase.h
 * @brief	サウンド デバイス基底クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * @brief サウンド データ取得インタフェイス
 */
class ISoundData
{
public:
	/**
	 * ストリーム データを得る
	 * @param[out] lpBuffer バッファ
	 * @param[in] nBufferCount バッファ カウント
	 * @return サンプル数
	 */
	virtual UINT Get16(SINT16* lpBuffer, UINT nBufferCount) = 0;
};

/**
 * @brief サウンド デバイス基底クラス
 */
class CSoundDeviceBase
{
public:
	/**
	 * コンストラクタ
	 */
	CSoundDeviceBase()
		: m_pSoundData(NULL)
	{
	}

	/**
	 * ストリーム データの設定
	 * @param[in] pSoundData サウンド データ
	 */
	void SetStreamData(ISoundData* pSoundData)
	{
		m_pSoundData = pSoundData;
	}

protected:
	ISoundData* m_pSoundData;		/*!< サウンド データ インスタンス */
};
