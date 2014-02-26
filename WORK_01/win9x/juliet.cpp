/**
 * @file	juliet.cpp
 * @brief	ROMEO アクセス クラス
 */

#include	"compiler.h"

#if defined(SUPPORT_ROMEO)

#include	"romeo.h"
#include	"juliet.h"

/**
 * @brief ROMEO アクセス クラス
 */
class CJuliet
{
public:
	CJuliet();
	bool Initialize();
	void Deinitialize();
	bool IsEnabled() const;
	bool IsBusy() const;
	void Reset() const;
	void WriteRegister(UINT nAddr, UINT8 cData);
	void WriteExtendRegister(UINT nAddr, UINT8 cData);
	void Mute(bool bMute) const;

public:
	HMODULE m_hModule;														//!< モジュール
	ULONG (WINAPI * m_fnRead32)(ULONG ulPciAddress, ULONG ulRegAddress);	//!< コンフィグレーション読み取り関数
	VOID (WINAPI * m_fnOut8)(ULONG ulAddress, UCHAR ucParam);				//!< outp 関数
	VOID (WINAPI * m_fnOut32)(ULONG ulAddress, ULONG ulParam);				//!< outpd 関数
	UCHAR (WINAPI * m_fnIn8)(ULONG ulAddress);								//!< inp 関数
	bool m_bOpna;															//!< OPNA 有効フラグ
	UCHAR m_ucIrq;															//!< ROMEO IRQ
	UINT8 m_cPsgMix;														//!< PSG ミキサー
	ULONG m_ulAddress;														//!< ROMEO ベース アドレス
	UINT8 m_cAlgorithm[8];													//!< アルゴリズム テーブル
	UINT8 m_cTtl[8 * 4];													//!< TTL テーブル

	void Clear();
	ULONG SearchRomeo() const;
	void WriteRegisterInner(UINT8 cAddr, UINT8 cData) const;
	void WriteExtendRegisterInner(UINT8 cAddr, UINT8 cData) const;
	void SetVolume(UINT nChannel, int nVolume) const;
};

//! アルゴリズム スロット マスク
static const UINT8 s_opmask[] = {0x08, 0x08, 0x08, 0x08, 0x0c, 0x0e, 0x0e, 0x0f};

//! @brief ロード関数
struct ProcItem
{
	LPCSTR lpSymbol;		//!< 関数名
	size_t nOffset;			//!< オフセット
};

//! ロード関数リスト
static const ProcItem s_dllProc[] =
{
	{FN_PCICFGREAD32,	offsetof(CJuliet, m_fnRead32)},
	{FN_PCIMEMWR8,		offsetof(CJuliet, m_fnOut8)},
	{FN_PCIMEMWR32,		offsetof(CJuliet, m_fnOut32)},
	{FN_PCIMEMRD8,		offsetof(CJuliet, m_fnIn8)},
};

/**
 * コンストラクタ
 */
CJuliet::CJuliet()
{
	Clear();
}

/**
 * クリア
 */
void CJuliet::Clear()
{
	ZeroMemory(this, sizeof(*this));
	FillMemory(m_cTtl, sizeof(m_cTtl), 0x7f);
	m_cPsgMix = 0x3f;
}

/**
 * 初期化
 * @retval true 成功
 * @retval false 失敗
 */
