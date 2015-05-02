/**
 * @file	rebirth.cpp
 * @brief	SCCI �A�N�Z�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "rebirth.h"
#include "scci.h"
#include "SCCIDefines.h"

/**
 * �R���X�g���N�^
 */
CRebirth::CRebirth()
	: m_hModule(NULL)
	, m_pManager(NULL)
	, m_pChip(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CRebirth::~CRebirth()
{
}

/**
 * ������
 * @retval true ����
 * @retval false ���s
 */
bool CRebirth::Initialize()
{
	Deinitialize();

	m_hModule = ::LoadLibrary(TEXT("SCCI.DLL"));
	if (m_hModule == NULL)
	{
		return false;
	}

	do
	{
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
		m_pManager->initializeInstance();

		// ���Z�b�g���s��
		m_pManager->reset();

		m_pChip = m_pManager->getSoundChip(SC_TYPE_YM2608, SC_CLOCK_7987200);
		if (m_pChip != NULL)
		{
			// �T�E���h�`�b�v�擾�ł���
			printf("Found YM2608\n");
			return true;
		}
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
		if (m_pChip)
		{
			// �`�b�v�̊J���i�`�b�v�P�ʂŊJ���̏ꍇ�j
			m_pManager->releaseSoundChip(m_pChip);
		}

		// �ꊇ�J������ꍇ�i�`�b�v�ꊇ�J���̏ꍇ�j
		m_pManager->releaseAllSoundChip();

		// �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�C���X�^���X�J��
		// FreeLibrary���s���O�ɕK���Ăяo�����Ă�������
		m_pManager->releaseInstance();
	}

	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
	}
	m_hModule = NULL;
	m_pManager = NULL;
	m_pChip = NULL;
}

/**
 * �r�W�[?
 * @retval true �r�W�[
 * @retval false ���f�B
 */
bool CRebirth::IsBusy()
{
	return (m_pChip == NULL);
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
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CRebirth::WriteRegister(UINT nAddr, UINT8 cData)
{
	if (m_pChip)
	{
		m_pChip->setRegister(nAddr, cData);
	}
}
