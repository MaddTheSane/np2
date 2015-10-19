/**
 * @file	c86ctlif.cpp
 * @brief	G.I.M.I.C �A�N�Z�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "c86ctlif.h"
#include "c86ctl.h"

using namespace c86ctl;

//! �C���^�t�F�C�X
typedef HRESULT (WINAPI * FnCreateInstance)(REFIID riid, LPVOID* ppi);

/**
 * �R���X�g���N�^
 */
C86CtlIf::C86CtlIf()
	: m_hModule(NULL)
	, m_chipbase(NULL)
	, m_gimic(NULL)
	, m_chip(NULL)
	, m_bHasADPCM(false)
{
}

/**
 * �f�X�g���N�^
 */
C86CtlIf::~C86CtlIf()
{
}

/**
 * ������
 * @retval true ����
 * @retval false ���s
 */
bool C86CtlIf::Initialize()
{
	Deinitialize();

	do
	{
		// DLL �ǂݍ���
		m_hModule = ::LoadLibrary(TEXT("c86ctl.dll"));
		if (m_hModule == NULL)
		{
			break;
		}
		FnCreateInstance CreateInstance = reinterpret_cast<FnCreateInstance>(::GetProcAddress(m_hModule, "CreateInstance"));
		if (CreateInstance == NULL)
		{
			break;
		}

		// �C���X�^���X�쐬
		(*CreateInstance)(IID_IRealChipBase, reinterpret_cast< LPVOID*>(&m_chipbase));
		if (m_chipbase == NULL)
		{
			break;
		}

		// ������
		if (m_chipbase->initialize() != C86CTL_ERR_NONE)
		{
			break;
		}

		// OPNA ��T��
		// @ToDo: YMF-288
		const int nDeviceCount = m_chipbase->getNumberOfChip();
		for (int i = 0; i < nDeviceCount; i++)
		{
			IRealChip* chip = NULL;
			m_chipbase->getChipInterface(i, IID_IRealChip, reinterpret_cast<LPVOID*>(&chip));
			if (chip == NULL)
			{
				continue;
			}

			// G.I.M.I.C ����
			IGimic* gimic = NULL;
			m_chipbase->getChipInterface(i, IID_IGimic, reinterpret_cast<LPVOID*>(&gimic));
			if (gimic)
			{
				Devinfo info;
				if (gimic->getModuleInfo(&info) == C86CTL_ERR_NONE)
				{
					if (!memcmp(info.Devname, "GMC-OPN3L", 9))
					{
						m_chip = chip;
						m_gimic = gimic;
						m_bHasADPCM = false;
						Reset();
						return true;
					}
					if (!memcmp(info.Devname, "GMC-OPNA", 8))
					{
						m_chip = chip;
						m_gimic = gimic;
						m_bHasADPCM = true;
						Reset();
						return true;
					}

				}
			}

			// ���̑��̔���
			IRealChip3* chip3 = NULL;
			m_chipbase->getChipInterface(i, IID_IRealChip3, reinterpret_cast<LPVOID*>(&chip3));
			if (chip3 != NULL)
			{
				ChipType chiptype = CHIP_UNKNOWN;
				chip3->getChipType(&chiptype);
				const ChipType type = static_cast<ChipType>(chiptype & 0xffff);
				if ((type == CHIP_OPNA) || (type == CHIP_OPN3L))
				{
					m_chip = chip3;
					m_gimic = NULL;
					m_bHasADPCM = (type == CHIP_OPNA);
					Reset();
					return true;
				}
			}
		}
	} while (0 /*CONSTCOND*/);

	Deinitialize();
	return false;
}

/**
 * ���
 */
void C86CtlIf::Deinitialize()
{
	if (m_chipbase)
	{
		m_chipbase->deinitialize();
		m_chipbase = NULL;
		m_gimic = NULL;
		m_chip = NULL;
		m_bHasADPCM = false;
	}
	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}

/**
 * �f�o�C�X�͗L��?
 * @retval true �L��
 * @retval false ����
 */
bool C86CtlIf::IsEnabled()
{
	return (m_chipbase != NULL);
}

/**
 * �r�W�[?
 * @retval true �r�W�[
 * @retval false ���f�B
 */
bool C86CtlIf::IsBusy()
{
	return false;
}

/**
 * �������Z�b�g
 */
void C86CtlIf::Reset()
{
	if (m_chip)
	{
		m_chip->reset();
	}
	if (m_gimic)
	{
		m_gimic->setPLLClock(7987200);
		m_gimic->setSSGVolume(31);
	}
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void C86CtlIf::WriteRegister(UINT nAddr, UINT8 cData)
{
	if (m_chip)
	{
		m_chip->out(nAddr, cData);
	}
}

/**
 * Has ADPCM?
 * @retval true Has
 * @retval false No exist
 */
bool C86CtlIf::HasADPCM()
{
	return m_bHasADPCM;
}
