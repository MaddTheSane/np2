/**
 * @file	juliet.h
 * @brief	ROMEO �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "..\..\ext\externalchip.h"

/**
 * @brief ROMEO �A�N�Z�X �N���X
 */
class CJuliet : public IExternalChip
{
public:
	CJuliet();
	virtual ~CJuliet();
	virtual bool Initialize();
	virtual void Deinitialize();
	virtual bool IsEnabled();
	virtual bool IsBusy();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);

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

	HMODULE m_hModule;			//!< ���W���[��
	FnRead32 m_fnRead32;		//!< �R���t�B�O���[�V�����ǂݎ��֐�
	FnOut8 m_fnOut8;			//!< outp �֐�
	FnOut32 m_fnOut32;			//!< outpd �֐�
	FnIn8 m_fnIn8;				//!< inp �֐�
	ULONG m_ulAddress;			//!< ROMEO �x�[�X �A�h���X
	UCHAR m_ucIrq;				//!< ROMEO IRQ

	ULONG SearchRomeo() const;
};

/**
 * ROMEO �͗L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool CJuliet::IsEnabled()
{
	return (m_hModule != NULL);
}
