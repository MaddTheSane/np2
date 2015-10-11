/**
 * @file	rebirth.cpp
 * @brief	SCCI �A�N�Z�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "rebirth.h"
#include "scci.h"
#include "SCCIDefines.h"
#include <algorithm>

/**
 * �R���X�g���N�^
 */
CRebirth::CRebirth()
	: m_hModule(NULL)
	, m_pManager(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CRebirth::~CRebirth()
{
	Deinitialize();
}

/**
 * ������
 * @retval true ����
 * @retval false ���s
 */
bool CRebirth::Initialize()
{
	if (m_hModule)
	{
		return false;
	}

	do
	{
		m_hModule = ::LoadLibrary(TEXT("SCCI.DLL"));
		if (m_hModule == NULL)
		{
			break;
		}

		// �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�擾�p�֐��A�h���X�擾
		SCCIFUNC fnGetSoundInterfaceManager = reinterpret_cast<SCCIFUNC>(::GetProcAddress(m_hModule, "getSoundInterfaceManager"));
		if (fnGetSoundInterfaceManager == NULL)
		{
			break;
		}

		// �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�擾
		m_pManager = (*fnGetSoundInterfaceManager)();
		if (m_pManager == NULL)
		{
			break;
		}

		// �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�C���X�^���X������
		// �K���ŏ��Ɏ��s���Ă�������
		if (!m_pManager->initializeInstance())
		{
			break;
		}

		// ���Z�b�g���s��
		Reset();
		return true;
	} while (false /*CONSTCOND*/);

	Deinitialize();
	return false;
}

/**
 * ���
 */
void CRebirth::Deinitialize()
{
	if (m_pManager)
	{
		while (!m_chips.empty())
		{
			delete m_chips.front();
		}
		m_chips.clear();

		// �ꊇ�J������ꍇ�i�`�b�v�ꊇ�J���̏ꍇ�j
		m_pManager->releaseAllSoundChip();

		// �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�C���X�^���X�J��
		// FreeLibrary���s���O�ɕK���Ăяo�����Ă�������
		m_pManager->releaseInstance();

		m_pManager = NULL;
	}

	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}

/**
 * �������Z�b�g
 */
void CRebirth::Reset()
{
	if (m_pManager)
	{
		// ���Z�b�g���s��
		m_pManager->reset();
	}
}

/**
 * �C���^�[�t�F�C�X�擾
 */
IExternalChip* CRebirth::GetInterface(IExternalChip::ChipType nType, UINT nClock)
{
	const bool bInitialized = Initialize();

	do
	{
		if (m_pManager == NULL)
		{
			break;
		}

		SC_CHIP_TYPE iSoundChipType = SC_TYPE_NONE;
		switch (nType)
		{
			case IExternalChip::kYM2608:
				iSoundChipType = SC_TYPE_YM2608;
				break;

			case IExternalChip::kYM3438:
				iSoundChipType = SC_TYPE_YM2612;
				break;

			case IExternalChip::kYMF288:
				iSoundChipType = SC_TYPE_YMF288;
				break;
		}

		SoundChip* pSoundChip = m_pManager->getSoundChip(iSoundChipType, nClock);
		if (pSoundChip != NULL)
		{
			// �T�E���h�`�b�v�擾�ł���
			Chip* pChip = new Chip(this, pSoundChip);
			m_chips.push_back(pChip);
			return pChip;
		}
	} while (false /*CONSTCOND*/);

	if (bInitialized)
	{
		Deinitialize();
	}
	return NULL;
}

/**
 * ���
 * @param[in] pChip �`�b�v
 */
void CRebirth::Detach(CRebirth::Chip* pChip)
{
	std::vector<Chip*>::iterator it = std::find(m_chips.begin(), m_chips.end(), pChip);
	if (it != m_chips.end())
	{
		m_chips.erase(it);
	}

	// �`�b�v�̊J���i�`�b�v�P�ʂŊJ���̏ꍇ�j
	if (m_pManager)
	{
		m_pManager->releaseSoundChip(*pChip);
	}
}

// ---- �`�b�v

/**
 * �R���X�g���N�^
 */
CRebirth::Chip::Chip(CRebirth* pRebirth, SoundChip* pChip)
	: m_pRebirth(pRebirth)
	, m_pChip(pChip)
{
}

/**
 * �f�X�g���N�^
 */
CRebirth::Chip::~Chip()
{
	m_pRebirth->Detach(this);
}

/**
 * �I�y���[�^
 */
CRebirth::Chip::operator SoundChip*()
{
	return m_pChip;
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType CRebirth::Chip::GetChipType()
{
	switch (m_pChip->getSoundChipType())
	{
		case SC_TYPE_YM2608:
			return IExternalChip::kYM2608;

		case SC_TYPE_YM2612:
			return IExternalChip::kYM3438;

		case SC_TYPE_YMF288:
			return IExternalChip::kYMF288;
	}
	return IExternalChip::kNone;
}

/**
 * ���Z�b�g
 */
void CRebirth::Chip::Reset()
{
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CRebirth::Chip::WriteRegister(UINT nAddr, UINT8 cData)
{
	m_pChip->setRegister(nAddr, cData);
}

/**
 * ���b�Z�[�W
 */
INTPTR CRebirth::Chip::Message(UINT nMessage, INTPTR nParameter)
{
	return 0;
}
