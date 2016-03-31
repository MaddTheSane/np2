/**
 * @file	usbdev.h
 * @brief	USB �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <IOKit/usb/IOUSBLib.h>

/**
 * @brief USB �A�N�Z�X �N���X
 */
class CUsbDev
{
public:
	CUsbDev();
	~CUsbDev();
	bool Open(unsigned int vid, unsigned int pid, unsigned int nIndex = 0);
	void Close();
	int CtrlXfer(int nType, int nRequest, int nValue = 0, int nIndex = 0, void* lpBuffer = NULL, int cbBuffer = 0);
	int WriteBulk(const void* lpBuffer, int cbBuffer);
	int ReadBulk(void* lpBuffer, int cbBuffer);
	bool IsOpened() const;

private:
	IOUSBDeviceInterface** m_device;			/*!< �f�o�C�X */
	IOUSBInterfaceInterface** m_interface;		/*!< �C���^�t�F�C�X */
	static IOReturn ConfigureDevice(IOUSBDeviceInterface** dev);
};

/**
 * �I�[�v����?
 * @retval true �I�[�v����
 * @retval false ���I�[�v��
 */
inline bool CUsbDev::IsOpened() const
{
	return (m_interface != NULL);
}
