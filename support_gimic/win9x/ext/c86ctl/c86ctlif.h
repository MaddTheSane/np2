/**
 * @file	c86ctlif.h
 * @brief	G.I.M.I.C �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <map>
#include "..\externalchip.h"

namespace c86ctl
{
	interface IRealChipBase;
	interface IGimic;
	interface IRealChip;
}

/**
 * @brief G.I.M.I.C �A�N�Z�X �N���X
 */
class C86CtlIf
{
public:
	C86CtlIf();
	~C86CtlIf();
	bool Initialize();
	void Deinitialize();
	void Reset();
	IExternalChip* GetInterface(IExternalChip::ChipType nType, UINT nClock);

private:
	HMODULE m_hModule;					//!< ���W���[�� �n���h��
	c86ctl::IRealChipBase* m_pChipBase;	//!< �`�b�v �x�[�X �C���X�^���X

	/**
	 * @brief �`�b�v �N���X
	 */
	class Chip : public IExternalChip
	{
		public:
			Chip(C86CtlIf* pC86Ctl, c86ctl::IRealChip* pRealChip, c86ctl::IGimic* pGimic, ChipType nType, UINT nClock);
			virtual ~Chip();
			virtual ChipType GetChipType();
			virtual void Reset();
			virtual void WriteRegister(UINT nAddr, UINT8 cData);
			virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

		private:
			C86CtlIf* m_pC86Ctl;				//!< C86Ctl �C���X�^���X
			c86ctl::IRealChip* m_pRealChip;		//!< �`�b�v �C���X�^���X
			c86ctl::IGimic* m_pGimic;			//!< G.I.M.I.C �C���X�^���X
			ChipType m_nType;					//!< �`�b�v �^�C�v
			UINT m_nClock;						//!< �`�b�v �N���b�N
	};

	std::map<int, Chip*> m_chips;			//!< �`�b�v
	void Detach(Chip* pChip);
	friend class Chip;
};
