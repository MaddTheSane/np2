/**
 * @file	cmserial.cpp
 * @brief	シリアル クラスの動作の定義を行います
 */

#include "compiler.h"
#include "cmserial.h"

const UINT32 cmserial_speed[10] = {110, 300, 1200, 2400, 4800,
							9600, 19200, 38400, 57600, 115200};

CComSerial* CComSerial::CreateInstance(UINT nPort, UINT8 cParam, UINT32 nSpeed)
{
	CComSerial* pSerial = new CComSerial;
	if (!pSerial->Initialize(nPort, cParam, nSpeed))
	{
		delete pSerial;
		pSerial = NULL;
	}
	return pSerial;
}

/**
 * コンストラクタ
 */
CComSerial::CComSerial()
	: CComBase(COMCONNECT_SERIAL)
	, m_hSerial(INVALID_HANDLE_VALUE)
{
}

/**
 * デストラクタ
 */
CComSerial::~CComSerial()
{
	if (m_hSerial != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hSerial);
	}
}

bool CComSerial::Initialize(UINT nPort, UINT8 cParam, UINT32 nSpeed)
{
	TCHAR szName[16];
	wsprintf(szName, TEXT("COM%u"), nPort);
	m_hSerial = CreateFile(szName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	if (m_hSerial == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DCB dcb;
	::GetCommState(m_hSerial, &dcb);
	for (UINT i = 0; i < NELEMENTS(cmserial_speed); i++)
	{
		if (cmserial_speed[i] >= nSpeed)
		{
			dcb.BaudRate = cmserial_speed[i];
			break;
		}
	}
	dcb.ByteSize = (UINT8)(((cParam >> 2) & 3) + 5);
	switch (cParam & 0x30)
	{
		case 0x10:
			dcb.Parity = ODDPARITY;
			break;

		case 0x30:
			dcb.Parity = EVENPARITY;
			break;

		default:
			dcb.Parity = NOPARITY;
			break;
	}
	switch (cParam & 0xc0)
	{
		case 0x80:
			dcb.StopBits = ONE5STOPBITS;
			break;

		case 0xc0:
			dcb.StopBits = TWOSTOPBITS;
			break;

		default:
			dcb.StopBits = ONESTOPBIT;
			break;
	}
	::SetCommState(m_hSerial, &dcb);
	return true;
}

UINT CComSerial::Read(UINT8* pData)
{
	DWORD err;
	COMSTAT ct;
	::ClearCommError(m_hSerial, &err, &ct);
	if (ct.cbInQue)
	{
		DWORD dwReadSize;
		if (::ReadFile(m_hSerial, pData, 1, &dwReadSize, NULL))
		{
			return 1;
		}
	}
	return 0;
}

UINT CComSerial::Write(UINT8 cData)
{
	DWORD dwWrittenSize;
	return (::WriteFile(m_hSerial, &cData, 1, &dwWrittenSize, NULL)) ? 1 : 0;
}

UINT8 CComSerial::GetStat()
{
	DCB dcb;
	::GetCommState(m_hSerial, &dcb);
	if (!dcb.fDsrSensitivity)
	{
		return 0x20;
	}
	else
	{
		return 0x00;
	}
}

INTPTR CComSerial::Message(UINT msg, INTPTR param)
{
	return 0;
}
