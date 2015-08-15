/**
 * @file	externalopna.cpp
 * @brief	�O�� OPNA ���t�N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "externalopna.h"
#include "c86ctl\c86ctlif.h"
#include "rebirth\rebirth.h"
#include "romeo\juliet.h"

CExternalOpna CExternalOpna::sm_instance;

/**
 * �R���X�g���N�^
 */
CExternalOpna::CExternalOpna()
	: m_module(NULL)
	, m_cPsgMix(0x3f)
{
	memset(m_cAlgorithm, 0, sizeof(m_cAlgorithm));
	memset(m_cTtl, 0x7f, sizeof(m_cTtl));
}

/**
 * ������
 */
void CExternalOpna::Initialize()
{
	// ROMEO
	IExtendModule* pModule = new CJuliet;
	if (pModule->Initialize())
	{
		m_module = pModule;
		return;
	}
	delete pModule;

	// G.I.M.I.C / C86BOX
	pModule = new C86CtlIf;
	if (pModule->Initialize())
	{
		m_module = pModule;
		return;
	}
	delete pModule;	

	// RE:birth
	pModule = new CRebirth;
	if (pModule->Initialize())
	{
		m_module = pModule;
		return;
	}
	delete pModule;

}

/**
 * ���
 */
void CExternalOpna::Deinitialize()
{
	IExtendModule* pModule = m_module;
	m_module = NULL;
	if (pModule)
	{
		pModule->Deinitialize();
		delete pModule;
	}
}

/**
 * �r�W�[?
 * @retval true �r�W�[
 * @retval false ���f�B
 */
bool CExternalOpna::IsBusy() const
{
	if (m_module)
	{
		return m_module->IsBusy();
	}
	return false;

}

/**
 * �������Z�b�g
 */
void CExternalOpna::Reset() const
{
	if (m_module)
	{
		m_module->Reset();
	}
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalOpna::WriteRegister(UINT nAddr, UINT8 cData)
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

/**
 * �~���[�g
 * @param[in] bMute �~���[�g
 */
void CExternalOpna::Mute(bool bMute) const
{
	WriteRegisterInner(0x07, (bMute) ? 0x3f : m_cPsgMix);

	const int nVolume = (bMute) ? -127 : 0;
	for (UINT ch = 0; ch < 3; ch++)
	{
		SetVolume(ch + 0, nVolume);
		SetVolume(ch + 4, nVolume);
	}
}

/**
 * ���W�X�^��������(����)
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalOpna::WriteRegisterInner(UINT nAddr, UINT8 cData) const
{
	if (m_module)
	{
		m_module->WriteRegister(nAddr, cData);
	}
}

/**
 * ���H�����[���ݒ�
 * @param[in] nChannel �`�����l��
 * @param[in] nVolume ���H�����[���l
 */
void CExternalOpna::SetVolume(UINT nChannel, int nVolume) const
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

/**
 * ���W�X�^�����X�g�A����
 * @param[in] data �f�[�^
 * @param[in] bOpna OPNA ���W�X�^�����X�g�A����
 */
void CExternalOpna::Restore(const UINT8* data, bool bOpna)
{
	for (UINT i = 0x30; i < 0xa0; i++)
	{
		WriteRegister(i, data[i]);
	}
	for (UINT ch = 0; ch < 3; ch++)
	{
		WriteRegister(ch + 0xa4, data[ch + 0x0a4]);
		WriteRegister(ch + 0xa0, data[ch + 0x0a0]);
		WriteRegister(ch + 0xb4, data[ch + 0x0b4]);
		WriteRegister(ch + 0xb0, data[ch + 0x0b0]);
	}

	if (bOpna)
	{
		for (UINT i = 0x130; i < 0x1a0; i++)
		{
			WriteRegister(i, data[i]);
		}
		for (UINT ch = 0; ch < 3; ch++)
		{
			WriteRegister(ch + 0x1a4, data[ch + 0x1a4]);
			WriteRegister(ch + 0x1a0, data[ch + 0x1a0]);
			WriteRegister(ch + 0x1b4, data[ch + 0x1b4]);
			WriteRegister(ch + 0x1b0, data[ch + 0x1b0]);
		}
		WriteRegister(0x11, data[0x11]);
		WriteRegister(0x18, data[0x18]);
		WriteRegister(0x19, data[0x19]);
		WriteRegister(0x1a, data[0x1a]);
		WriteRegister(0x1b, data[0x1b]);
		WriteRegister(0x1c, data[0x1c]);
		WriteRegister(0x1d, data[0x1d]);
	}

	for (UINT i = 0; i < 0x0e; i++)
	{
		WriteRegister(i, data[i]);
	}
}
