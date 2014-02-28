/**
 * @file	juliet.cpp
 * @brief	ROMEO �A�N�Z�X �N���X
 */

#include	"compiler.h"

#if defined(SUPPORT_ROMEO)

#include	"romeo.h"
#include	"juliet.h"

/**
 * @brief ROMEO �A�N�Z�X �N���X
 */
class CJuliet
{
public:
	static CJuliet* GetInstance();
	CJuliet();
	bool Initialize();
	void Deinitialize();
	bool IsEnabled() const;
	bool IsBusy() const;
	void Reset() const;
	void WriteRegister(UINT nAddr, UINT8 cData);
	void Mute(bool bMute) const;

private:
	//! @brief ���[�h�֐�
	struct ProcItem
	{
		LPCSTR lpSymbol;		//!< �֐���
		size_t nOffset;			//!< �I�t�Z�b�g
	};

	static CJuliet sm_instance;												//!< �B��̃C���X�^���X�ł�

	HMODULE m_hModule;														//!< ���W���[��
	ULONG (WINAPI * m_fnRead32)(ULONG ulPciAddress, ULONG ulRegAddress);	//!< �R���t�B�O���[�V�����ǂݎ��֐�
	VOID (WINAPI * m_fnOut8)(ULONG ulAddress, UCHAR ucParam);				//!< outp �֐�
	VOID (WINAPI * m_fnOut32)(ULONG ulAddress, ULONG ulParam);				//!< outpd �֐�
	UCHAR (WINAPI * m_fnIn8)(ULONG ulAddress);								//!< inp �֐�
	bool m_bOpna;															//!< OPNA �L���t���O
	UCHAR m_ucIrq;															//!< ROMEO IRQ
	UINT8 m_cPsgMix;														//!< PSG �~�L�T�[
	ULONG m_ulAddress;														//!< ROMEO �x�[�X �A�h���X
	UINT8 m_cAlgorithm[8];													//!< �A���S���Y�� �e�[�u��
	UINT8 m_cTtl[8 * 4];													//!< TTL �e�[�u��

	void Clear();
	ULONG SearchRomeo() const;
	void WriteRegisterInner(UINT nAddr, UINT8 cData) const;
	void SetVolume(UINT nChannel, int nVolume) const;
};

//! �B��̃C���X�^���X�ł�
CJuliet CJuliet::sm_instance;

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline CJuliet* CJuliet::GetInstance()
{
	return &sm_instance;
}

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
		{FN_PCICFGREAD32,	offsetof(CJuliet, m_fnRead32)},
		{FN_PCIMEMWR8,		offsetof(CJuliet, m_fnOut8)},
		{FN_PCIMEMWR32,		offsetof(CJuliet, m_fnOut32)},
		{FN_PCIMEMRD8,		offsetof(CJuliet, m_fnIn8)},
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
	#define PCIBUSDEVFUNC(b, d, f)	(((b) << 8) | ((d) << 3) | (f))
	#define	DEVVEND(v, d)			((ULONG)((v) | ((d) << 16)))

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
 * ROMEO �͗L��?
 * @retval true �L��
 * @retval false ����
 */
bool CJuliet::IsEnabled() const
{
	return (m_hModule != NULL);
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


// ----

BOOL juliet_initialize(void)
{
	return CJuliet::GetInstance()->Initialize() ? SUCCESS : FAILURE;
}

void juliet_deinitialize(void)
{
	CJuliet::GetInstance()->Deinitialize();
}

void juliet_YMF288Reset(void)
{
	CJuliet::GetInstance()->Reset();
}

BOOL juliet_YMF288IsEnable(void)
{
	return CJuliet::GetInstance()->IsEnabled();
}

BOOL juliet_YMF288IsBusy(void)
{
	return CJuliet::GetInstance()->IsBusy();
}

void juliet_YMF288A(UINT addr, UINT8 data)
{
	CJuliet::GetInstance()->WriteRegister(addr, data);
}

void juliet_YMF288B(UINT addr, UINT8 data)
{
	CJuliet::GetInstance()->WriteRegister(addr + 0x100, data);
}

void juliet_YMF288Enable(BOOL enable)
{
	CJuliet::GetInstance()->Mute(!enable);
}

#endif
