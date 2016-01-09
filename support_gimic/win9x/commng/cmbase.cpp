/**
 * @file	cmbase.h
 * @brief	commng 基底クラスの動作の定義を行います
 */

#include "compiler.h"
#include "cmbase.h"

/**
 * コンストラクタ
 */
CComBase::CComBase(UINT cConnect)
{
	this->connect = cConnect;
	this->read = cRead;
	this->write = cWrite;
	this->getstat = cGetStat;
	this->msg = cMessage;
	this->release = cRelease;
}

/**
 * デストラクタ
 */
CComBase::~CComBase()
{
}

/**
 * Read
 * @param[out] pData
 * @return result
 */
UINT CComBase::cRead(COMMNG cm, UINT8* pData)
{
	return static_cast<CComBase*>(cm)->Read(pData);
}

/**
 * Write
 * @param[in] cData
 * @return result
 */
UINT CComBase::cWrite(COMMNG cm, UINT8 cData)
{
	return static_cast<CComBase*>(cm)->Write(cData);
}

/**
 * ステータスを得る
 * @return ステータス
 */
UINT8 CComBase::cGetStat(COMMNG cm)
{
	return static_cast<CComBase*>(cm)->GetStat();
}

/**
 * メッセージ
 */
INTPTR CComBase::cMessage(COMMNG cm, UINT msg, INTPTR param)
{
	return static_cast<CComBase*>(cm)->Message(msg, param);
}

/**
 * リリース
 */
void CComBase::cRelease(COMMNG cm)
{
	delete static_cast<CComBase*>(cm);
}
