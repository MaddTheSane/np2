/**
 * @file	cmbase.h
 * @brief	commng ���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "commng.h"

/**
 * @brief commng ���N���X
 */
class CComBase : public _commng
{
protected:
	CComBase(UINT cConnect);
	virtual ~CComBase();

	/**
	 * Read
	 * @param[out] pcData
	 * @return result
	 */
	virtual UINT Read(UINT8* pData) = 0;

	/**
	 * Write
	 * @param[in] cData
	 * @return result
	 */
	virtual UINT Write(UINT8 cData) = 0;

	/**
	 * �X�e�[�^�X�𓾂�
	 * @return �X�e�[�^�X
	 */
	virtual UINT8 GetStat() = 0;

	/**
	 * ���b�Z�[�W
	 */
	virtual INTPTR Message(UINT msg, INTPTR param) = 0;

private:
	static UINT cRead(COMMNG cm, UINT8* pData);
	static UINT cWrite(COMMNG cm, UINT8 cData);
	static UINT8 cGetStat(COMMNG cm);
	static INTPTR cMessage(COMMNG cm, UINT msg, INTPTR param);
	static void cRelease(COMMNG cm);
};
