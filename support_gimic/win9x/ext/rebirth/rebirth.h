/**
 * @file	rebirth.h
 * @brief	SCCI �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "..\externalchip.h"

class SoundInterfaceManager;
class SoundChip;

/**
 * @brief SCCI �A�N�Z�X �N���X
 */
class CRebirth : public IExternalChip
{
public:
	CRebirth();
	virtual ~CRebirth();
	virtual bool Initialize();
	virtual void Deinitialize();
	virtual bool IsEnabled();
	virtual bool IsBusy();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);

private:
	HMODULE m_hModule;					//!< ���W���[��	
	SoundInterfaceManager* m_pManager;	//!< �}�l�[�W��
	SoundChip* m_pChip;					//!< �T�E���h �`�b�v �C���^�[�t�F�C�X
	static bool sm_bOnce;				//!< once
};

/**
 * RE:birth �͗L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool CRebirth::IsEnabled()
{
	return (m_pChip != NULL);
}
