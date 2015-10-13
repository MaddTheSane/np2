/**
 * @file	rebirth.h
 * @brief	SCCI �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>
#include "..\externalchip.h"

class SoundInterfaceManager;
class SoundChip;

/**
 * @brief SCCI �A�N�Z�X �N���X
 */
class CRebirth
{
public:
	CRebirth();
	~CRebirth();
	bool Initialize();
	void Deinitialize();
	void Reset();
	IExternalChip* GetInterface(IExternalChip::ChipType nType, UINT nClock);

private:
	HMODULE m_hModule;					//!< ���W���[��	
	SoundInterfaceManager* m_pManager;	//!< �}�l�[�W��

	/**
	 * @brief �`�b�v �N���X
	 */
	class Chip : public IExternalChip
	{
		public:
			Chip(CRebirth* pRebirth, SoundChip* pChip);
			virtual ~Chip();
			operator SoundChip*();
			virtual ChipType GetChipType();
			virtual void Reset();
			virtual void WriteRegister(UINT nAddr, UINT8 cData);
			virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

		private:
			CRebirth* m_pRebirth;		//!< �e�C���X�^���X
			SoundChip* m_pChip;			//!< �`�b�v �C���X�^���X
	};

	std::vector<Chip*> m_chips;			//!< �`�b�v
	void Detach(Chip* pChip);
	friend class Chip;
};
