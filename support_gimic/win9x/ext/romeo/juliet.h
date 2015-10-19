/**
 * @file	juliet.h
 * @brief	ROMEO アクセス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "..\externalchip.h"

/**
 * @brief ROMEO アクセス クラス
 */
class CJuliet
{
public:
	CJuliet();
	~CJuliet();
	bool Initialize();
	void Deinitialize();
	void Reset();
	IExternalChip* GetInterface(IExternalChip::ChipType nChipType, UINT nClock);
	bool IsEnabled() const;

private:
	//! @brief ロード関数
	struct ProcItem
	{
		LPCSTR lpSymbol;		//!< 関数名
		size_t nOffset;			//!< オフセット
	};

	// 定義
	typedef ULONG (WINAPI * FnRead32)(ULONG ulPciAddress, ULONG ulRegAddress);	//!< コンフィグレーション読み取り関数定義
	typedef VOID (WINAPI * FnOut8)(ULONG ulAddress, UCHAR ucParam);				//!< outp 関数定義
	typedef VOID (WINAPI * FnOut32)(ULONG ulAddress, ULONG ulParam);			//!< outpd 関数定義
	typedef UCHAR (WINAPI * FnIn8)(ULONG ulAddress);							//!< inp 関数定義

	HMODULE m_hModule;			//!< モジュール
	FnRead32 m_fnRead32;		//!< コンフィグレーション読み取り関数
	FnOut8 m_fnOut8;			//!< outp 関数
	FnOut32 m_fnOut32;			//!< outpd 関数
	FnIn8 m_fnIn8;				//!< inp 関数
	ULONG m_ulAddress;			//!< ROMEO ベース アドレス
	UCHAR m_ucIrq;				//!< ROMEO IRQ

	ULONG SearchRomeo() const;

	/**
	 * @brief チップ クラス
	 */
	class Chip288 : public IExternalChip
	{
		public:
			Chip288(CJuliet* pJuliet);
			virtual ~Chip288();
			virtual ChipType GetChipType();
			virtual void Reset();
			virtual void WriteRegister(UINT nAddr, UINT8 cData);
			virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

		private:
			CJuliet* m_pJuliet;			//!< 親インスタンス
			bool IsBusy() const;
	};
	IExternalChip* m_pChip288;		//!< YMF288 インスタンス

	void Detach(IExternalChip* pChip);
	friend class Chip288;
};

/**
 * ROMEO は有効?
 * @retval true 有効
 * @retval false 無効
 */
inline bool CJuliet::IsEnabled() const
{
	return (m_hModule != NULL);
}
