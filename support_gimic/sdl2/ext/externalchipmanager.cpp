/**
 * @file	externalopna.cpp
 * @brief	�O�� OPNA ���t�N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "externalchipmanager.h"
#include "gimic/gimic.h"
#include "spfm/spfmlight.h"
#include "externalopna.h"

/*! �B��̃C���X�^���X�ł� */
CExternalChipManager CExternalChipManager::sm_instance;

/**
 * �R���X�g���N�^
 */
CExternalChipManager::CExternalChipManager()
{
}

/**
 * �f�X�g���N�^
 */
CExternalChipManager::~CExternalChipManager()
{
	Deinitialize();
}

/**
 * ������
 */
void CExternalChipManager::Initialize()
{
}

/**
 * ���
 */
void CExternalChipManager::Deinitialize()
{
	m_spfm.Deinitialize();
}

/**
 * �`�b�v�m��
 * @param[in] nChipType �`�b�v �^�C�v
 * @param[in] nClock �`�b�v �N���b�N
 * @return �C���X�^���X
 */
IExternalChip* CExternalChipManager::GetInterface(IExternalChip::ChipType nChipType, UINT nClock)
{
	IExternalChip* pChip = GetInterfaceInner(nChipType, nClock);
	if (pChip == NULL)
	{
		if (nChipType == IExternalChip::kYMF288)
		{
			pChip = GetInterface(IExternalChip::kYM2608, nClock);
		}
		else if (nChipType == IExternalChip::kYM3438)
		{
			pChip = GetInterface(IExternalChip::kYMF288, nClock);
		}
	}
	return pChip;
}

/**
 * �`�b�v�m�� (Inner)
 * @param[in] nChipType �`�b�v �^�C�v
 * @param[in] nClock �`�b�v �N���b�N
 * @return �C���X�^���X
 */
IExternalChip* CExternalChipManager::GetInterfaceInner(IExternalChip::ChipType nChipType, UINT nClock)
{
	IExternalChip* pChip = NULL;

	// G.I.M.I.C / C86BOX
	if (pChip == NULL)
	{
		pChip = m_gimic.GetInterface(nChipType, nClock);
	}

	// SPFM Light
	if (pChip == NULL)
	{
		pChip = m_spfm.GetInterface(nChipType, nClock);
	}

	if (pChip)
	{
		pChip = new CExternalOpna(pChip);
	}

	return pChip;
}

/**
 * �`�b�v���
 * @param[in] pChip �`�b�v
 */
void CExternalChipManager::Release(IExternalChip* pChip)
{
	if (pChip != NULL)
	{
		delete pChip;
	}
}

/**
 * �������Z�b�g
 */
void CExternalChipManager::Reset()
{
	m_spfm.Reset();
}

/**
 * �~���[�g
 * @param[in] bMute �~���[�g
 */
void CExternalChipManager::Mute(bool bMute)
{
	m_spfm.Message(IExternalChip::kMute, static_cast<INTPTR>(bMute));
}
