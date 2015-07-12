/**
 * @file	gimic.cpp
 * @brief	G.I.M.I.C アクセス クラスの動作の定義を行います
 */

#include "compiler.h"
#include "gimic.h"
#include "c86boxusb.h"
#include "gimicusb.h"

/**
 * コンストラクタ
 */
CGimic::CGimic()
	: m_device(NULL)
{
}

/**
 * デストラクタ
 */
CGimic::~CGimic()
{
}

/**
 * 初期化
 * @retval true 成功
 * @retval false 失敗
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
 * 解放
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
 * デバイスは有効?
 * @retval true 有効
 * @retval false 無効
 */
bool CGimic::IsEnabled()
{
	return (m_device != NULL);
}

/**
 * ビジー?
 * @retval true ビジー
 * @retval false レディ
 */
bool CGimic::IsBusy()
{
	return false;
}

/**
 * 音源リセット
 */
void CGimic::Reset()
{
	if (m_device)
	{
		m_device->Reset();

		// G.I.M.I.C の初期化
		CGimicUSB* gimicusb = dynamic_cast<CGimicUSB*>(m_device);
		if (gimicusb)
		{
			gimicusb->SetPLLClock(7987200);
			gimicusb->SetSSGVolume(31);
		}
	}
}

/**
 * レジスタ書き込み
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CGimic::WriteRegister(UINT nAddr, UINT8 cData)
{
	// printf("WriteReg %04x %02x\n", nAddr, cData);
	if (m_device)
	{
		m_device->Out(nAddr, cData);
	}
}