bool CJuliet::Initialize()
{
	Deinitialize();

	m_hModule = ::LoadLibrary(PCIDEBUG_DLL);
	if (m_hModule == NULL)
	{
		return false;
	}

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
 * 解放
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
 * ROMEO を探す
 * @note pciFindPciDevice使うと、OS起動後一発目に見つけられないことが多いので、自前で検索する（矢野さん方式）
 * @return バス アドレス
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
 * ROMEO は有効?
 * @retval true 有効
 * @retval false 無効
 */
bool CJuliet::IsEnabled() const
{
	return (m_hModule != NULL);
}

/**
 * ビジー?
 * @retval true ビジー
 * @retval false レディ
 */
bool CJuliet::IsBusy() const
{
	return (!m_bOpna) || (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0);

}

/**
 * 音源リセット
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
 * レジスタ書き込み
 * @param[in] nAddr アドレス
 * @param[in] cData データ
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
		else if ((nAddr & (~15)) == 0x40)
		{
			// ttl
			m_cTtl[nAddr & 15] = cData;
		}
		else if ((nAddr & (~3)) == 0xb0)
		{
			// algorithm
			m_cAlgorithm[nAddr & 3] = cData;
		}
		WriteRegisterInner(nAddr, cData);
	}
}

/**
 * 拡張レジスタ書き込み
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CJuliet::WriteExtendRegister(UINT nAddr, UINT8 cData)
{
	if (m_bOpna)
	{
		if ((nAddr & (~15)) == 0x40)
		{
			// ttl
			m_cTtl[0x10 + (nAddr & 15)] = cData;
		}
		else if ((nAddr & (~3)) == 0xb0)
		{
			// algorithm
			m_cAlgorithm[4 + (nAddr & 3)] = cData;
		}
		WriteExtendRegisterInner(nAddr, cData);
	}
}

/**
 * ミュート
 * @param[in] bMute ミュート
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
 * レジスタ書き込み(内部)
 * @param[in] cAddr アドレス
 * @param[in] cData データ
 */
void CJuliet::WriteRegisterInner(UINT8 cAddr, UINT8 cData) const
{
	while (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
	{
		::Sleep(0);
	}
	(*m_fnOut8)(m_ulAddress + ROMEO_YMF288ADDR1, cAddr);

	while (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
	{
		::Sleep(0);
	}
	(*m_fnOut8)(m_ulAddress + ROMEO_YMF288DATA1, cData);
}

/**
 * 拡張レジスタ書き込み(内部)
 * @param[in] cAddr アドレス
 * @param[in] cData データ
 */
void CJuliet::WriteExtendRegisterInner(UINT8 cAddr, UINT8 cData) const
{
	while (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
	{
		::Sleep(0);
	}
	(*m_fnOut8)(m_ulAddress + ROMEO_YMF288ADDR2, cAddr);

	while (((*m_fnIn8)(m_ulAddress + ROMEO_YMF288ADDR1) & 0x80) != 0)
	{
		::Sleep(0);
	}
	(*m_fnOut8)(m_ulAddress + ROMEO_YMF288DATA2, cData);
}

/**
 * ヴォリューム設定
 * @param[in] nChannel チャンネル
 * @param[in] nVolume ヴォリューム値
 */
void CJuliet::SetVolume(UINT nChannel, int nVolume) const
{
	// 書き込み関数
	void (CJuliet::*fnSetReg)(UINT8 cAddr, UINT8 cData) const = (nChannel & 4) ? &CJuliet::WriteExtendRegisterInner : &CJuliet::WriteRegisterInner;

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
			(this->*fnSetReg)(0x40 + nOffset, static_cast<UINT8>(nTtl));
		}
		nOffset += 4;
		cMask >>= 1;
	} while (cMask != 0);
}


// ----

static CJuliet romeo;

BOOL juliet_initialize(void)
{
	return romeo.Initialize() ? SUCCESS : FAILURE;
}

void juliet_deinitialize(void)
{
	romeo.Deinitialize();
}

void juliet_YMF288Reset(void)
{
	romeo.Reset();
}

BOOL juliet_YMF288IsEnable(void)
{
	return romeo.IsEnabled();
}

BOOL juliet_YMF288IsBusy(void)
{
	return romeo.IsBusy();
}

void juliet_YMF288A(UINT addr, UINT8 data)
{
	romeo.WriteRegister(addr, data);
}

void juliet_YMF288B(UINT addr, UINT8 data)
{
	romeo.WriteExtendRegister(addr, data);
}

void juliet_YMF288Enable(BOOL enable)
{
	romeo.Mute(!enable);
}

#endif
