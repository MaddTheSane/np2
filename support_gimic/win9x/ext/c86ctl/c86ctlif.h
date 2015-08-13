/**
 * @file	c86ctlif.h
 * @brief	G.I.M.I.C �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

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
class C86CtlIf : public IExternalChip
{
public:
	C86CtlIf();
	virtual ~C86CtlIf();
	virtual bool Initialize();
	virtual void Deinitialize();
	virtual bool IsEnabled();
	virtual bool IsBusy();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual bool HasADPCM();

private:
	HMODULE m_hModule;					//!< ���W���[�� �n���h��
	c86ctl::IRealChipBase* m_chipbase;	//!< �`�b�v �x�[�X �C���X�^���X
	c86ctl::IGimic* m_gimic;			//!< G.I.M.I.C �C���^�t�F�C�X
	c86ctl::IRealChip* m_chip;			//!< �`�b�v �C���^�t�F�C�X
	bool m_bHasADPCM;					//!< ADPCM �t���O
};
