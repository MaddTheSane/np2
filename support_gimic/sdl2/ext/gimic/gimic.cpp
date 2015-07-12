/**
 * @file	gimic.cpp
 * @brief	G.I.M.I.C �A�N�Z�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "gimic.h"
#include "c86boxusb.h"
#include "gimicusb.h"

/**
 * �R���X�g���N�^
 */
CGimic::CGimic()
	: m_device(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CGimic::~CGimic()
{
}

/**
 * ������
 * @retval true ����
 * @retval false ���s
 */
bool CGimic::Initialize()
{
	Deinitialize();

	CGimicUSB* gimic = new CGimicUSB();
	if (gimic->Initialize() == C86CTL_ERR_NONE)
	{
		m_device = gimic;
		Reset();
		return true;
	}
	delete gimic;

	C86BoxUSB* c86box = new C86BoxUSB();
	if (c86box->Initialize() == C86CTL_ERR_NONE)
	{
		m_device = c86box;
		Reset();
		return true;
	}
	delete c86box;
	return false;
}

/**
 * ���
 */
void CGimic::Deinitialize()
{
	if (m_device)
	{
		m_device->Deinitialize();
		delete m_device;
		m_device = NULL;
	}
}

/**
 * �f�o�C�X�͗L��?
 * @retval true �L��
 * @retval false ����
 */
bool CGimic::IsEnabled()
{
	return (m_device != NULL);
}

/**
 * �r�W�[?
 * @retval true �r�W�[
 * @retval false ���f�B
 */
bool CGimic::IsBusy()
{
	return false;
}

/**
 * �������Z�b�g
 */
void CGimic::Reset()
{
	if (m_device)
	{
		m_device->Reset();

		// G.I.M.I.C �̏�����
		CGimicUSB* gimicusb = dynamic_cast<CGimicUSB*>(m_device);
		if (gimicusb)
		{
			gimicusb->SetPLLClock(7987200);
			gimicusb->SetSSGVolume(31);
		}
	}
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CGimic::WriteRegister(UINT nAddr, UINT8 cData)
{
	// printf("WriteReg %04x %02x\n", nAddr, cData);
	if (m_device)
	{
		m_device->Out(nAddr, cData);
	}
}
