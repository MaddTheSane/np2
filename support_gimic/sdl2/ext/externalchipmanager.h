/**
 * @file	externalchipmanager.h
 * @brief	�O���`�b�v�Ǘ��N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>
#include "externalchip.h"
#include "gimic/gimic.h"
#include "scci/scciif.h"

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
	static CExternalChipManager sm_instance;	/*!< �B��̃C���X�^���X�ł� */
	std::vector<IExternalChip*> m_chips;		/*!< ���W���[�� */

	CGimic m_gimic;								/*!< C86Ctl �C���X�^���X */
	CScciIf m_scci;							/*!< SCCI �C���X�^���X */
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
