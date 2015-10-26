/**
 * @file	externalchipmanager.h
 * @brief	�O���`�b�v�Ǘ��N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "externalchip.h"

/**
 * @brief �O���`�b�v�Ǘ��N���X
 */
class CExternalChipManager
{
public:
	static CExternalChipManager* GetInstance();

	CExternalChipManager();
	~CExternalChipManager();
	void Initialize();
	void Deinitialize();
	IExternalChip* GetInterface(IExternalChip::ChipType nChipType, UINT nClock);
	void Release(IExternalChip* pChip);
	void Reset();
	void Mute(bool bMute);

private:
	static CExternalChipManager sm_instance;	//!< �B��̃C���X�^���X�ł�

	IExternalChip* m_pGimic;
	IExternalChip* m_pLight;

	IExternalChip* GetInterfaceInner(IExternalChip::ChipType nChipType, UINT nClock);
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline CExternalChipManager* CExternalChipManager::GetInstance()
{
	return &sm_instance;
}
