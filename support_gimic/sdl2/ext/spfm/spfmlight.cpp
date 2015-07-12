/**
 * @file	spfmlight.cpp
 * @brief	Implementation of accessing SPFM Light
 */

#include "compiler.h"
#include "spfmlight.h"

/**
 * コンストラクタ
 */
CSpfmLight::CSpfmLight()
{
}

/**
 * デストラクタ
 */
CSpfmLight::~CSpfmLight()
{
}

/**
 * 初期化
 * @retval true 成功
 * @retval false 失敗
 */
bool CSpfmLight::Initialize()
{
	Deinitialize();

	return m_serial.Open(5, 1500000, TEXT("8N1"));
}

/**
 * 解放
 */
void CSpfmLight::Deinitialize()
{
	m_serial.Close();
}

/**
 * デバイスは有効?
 * @retval true 有効
 * @retval false 無効
 */
bool CSpfmLight::IsEnabled()
{
	return m_serial.IsOpened();
}

/**
 * ビジー?
 * @retval true ビジー
 * @retval false レディ
 */
bool CSpfmLight::IsBusy()
{
	return false;
}

/**
 * 音源リセット
 */
void CSpfmLight::Reset()
{
	const unsigned char reset[1] = {0xfe};
	m_serial.Write(reset, sizeof(reset));

	::Sleep(1);

	char buffer[4];
	m_serial.Read(buffer, 2);
	buffer[2] = '\0';
	printf("ret = %s (%x,%x)\n", buffer, buffer[0], buffer[1]);
}

/**
 * レジスタ書き込み
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CSpfmLight::WriteRegister(UINT nAddr, UINT8 cData)
{
	unsigned char cmd[4];
	cmd[0] = 0x00;
	cmd[1] = static_cast<unsigned char>((nAddr >> 7) & 2);
	cmd[2] = static_cast<unsigned char>(nAddr & 0xff);
	cmd[3] = cData;
	m_serial.Write(cmd, sizeof(cmd));
}
