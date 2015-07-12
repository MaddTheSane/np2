/**
 * @file	spfmlight.cpp
 * @brief	Implementation of accessing SPFM Light
 */

#include "compiler.h"
#include "spfmlight.h"

/**
 * �R���X�g���N�^
 */
CSpfmLight::CSpfmLight()
{
}

/**
 * �f�X�g���N�^
 */
CSpfmLight::~CSpfmLight()
{
}

/**
 * ������
 * @retval true ����
 * @retval false ���s
 */
bool CSpfmLight::Initialize()
{
	Deinitialize();

	return m_serial.Open(5, 1500000, TEXT("8N1"));
}

/**
 * ���
 */
void CSpfmLight::Deinitialize()
{
	m_serial.Close();
}

/**
 * �f�o�C�X�͗L��?
 * @retval true �L��
 * @retval false ����
 */
bool CSpfmLight::IsEnabled()
{
	return m_serial.IsOpened();
}

/**
 * �r�W�[?
 * @retval true �r�W�[
 * @retval false ���f�B
 */
bool CSpfmLight::IsBusy()
{
	return false;
}

/**
 * �������Z�b�g
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
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
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
