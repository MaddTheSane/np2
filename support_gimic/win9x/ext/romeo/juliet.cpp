/**
 * @file	juliet.cpp
 * @brief	ROMEO �A�N�Z�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "juliet.h"
#include <stddef.h>
#include "romeo.h"

//! DLL ��
#define	PCIDEBUG_DLL			TEXT("pcidebug.dll")

//! PCI �f�o�C�X �A�h���X���쐬
#define PCIBUSDEVFUNC(b, d, f)	(((b) << 8) | ((d) << 3) | (f))

//! �x���_/�f�o�C�X���쐬
#define	DEVVEND(v, d)			((ULONG)((v) | ((d) << 16)))

/**
 * �R���X�g���N�^
 */
CJuliet::CJuliet()
	: m_hModule(NULL)
	, m_fnRead32(NULL)
	, m_fnOut8(NULL)
	, m_fnOut32(NULL)
	, m_fnIn8(NULL)
	, m_ulAddress(0)
	, m_ucIrq(0)
	, m_pChip288(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CJuliet::~CJuliet()
{
}

/**
 * ������
 * @retval true ����
 * @retval false ���s
 */
bool CJuliet::Initialize()
{
	if (m_hModule)
	{
		return false;
	}

	m_hModule = ::LoadLibrary(PCIDEBUG_DLL);
	if (m_hModule == NULL)
	{
		return false;
	}

	//! ���[�h�֐����X�g
	static const ProcItem s_dllProc[] =
	{
		{"_pciConfigReadLong",	offsetof(CJuliet, m_fnRead32)},
		{"_MemWriteChar",		offsetof(CJuliet, m_fnOut8)},
		{"_MemWriteLong",		offsetof(CJuliet, m_fnOut32)},
		{"_MemReadChar",		offsetof(CJuliet, m_fnIn8)},
	};

	for (size_t i = 0; i < _countof(s_dllProc); i++)
	{
		FARPROC proc = ::GetProcAddress(m_hModule, s_dllProc[i].lpSymbol);
		if (proc == NULL)
		{
			Deinitialize();
			return false;
		}
		*(reinterpret_cast<FARPROC*>(reinterpret_cast<INT_PTR>(this) + s_dllProc[i].nOffset)) = proc;
	}

	const ULONG ulPciAddress = SearchRomeo();
	if (ulPciAddress == static_cast<ULONG>(-1))
	{
		Deinitialize();
		return false;
	}
	m_ulAddress = (*m_fnRead32)(ulPciAddress, ROMEO_BASEADDRESS1);
	m_ucIrq = static_cast<UCHAR>((*m_fnRead32)(ulPciAddress, ROMEO_PCIINTERRUPT));
	if (!m_ulAddress)
	{
		Deinitialize();
		return false;
	}

	Reset();

	return true;
}

/**
 * ���
 */
void CJuliet::Deinitialize()
{
	if (m_pChip288)
	{
		delete m_pChip288;
	}

	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
		m_fnRead32 = NULL;
		m_fnOut8 = NULL;
		m_fnOut32 = NULL;
		m_fnIn8 = NULL;
		m_ulAddress = 0;
		m_ucIrq = 0;
	}
}

/**
 * ROMEO ��T��
 * @note pciFindPciDevice�g���ƁAOS�N����ꔭ�ڂɌ������Ȃ����Ƃ������̂ŁA���O�Ō�������i��삳������j
 * @return �o�X �A�h���X
 */
ULONG CJuliet::SearchRomeo() const
{
	for (UINT bus = 0; bus < 0x100; bus++)
	{
		for (UINT dev = 0; dev < 0x20; dev++)
		{
			for (UINT func = 0; func < 0x08; func++)
			{
				const ULONG ulPciAddress = PCIBUSDEVFUNC(bus, dev, func);
				const ULONG ulDeviceVendor = (*m_fnRead32)(ulPciAddress, 0x0000);
				if ((ulDeviceVendor == DEVVEND(ROMEO_VENDORID, ROMEO_DEVICEID)) || (ulDeviceVendor == DEVVEND(ROMEO_VENDORID, ROMEO_DEVICEID2)))
				{
					return ulPciAddress;
				}
			}
		}
	}
	return static_cast<ULONG>(-1);
}

