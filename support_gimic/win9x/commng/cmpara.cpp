/**
 * @file	cmpara.cpp
 * @brief	パラレル クラスの動作の定義を行います
 */

#include "compiler.h"
#include "cmpara.h"

CComPara* CComPara::CreateInstance(UINT nPort)
{
	CComPara* pPara = new CComPara;
	if (!pPara->Initialize(nPort))
	{
		delete pPara;
		pPara = NULL;
	}
	return pPara;
}

/**
 * コンストラクタ
 */
CComPara::CComPara()
	: CComBase(COMCONNECT_PARALLEL)
	, m_hParallel(INVALID_HANDLE_VALUE)
{
}

/**
 * デストラクタ
 */
CComPara::~CComPara()
{
	if (m_hParallel != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hParallel);
	}
}

bool CComPara::Initialize(UINT nPort)
{
	TCHAR szName[16];
	wsprintf(szName, TEXT("LPT%u"), nPort);
	m_hParallel = CreateFile(szName, GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	return (m_hParallel != INVALID_HANDLE_VALUE);
}

UINT CComPara::Read(UINT8* pData)
{
	return 0;
}

UINT CComPara::Write(UINT8 cData)
{
	DWORD dwWrittenSize;
	return (::WriteFile(m_hParallel, &cData, 1, &dwWrittenSize, NULL)) ? 1 : 0;
}

UINT8 CComPara::GetStat()
{
	return 0x00;
}

INTPTR CComPara::Message(UINT msg, INTPTR param)
{
	return 0;
}
