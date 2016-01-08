/**
 * @file	externalopna.h
 * @brief	�O�� OPNA ���t�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "externalchip.h"

/**
 * @brief �O�� OPNA ���t�N���X
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
	IExternalChip* m_pChip;				//!< �`�b�v
	bool m_bHasPsg;						//!< PSG
	bool m_bHasExtend;					//!< Extend
	bool m_bHasRhythm;					//!< Rhythm
	bool m_bHasADPCM;					//!< ADPCM
	UINT8 m_cPsgMix;					//!< PSG �~�L�T�[
	UINT8 m_cMode;						/*!< ���[�h */
	UINT8 m_cAlgorithm[8];				//!< �A���S���Y�� �e�[�u��
	UINT8 m_cTtl[8 * 4];				//!< TTL �e�[�u��

	void Mute(bool bMute) const;
	void WriteRegisterInner(UINT nAddr, UINT8 cData) const;
	void SetVolume(UINT nChannel, int nVolume) const;
};

/**
 * PSG �������Ă���?
 * @retval true �L��
 * @retval false ����
 */
inline bool CExternalOpna::HasPsg() const
{
	return m_bHasPsg;
}

/**
 * Rhythm �������Ă���?
 * @retval true �L��
 * @retval false ����
 */
inline bool CExternalOpna::HasRhythm() const
{
	return m_bHasRhythm;
}

/**
 * ADPCM �̃o�b�t�@�������Ă���?
 * @retval true �L��
 * @retval false ����
 */
inline bool CExternalOpna::HasADPCM() const
{
	return m_bHasADPCM;
}
