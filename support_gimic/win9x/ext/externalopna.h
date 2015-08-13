/**
 * @file	externalopna.h
 * @brief	�O�� OPNA ���t�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

class IExtendModule;

/**
 * @brief �O�� OPNA ���t�N���X
 */
class CExternalOpna
{
public:
	static CExternalOpna* GetInstance();

	CExternalOpna();
	void Initialize();
	void Deinitialize();
	bool IsEnabled() const;
	bool IsBusy() const;
	void Reset() const;
	void WriteRegister(UINT nAddr, UINT8 cData);
	void Mute(bool bMute) const;
	void Restore(const UINT8* data, bool bOpna);

private:
	static CExternalOpna sm_instance;	//!< �B��̃C���X�^���X�ł�
	IExtendModule* m_module;			//!< ���W���[��
	UINT8 m_cPsgMix;					//!< PSG �~�L�T�[
	UINT8 m_cAlgorithm[8];				//!< �A���S���Y�� �e�[�u��
	UINT8 m_cTtl[8 * 4];				//!< TTL �e�[�u��

	void WriteRegisterInner(UINT nAddr, UINT8 cData) const;
	void SetVolume(UINT nChannel, int nVolume) const;
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline CExternalOpna* CExternalOpna::GetInstance()
{
	return &sm_instance;
}

/**
 * �C���X�^���X�͗L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool CExternalOpna::IsEnabled() const
{
	return (m_module != NULL);
}