/**
 * �������Z�b�g
 */
void CJuliet::Reset()
{
	if (m_fnOut32 != NULL)
	{
		(*m_fnOut32)(m_ulAddress + ROMEO_YMF288CTRL, 0x00);
		::Sleep(150);

		(*m_fnOut32)(m_ulAddress + ROMEO_YMF288CTRL, 0x80);
		::Sleep(150);
	}
}

/**
 * �C���^�[�t�F�C�X�擾
 * @param[in] nChipType �^�C�v
 * @param[in] nClock �N���b�N
 * @return �C���X�^���X
 */
IExternalChip* CJuliet::GetInterface(IExternalChip::ChipType nChipType, UINT nClock)
{
	const bool bInitialized = Initialize();

	do
	{
		if (m_hModule == NULL)
		{
			break;
		}

		if ((nChipType == IExternalChip::kYMF288) && (m_pChip288 == NULL))
		{
			m_pChip288 = new Chip288(this);
			return m_pChip288;
		}
	} while (false /*CONSTCOND*/);

	if (bInitialized)
	{
//		Deinitialize();
	}

	return NULL;
}

/**
 * ���
 * @param[in] pChip �`�b�v
 */
void CJuliet::Detach(IExternalChip* pChip)
{
	if (m_pChip288 == pChip)
	{
		m_pChip288 = NULL;
	}
}



// ---- �`�b�v

/**
 * �R���X�g���N�^
 * @param[in] pScciIf �e�C���X�^���X
 * @param[in] pChip �`�b�v �C���X�^���X
 */
CJuliet::Chip288::Chip288(CJuliet* pJuliet)
	: m_pJuliet(pJuliet)
{
}

/**
 * �f�X�g���N�^
 */
CJuliet::Chip288::~Chip288()
{
	m_pJuliet->Detach(this);
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType CJuliet::Chip288::GetChipType()
{
	return IExternalChip::kYMF288;
}

/**
 * ���Z�b�g
 */
void CJuliet::Chip288::Reset()
{
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CJuliet::Chip288::WriteRegister(UINT nAddr, UINT8 cData)
{
	CJuliet* pJuliet = m_pJuliet;
	ULONG ulAddress = pJuliet->m_ulAddress;
	if (ulAddress != 0)
	{
		FnOut8 fnOut8 = pJuliet->m_fnOut8;
		FnIn8 fnIn8 = pJuliet->m_fnIn8;
		while (((*fnIn8)(ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
		{
			::Sleep(0);
		}
		(*fnOut8)(ulAddress + ((nAddr & 0x100) ? ROMEO_YMF288ADDR2 : ROMEO_YMF288ADDR1), nAddr);

		while (((*fnIn8)(ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
		{
			::Sleep(0);
		}
		(*fnOut8)(ulAddress + ((nAddr & 0x100) ? ROMEO_YMF288DATA2 : ROMEO_YMF288DATA1), cData);
	}
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParameter �p�����[�^
 * @return ���U���g
 */
INTPTR CJuliet::Chip288::Message(UINT nMessage, INTPTR nParameter)
{
	if (nMessage == kBusy)
	{
		return IsBusy() ? 1 : 0;
	}
	return 0;
}

/**
 * �r�W�[?
 * @retval true �r�W�[
 * @retval false �r�W�[�ł͂Ȃ�
 */
bool CJuliet::Chip288::IsBusy() const
{
	CJuliet* pJuliet = m_pJuliet;
	ULONG ulAddress = pJuliet->m_ulAddress;
	FnIn8 fnIn8 = pJuliet->m_fnIn8;

	return (fnIn8 == NULL) || (((*fnIn8)(ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0);
}
