/**
 * @file	ttyl.cpp
 * @brief	シリアル通信クラスの動作の定義を行います
 */

#include "compiler.h"
#include "tty.h"
#include <algorithm>
#include <setupapi.h>
#include <tchar.h>

#pragma comment(lib, "setupapi.lib")

/**
 * コンストラクタ
 */
CTty::CTty()
	: m_hFile(INVALID_HANDLE_VALUE)
{
}

/**
 * デストラクタ
 */
CTty::~CTty()
{
	Close();
}

/**
 * オープンする
 * @param[in] nPort ポート番号
 * @param[in] nSpeed ボーレート
 * @param[in] lpcszParam パラメタ
 * @retval true 成功
 * @retval false 失敗
 */
bool CTty::Open(int nPort, UINT nSpeed, LPCTSTR lpcszParam)
{
	Close();

	TCHAR szPort[16];
	::wsprintf(szPort, TEXT("COM%u"), nPort);
	return OpenPort(szPort, nSpeed, lpcszParam);
}

/**
 * オープンする
 * @param[in] lpDevName デバイス名
 * @param[in] nSpeed ボーレート
 * @param[in] lpcszParam パラメタ
 * @retval true 成功
 * @retval false 失敗
 */
bool CTty::Open(LPCTSTR lpDevName, UINT nSpeed, LPCTSTR lpcszParam)
{
	Close();

	TCHAR szPortName[MAX_PATH];
	if (!GetPortName(lpDevName, szPortName, _countof(szPortName)))
	{
		return false;
	}
	return OpenPort(szPortName, nSpeed, lpcszParam);
}

/**
 * ポート オープンする
 * @param[in] lpPortName ポート名
 * @param[in] nSpeed ボーレート
 * @param[in] lpcszParam パラメタ
 * @retval true 成功
 * @retval false 失敗
 */
