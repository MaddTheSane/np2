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
	Deinitialize();

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
	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
	}
	m_hModule = NULL;
	m_fnRead32 = NULL;
	m_fnOut8 = NULL;
	m_fnOut32 = NULL;
	m_fnIn8 = NULL;
	m_ulAddress = 0;
	m_ucIrq = 0;
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
 * �r�W�[?
 * @retval true �r�W�[
 * @retval false ���f�B
 */
bool CJuliet::IsBusy()
{
	return (m_fnIn8 == NULL) || (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0);

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
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CJuliet::WriteRegister(UINT nAddr, UINT8 cData)
{
	if (m_ulAddress != 0)
	{
		while (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
		{
			::Sleep(0);
		}
		(*m_fnOut8)(m_ulAddress + ((nAddr & 0x100) ? ROMEO_YMF288ADDR2 : ROMEO_YMF288ADDR1), nAddr);

		while (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
		{
			::Sleep(0);
		}
		(*m_fnOut8)(m_ulAddress + ((nAddr & 0x100) ? ROMEO_YMF288DATA2 : ROMEO_YMF288DATA1), cData);
	}
}
