/**
 * @file	juliet.cpp
 * @brief	ROMEO �A�N�Z�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "juliet.h"
#include "romeo.h"

//! �B��̃C���X�^���X�ł�
CJuliet CJuliet::sm_instance;

//! DLL ��
#define	PCIDEBUG_DLL			_T("pcidebug.dll")

//! PCI �f�o�C�X �A�h���X���쐬
#define PCIBUSDEVFUNC(b, d, f)	(((b) << 8) | ((d) << 3) | (f))

//! �x���_/�f�o�C�X���쐬
#define	DEVVEND(v, d)			((ULONG)((v) | ((d) << 16)))

/**
 * �R���X�g���N�^
 */
CJuliet::CJuliet()
{
	Clear();
}

/**
 * �N���A
 */
void CJuliet::Clear()
{
	ZeroMemory(this, sizeof(*this));
	FillMemory(m_cTtl, sizeof(m_cTtl), 0x7f);
	m_cPsgMix = 0x3f;
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

	for (size_t i = 0; i < NELEMENTS(s_dllProc); i++)
	{
		FARPROC proc = ::GetProcAddress(m_hModule, s_dllProc[i].lpSymbol);
		if (proc == NULL)
		{
			Deinitialize();
			return false;
		}
		*(reinterpret_cast<FARPROC*>(reinterpret_cast<INTPTR>(this) + s_dllProc[i].nOffset)) = proc;
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

	m_bOpna = true;
	Reset();

	TRACEOUT(("ROMEO enable"));
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
	Clear();
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
bool CJuliet::IsBusy() const
{
	return (!m_bOpna) || (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0);

}

/**
 * �������Z�b�g
 */
void CJuliet::Reset() const
{
	if (m_bOpna)
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
	if (m_bOpna)
	{
		if (nAddr == 0x07)
		{
			// psg mix
			m_cPsgMix = cData;
		}
		else if ((nAddr & 0xf0) == 0x40)
		{
			// ttl
			m_cTtl[((nAddr & 0x100) >> 4) + (nAddr & 15)] = cData;
		}
		else if ((nAddr & 0xfc) == 0xb0)
		{
			// algorithm
			m_cAlgorithm[((nAddr & 0x100) >> 6) + (nAddr & 3)] = cData;
		}
		WriteRegisterInner(nAddr, cData);
	}
}

/**
 * �~���[�g
 * @param[in] bMute �~���[�g
 */
void CJuliet::Mute(bool bMute) const
{
	if (m_bOpna)
	{
		WriteRegisterInner(0x07, (bMute) ? 0x3f : m_cPsgMix);

		const int nVolume = (bMute) ? -127 : 0;
		for (UINT ch = 0; ch < 3; ch++)
		{
			SetVolume(ch + 0, nVolume);
			SetVolume(ch + 4, nVolume);
		}
	}
}

/**
 * ���W�X�^��������(����)
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CJuliet::WriteRegisterInner(UINT nAddr, UINT8 cData) const
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

/**
 * ���H�����[���ݒ�
 * @param[in] nChannel �`�����l��
 * @param[in] nVolume ���H�����[���l
 */
void CJuliet::SetVolume(UINT nChannel, int nVolume) const
{
	const UINT nBaseReg = (nChannel & 4) ? 0x140 : 0x40;

	//! �A���S���Y�� �X���b�g �}�X�N
	static const UINT8 s_opmask[] = {0x08, 0x08, 0x08, 0x08, 0x0c, 0x0e, 0x0e, 0x0f};
	UINT8 cMask = s_opmask[m_cAlgorithm[nChannel & 7] & 7];
	const UINT8* pTtl = m_cTtl + ((nChannel & 4) << 2);

	int nOffset = nChannel & 3;
	do
	{
		if (cMask & 1)
		{
			int nTtl = (pTtl[nOffset] & 0x7f) - nVolume;
			if (nTtl < 0)
			{
				nTtl = 0;
			}
			else if (nTtl > 0x7f)
			{
				nTtl = 0x7f;
			}
			WriteRegisterInner(nBaseReg + nOffset, static_cast<UINT8>(nTtl));
		}
		nOffset += 4;
		cMask >>= 1;
	} while (cMask != 0);
}

/**
 * ���W�X�^�����X�g�A����
 * @param[in] data �f�[�^
 * @param[in] bOpna OPNA ���W�X�^�����X�g�A����
 */
void CJuliet::Restore(const UINT8* data, bool bOpna)
{
	for (UINT i = 0x30; i < 0xa0; i++)
	{
		WriteRegister(i, data[i]);
	}
	for (UINT ch = 0; ch < 3; ch++)
	{
		WriteRegister(ch + 0xa4, data[ch + 0x0a4]);
		WriteRegister(ch + 0xa0, data[ch + 0x0a0]);
		WriteRegister(ch + 0xb0, data[ch + 0x0b0]);
		WriteRegister(ch + 0xb4, data[ch + 0x0b4]);
	}

	if (bOpna)
	{
		for (UINT i = 0x130; i < 0x1a0; i++)
		{
			WriteRegister(i, data[i]);
		}
		for (UINT ch = 0; ch < 3; ch++)
		{
			WriteRegister(ch + 0x1a4, data[ch + 0x1a4]);
			WriteRegister(ch + 0x1a0, data[ch + 0x1a0]);
			WriteRegister(ch + 0x1b0, data[ch + 0x1b0]);
			WriteRegister(ch + 0x1b4, data[ch + 0x1b4]);
		}
		WriteRegister(0x11, data[0x11]);
		WriteRegister(0x18, data[0x18]);
		WriteRegister(0x19, data[0x19]);
		WriteRegister(0x1a, data[0x1a]);
		WriteRegister(0x1b, data[0x1b]);
		WriteRegister(0x1c, data[0x1c]);
		WriteRegister(0x1d, data[0x1d]);
	}

	for (UINT i = 0; i < 0x0e; i++)
	{
		WriteRegister(i, data[i]);
	}
}
