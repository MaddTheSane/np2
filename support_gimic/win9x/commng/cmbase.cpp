/**
 * @file	cmbase.h
 * @brief	commng ���N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmbase.h"

/**
 * �R���X�g���N�^
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
 * �f�X�g���N�^
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
 * �X�e�[�^�X�𓾂�
 * @return �X�e�[�^�X
 */
UINT8 CComBase::cGetStat(COMMNG cm)
{
	return static_cast<CComBase*>(cm)->GetStat();
}

/**
 * ���b�Z�[�W
 */
INTPTR CComBase::cMessage(COMMNG cm, UINT msg, INTPTR param)
{
	return static_cast<CComBase*>(cm)->Message(msg, param);
}

/**
 * �����[�X
 */
void CComBase::cRelease(COMMNG cm)
{
	delete static_cast<CComBase*>(cm);
}