bool CTty::OpenPort(LPCTSTR lpPortName, UINT nSpeed, LPCTSTR lpcszParam)
{
	if (!SetParam(lpcszParam, NULL))
	{
		return false;
	}

	HANDLE hFile = ::CreateFile(lpPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DCB dcb;
	::GetCommState(hFile, &dcb);
	if (nSpeed != 0)
	{
		dcb.BaudRate = nSpeed;
	}
	SetParam(lpcszParam, &dcb);

	dcb.fOutxCtsFlow = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;

	if (!::SetCommState(hFile, &dcb))
	{
		::CloseHandle(hFile);
		return false;
	}

	m_hFile = hFile;
	return true;
}

/**
 * クローズする
 */
void CTty::Close()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

/**
 * データ受信
 * @param[in] lpcvData 送信データのポインタ
 * @param[in] nDataSize 送信データのサイズ
 * @return 送信バイト数
 */
int CTty::Read(LPVOID lpcvData, int nDataSize)
{
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	if ((lpcvData == NULL) || (nDataSize <= 0))
	{
		return 0;
	}

	DWORD dwErrors;
	COMSTAT stat;
	if (!::ClearCommError(m_hFile, &dwErrors, &stat))
	{
		return 0;
	}

	DWORD dwReadLength = std::min(stat.cbInQue, static_cast<DWORD>(nDataSize));
	if (dwReadLength == 0)
	{
		return 0;
	}

	DWORD dwReadSize = 0;
	if (!::ReadFile(m_hFile, lpcvData, dwReadLength, &dwReadSize, NULL))
	{
		return 0;
	}
	return static_cast<int>(dwReadSize);
}

/**
 * データ送信
 * @param[in] lpcvData 送信データのポインタ
 * @param[in] nDataSize 送信データのサイズ
 * @return 送信バイト数
 */
int CTty::Write(LPCVOID lpcvData, int nDataSize)
{
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	if ((lpcvData == NULL) || (nDataSize <= 0))
	{
		return 0;
	}

	DWORD dwWrittenSize = 0;
	if (!::WriteFile(m_hFile, lpcvData, nDataSize, &dwWrittenSize, NULL))
	{
		// DEBUGLOG(_T("Failed to write."));
		return 0;
	}
	return static_cast<int>(dwWrittenSize);
}

/**
 * パラメータ設定
 * @param[in] lpcszParam パラメタ
 * @param[in, out] dcb DCB 構造体のポインタ
 * @retval true 成功
 * @retval false 失敗
 */
bool CTty::SetParam(LPCTSTR lpcszParam, DCB* dcb)
{
	BYTE cByteSize = 8;
	BYTE cParity = NOPARITY;
	BYTE cStopBits = ONESTOPBIT;

	if (lpcszParam != NULL)
	{
		TCHAR c = lpcszParam[0];
		if ((c < TEXT('4')) || (c > TEXT('8')))
		{
			return false;
		}
		cByteSize = static_cast<BYTE>(c - TEXT('0'));

		c = lpcszParam[1];
		switch (c & (~0x20))
		{
			case TEXT('N'):		// for no parity
				cParity = NOPARITY;
				break;

			case TEXT('E'):		// for even parity
				cParity = EVENPARITY;
				break;

			case TEXT('O'):		// for odd parity
				cParity = ODDPARITY;
				break;

			case TEXT('M'):		// for mark parity
				cParity = MARKPARITY;
				break;

			case TEXT('S'):		// for for space parity
				cParity = SPACEPARITY;
				break;

			default:
				return false;
		}

		if (::lstrcmp(lpcszParam + 2, TEXT("1")) == 0)
		{
			cStopBits = ONESTOPBIT;
		}
		else if (::lstrcmp(lpcszParam + 2, TEXT("1.5")) == 0)
		{
			cStopBits = ONE5STOPBITS;
		}
		else if (::lstrcmp(lpcszParam + 2, TEXT("2")) == 0)
		{
			cStopBits = TWOSTOPBITS;
		}
		else
		{
			return false;
		}
	}

	if (dcb != NULL)
	{
		dcb->ByteSize = cByteSize;
		dcb->Parity = cParity;
		dcb->StopBits = cStopBits;
	}
	return true;
}

/**
 * ポート名を得る
 * @param[in] lpDevName デバイス名
 * @param[out] lpPortName ポート名
 * @paramin] cchPortName ポート名長
 * @retval true 発見した
 * @retval false 発見できなかった
 */
bool CTty::GetPortName(LPCTSTR lpDevName, LPTSTR lpPortName, UINT cchPortName)
{
	bool ret = false;

	GUID ClassGuid[1];
	DWORD dwRequiredSize;
	if (::SetupDiClassGuidsFromName(TEXT("PORTS"), ClassGuid, _countof(ClassGuid), &dwRequiredSize))
	{
		HDEVINFO DeviceInfoSet = ::SetupDiGetClassDevs(&ClassGuid[0], NULL, NULL, DIGCF_PRESENT | DIGCF_PROFILE);
		if (DeviceInfoSet)
		{
			DWORD dwMemberIndex = 0;
			SP_DEVINFO_DATA DeviceInfoData;
			ZeroMemory(&DeviceInfoData, sizeof(DeviceInfoData));
			DeviceInfoData.cbSize = sizeof(DeviceInfoData);
			while (::SetupDiEnumDeviceInfo(DeviceInfoSet, dwMemberIndex++, &DeviceInfoData))
			{
				DWORD dwPropType;
				TCHAR szFriendlyName[MAX_PATH];
				DWORD dwReqSize = 0;
				::SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, &dwPropType, reinterpret_cast<LPBYTE>(szFriendlyName), sizeof(szFriendlyName), &dwReqSize);

				HKEY hKey = ::SetupDiOpenDevRegKey(DeviceInfoSet, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
				if (hKey)
				{
					TCHAR szPortName[MAX_PATH];
					DWORD dwType = REG_SZ;
					dwReqSize = sizeof(szPortName);
					long lRet = RegQueryValueEx(hKey, TEXT("PortName"), 0, &dwType, reinterpret_cast<LPBYTE>(szPortName), &dwReqSize);
					::RegCloseKey(hKey);

					if (_tcsnicmp(szFriendlyName, lpDevName, _tcslen(lpDevName)) == 0)
					{
						_tcscpy_s(lpPortName, cchPortName, szPortName);
						ret = true;
						break;
					}
				}
			}
		}
		::SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	}

	return ret;
}
