/**
 * @file	externalopna.h
 * @brief	外部 OPNA 演奏クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "externalchip.h"

/**
 * @brief 外部 OPNA 演奏クラス
 */
class CExternalOpna : public IExternalChip
{
public:
	CExternalOpna(IExternalChip* pChip);
	virtual ~CExternalOpna();
	bool HasPsg() const;
	bool HasRhythm() const;
	bool HasADPCM() const;
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter);

protected:
	IExternalChip* m_pChip;				//!< チップ
	bool m_bHasPsg;						//!< PSG
	bool m_bHasExtend;					//!< Extend
	bool m_bHasRhythm;					//!< Rhythm
	bool m_bHasADPCM;					//!< ADPCM
	UINT8 m_cPsgMix;					//!< PSG ミキサー
	UINT8 m_cMode;						/*!< モード */
	UINT8 m_cAlgorithm[8];				//!< アルゴリズム テーブル
	UINT8 m_cTtl[8 * 4];				//!< TTL テーブル

	void Mute(bool bMute) const;
	void WriteRegisterInner(UINT nAddr, UINT8 cData) const;
	void SetVolume(UINT nChannel, int nVolume) const;
};

/**
 * PSG を持っている?
 * @retval true 有効
 * @retval false 無効
 */
inline bool CExternalOpna::HasPsg() const
{
	return m_bHasPsg;
}

/**
 * Rhythm を持っている?
 * @retval true 有効
 * @retval false 無効
 */
inline bool CExternalOpna::HasRhythm() const
{
	return m_bHasRhythm;
}

/**
 * ADPCM のバッファを持っている?
 * @retval true 有効
 * @retval false 無効
 */
inline bool CExternalOpna::HasADPCM() const
{
	return m_bHasADPCM;
}
