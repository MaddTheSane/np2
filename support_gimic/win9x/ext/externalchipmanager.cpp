/**
 * @file	externalchipmanager.c
 * @brief	�O���`�b�v�Ǘ��N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "externalchipmanager.h"
#include <algorithm>
#include "np2.h"
#include "externalopna.h"
// #include "c86ctl\c86ctlif.h"
#include "rebirth\rebirth.h"
// #include "romeo\juliet.h"

/*! �B��̃C���X�^���X�ł� */
CExternalChipManager CExternalChipManager::sm_instance;

/**
 * �R���X�g���N�^
 */
CExternalChipManager::CExternalChipManager()
{
}

/**
 * ������
 */
void CExternalChipManager::Initialize()
{
#if 0
	IExternalChip* pModule;

	// ROMEO
	if (np2oscfg.useromeo)
	{
		pModule = new CJuliet;
		if (pModule->Initialize())
		{
			m_module = pModule;
//			m_bHasADPCM = pModule->HasADPCM();
			return;
		}
		delete pModule;
	}

	// G.I.M.I.C / C86BOX
	pModule = new C86CtlIf;
	if (pModule->Initialize())
	{
		m_module = pModule;
		m_bHasADPCM = pModule->HasADPCM();
		return;
	}
	delete pModule;

	// RE:birth
	pModule = new CRebirth;
	if (pModule->Initialize())
	{
		m_module = pModule;
		m_bHasADPCM = pModule->HasADPCM();
		return;
	}
	delete pModule;
#endif
}

/**
 * ���
 */
void CExternalChipManager::Deinitialize()
{
	std::vector<IExternalChip*>::iterator it = m_chips.begin();
	while (it != m_chips.end())
	{
		IExternalChip* pChip = *it;
		it = m_chips.erase(it);

		pChip->Reset();
		delete pChip;
	}

	m_rebirth.Deinitialize();
}

/**
 * �`�b�v�m��
 * @return ���\�[�X
 */
IExternalChip* CExternalChipManager::GetInterface(IExternalChip::ChipType nType, UINT nClock)
{
	IExternalChip* pChip = NULL;

	if (pChip == NULL)
	{
		pChip = m_rebirth.GetInterface(nType, nClock);
	}

	// ���b�s���O
	if (pChip)
	{
		switch (nType)
		{
			case IExternalChip::kYM2608:
			case IExternalChip::kYM3438:
			case IExternalChip::kYMF288:
				pChip = new CExternalOpna(pChip);
				break;
		}
	}
	if (pChip)
	{
		m_chips.push_back(pChip);
	}
	return pChip;
}

/**
 * �`�b�v���
 * @param[in] pChip �`�b�v
 */
void CExternalChipManager::Release(IExternalChip* pChip)
{
	std::vector<IExternalChip*>::iterator it = std::find(m_chips.begin(), m_chips.end(), pChip);
	if (it != m_chips.end())
	{
		m_chips.erase(it);
		(*it)->Reset();
		delete *it;
	}
}

/**
 * �������Z�b�g
 */
void CExternalChipManager::Reset()
{
	for (std::vector<IExternalChip*>::iterator it = m_chips.begin(); it != m_chips.end(); ++it)
	{
		(*it)->Reset();
	}
}

/**
 * �~���[�g
 * @param[in] bMute �~���[�g
 */
void CExternalChipManager::Mute(bool bMute)
{
	for (std::vector<IExternalChip*>::iterator it = m_chips.begin(); it != m_chips.end(); ++it)
	{
		(*it)->Message(IExternalChip::kMute, static_cast<INTPTR>(bMute));
	}
}
