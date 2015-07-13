/**
 * @file	tty.h
 * @brief	シリアル通信クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * @brief シリアル通信
 */
class CTty
{
public:
	CTty();
	~CTty();
	bool Open(int nPort, UINT nSpeed = 0, LPCTSTR lpcszParam = NULL);
	bool IsOpened() const;
	void Close();
	int Read(LPVOID lpcvData, int nDataSize);
	int Write(LPCVOID lpcvData, int nDataSize);

private:
	HANDLE m_hFile;				/*!< ファイル ハンドル */
	static bool SetParam(LPCTSTR lpcszParam, DCB* dcb = NULL);
	static LPCTSTR GetBoolString(BOOL f);
};

/**
 * オープン済?
 * @retval true オープン済
 * @retval false 未オープン
 */
inline bool CTty::IsOpened() const
{
	return (m_hFile != INVALID_HANDLE_VALUE);
}

/**
 * TRUE/FALSE 文字列を返す
 * @param[in] f BOOL値
 * @return 文字列ポインタ
 */
inline LPCTSTR CTty::GetBoolString(BOOL f)
{
	return (f) ? TEXT("TRUE") : TEXT("FALSE");
}
