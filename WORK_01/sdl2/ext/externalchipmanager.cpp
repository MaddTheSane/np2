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
	: m_pGimic(NULL)
	, m_pLight(NULL)
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
	Release(m_pGimic);
	Release(m_pLight);
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
	// G.I.M.I.C / C86BOX
	if (m_pGimic == NULL)
	{
		CGimic* pModule = new CGimic;
		if (pModule->Initialize(nChipType, nClock))
		{
			m_pGimic = new CExternalOpna(pModule);
			return m_pGimic;
		}
		else
		{
			delete pModule;
		}
	}

	// SPFM Light
	if (m_pLight == NULL)
	{
		CSpfmLight* pModule = new CSpfmLight;
		if (pModule->Initialize(nChipType, nClock))
		{
			m_pLight = new CExternalOpna(pModule);
			return m_pLight;
		}
		else
		{
			delete pModule;
		}
	}
	return NULL;
}

/**
 * �`�b�v���
 * @param[in] pChip �`�b�v
 */
void CExternalChipManager::Release(IExternalChip* pChip)
{
	if (pChip == NULL)
	{
		return;
	}
	if (m_pGimic == pChip)
	{
		m_pGimic = NULL;
		pChip->Reset();
		delete pChip;
	}
	if (m_pLight == pChip)
	{
		m_pLight = NULL;
		pChip->Reset();
		delete pChip;
	}
}

/**
 * �������Z�b�g
 */
void CExternalChipManager::Reset()
{
	if (m_pGimic)
	{
		m_pGimic->Reset();
	}
	if (m_pLight)
	{
		m_pLight->Reset();
	}
}

/**
 * �~���[�g
 * @param[in] bMute �~���[�g
 */
void CExternalChipManager::Mute(bool bMute)
{
	if (m_pGimic)
	{
		m_pGimic->Message(IExternalChip::kMute, static_cast<INTPTR>(bMute));
	}
	if (m_pLight)
	{
		m_pLight->Message(IExternalChip::kMute, static_cast<INTPTR>(bMute));
	}
}
