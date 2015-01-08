/**
 * @file	juliet.h
 * @brief	ROMEO �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief ROMEO �A�N�Z�X �N���X
 */
class CJuliet
{
public:
	static CJuliet* GetInstance();
	CJuliet();
	bool Initialize();
	void Deinitialize();
	bool IsEnabled() const;
	bool IsBusy() const;
	void Reset() const;
	void WriteRegister(UINT nAddr, UINT8 cData);
	void Mute(bool bMute) const;
	void Restore(const UINT8* data, bool bOpna);

private:
	//! @brief ���[�h�֐�
	struct ProcItem
	{
		LPCSTR lpSymbol;		//!< �֐���
		size_t nOffset;			//!< �I�t�Z�b�g
	};

	// ��`
	typedef ULONG (WINAPI * FnRead32)(ULONG ulPciAddress, ULONG ulRegAddress);	//!< �R���t�B�O���[�V�����ǂݎ��֐���`
	typedef VOID (WINAPI * FnOut8)(ULONG ulAddress, UCHAR ucParam);				//!< outp �֐���`
	typedef VOID (WINAPI * FnOut32)(ULONG ulAddress, ULONG ulParam);			//!< outpd �֐���`
	typedef UCHAR (WINAPI * FnIn8)(ULONG ulAddress);							//!< inp �֐���`


	static CJuliet sm_instance;	//!< �B��̃C���X�^���X�ł�

	HMODULE m_hModule;			//!< ���W���[��
	FnRead32 m_fnRead32;		//!< �R���t�B�O���[�V�����ǂݎ��֐�
	FnOut8 m_fnOut8;			//!< outp �֐�
	FnOut32 m_fnOut32;			//!< outpd �֐�
	FnIn8 m_fnIn8;				//!< inp �֐�
	bool m_bOpna;				//!< OPNA �L���t���O
	UCHAR m_ucIrq;				//!< ROMEO IRQ
	UINT8 m_cPsgMix;			//!< PSG �~�L�T�[
	ULONG m_ulAddress;			//!< ROMEO �x�[�X �A�h���X
	UINT8 m_cAlgorithm[8];		//!< �A���S���Y�� �e�[�u��
	UINT8 m_cTtl[8 * 4];		//!< TTL �e�[�u��

	void Clear();
	ULONG SearchRomeo() const;
	void WriteRegisterInner(UINT nAddr, UINT8 cData) const;
	void SetVolume(UINT nChannel, int nVolume) const;
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline CJuliet* CJuliet::GetInstance()
{
	return &sm_instance;
}

/**
 * ROMEO �͗L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool CJuliet::IsEnabled() const
{
	return (m_hModule != NULL);
}
