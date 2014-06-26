/**
 * @file	gimicusb.cpp
 * @brief	G.I.M.I.C USB アクセス クラス
 */

#include "compiler.h"
#include "gimicusb.h"
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/USBSpec.h>

/**
 * コンストラクタ
 */
CGimicUSB::CGimicUSB()
	: m_device(NULL)
	, m_interface(NULL)
	, m_nChipType(CHIP_UNKNOWN)
{
	memset(m_sReg, 0, sizeof(m_sReg));
}

/**
 * USB オープン
 * @retval true 成功
 * @retval false 失敗
 */
bool CGimicUSB::Open()
{
	// 探すデバイス
	const SInt32 usbVendor = 0x16c0;
	const SInt32 usbProduct = 0x05e5;

	// Set up matching dictionary for class IOUSBDevice and its subclasses
	CFMutableDictionaryRef matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
	if (matchingDict == NULL)
	{
		printf("Couldn't create a USB matching dictionary\n");
		return false;
	}

	// Add the vendor and product IDs to the matching dictionary.
	// This is the second key in the table of device-matching keys of the
	// USB Common Class Specification
	CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorName), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbVendor));
	CFDictionarySetValue(matchingDict, CFSTR(kUSBProductName), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbProduct));

	// インタフェイスを得る
	io_iterator_t iterator = 0;
	IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, &iterator);
	io_service_t usbDevice = IOIteratorNext(iterator);
	if (usbDevice == 0)
	{
		printf("Device not found\n");
		return false;
	}
	IOObjectRelease(iterator);

	// Create an intermediate plug-in
	IOCFPlugInInterface** plugInInterface = NULL;
	SInt32 score = 0;
	IOReturn kr = IOCreatePlugInInterfaceForService(usbDevice, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
	kr = IOObjectRelease(usbDevice);
	if ((kr != kIOReturnSuccess) || (plugInInterface == NULL))
	{
		printf("Unable to create a plug-in (%08x)\n", kr);
		return false;
	}

	// Now create the device interface
	IOUSBDeviceInterface** dev = NULL;
	HRESULT result = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID*)&dev);
	(*plugInInterface)->Release(plugInInterface);
	if ((result != S_OK) || (dev == NULL))
	{
		printf("Couldn't create a device interface (%08x)\n", (int)result);
		return false;
	}

	// Open the device to change its state
	kr = (*dev)->USBDeviceOpen(dev);
	if (kr != kIOReturnSuccess)
	{
		printf("Unable to open device: %08x\n", kr);
		(*dev)->Release(dev);
		return false;
	}

	// Configure device
	kr = ConfigureDevice(dev);
	if (kr != kIOReturnSuccess)
	{
		printf("Unable to configure device: %08x\n", kr);
		(*dev)->USBDeviceClose(dev);
		(*dev)->Release(dev);
		return false;
	}

	// Placing the constant kIOUSBFindInterfaceDontCare into the following
	// fields of the IOUSBFindInterfaceRequest structure will allow you
	// to find all the interfaces
	IOUSBFindInterfaceRequest request;
	request.bInterfaceClass = kIOUSBFindInterfaceDontCare;
	request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
	request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
	request.bAlternateSetting = kIOUSBFindInterfaceDontCare;

	// Get an iterator for the interfaces on the device
	io_iterator_t iterator2;
	kr = (*dev)->CreateInterfaceIterator(dev, &request, &iterator2);
	while (1 /*EVER*/)
	{
		io_service_t usbInterface = IOIteratorNext(iterator2);
		if (usbInterface == 0)
		{
			break;
		}

		// Create an intermediate plug-in
		IOCFPlugInInterface** plugInInterface = NULL;
		SInt32 score;
		kr = IOCreatePlugInInterfaceForService(usbInterface, kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
		kr = IOObjectRelease(usbInterface);
		if ((kr != kIOReturnSuccess) || (plugInInterface == NULL))
		{
			printf("Unable to create a plug-in (%08x)\n", kr);
			continue;
		}

		// Now create the device interface for the interface
		IOUSBInterfaceInterface** interface = NULL;
		HRESULT result = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID), (LPVOID*)&interface);
		(*plugInInterface)->Release(plugInInterface);
		if ((result != S_OK) || (interface == NULL))
		{
			printf("Couldn't create a device interface for the interface(%08x)\n", (int) result);
			continue;
		}

		// Get interface class and subclass
		UInt8 interfaceClass;
		kr = (*interface)->GetInterfaceClass(interface, &interfaceClass);

		UInt8 interfaceSubClass;
		kr = (*interface)->GetInterfaceSubClass(interface, &interfaceSubClass);

		printf("Interface class: %d, subclass: %d\n", interfaceClass, interfaceSubClass);

		// Now open the interface. This will cause the pipes associated with
		// the endpoints in the interface descriptor to be instantiated
		kr = (*interface)->USBInterfaceOpen(interface);
		if (kr != kIOReturnSuccess)
		{
			printf("Unable to open interface (%08x)\n", kr);
			(*interface)->Release(interface);
			continue;
		}

		// Get the number of endpoints associated with this interface
		UInt8 interfaceNumEndpoints;
		kr = (*interface)->GetNumEndpoints(interface, &interfaceNumEndpoints);
		if (kr != kIOReturnSuccess)
		{
			printf("Unable to get number of endpoints (%08x)\n", kr);
			(*interface)->USBInterfaceClose(interface);
			(*interface)->Release(interface);
			continue;
		}
 
		printf("Interface has %d endpoints\n", interfaceNumEndpoints);
		// Access each pipe in turn, starting with the pipe at index 1
		// The pipe at index 0 is the default control pipe and should be
		// accessed using (*usbDevice)->DeviceRequest() instead
		for (int pipeRef = 1; pipeRef <= interfaceNumEndpoints; pipeRef++)
		{
			printf(" PipeRef %d: ", pipeRef);

			UInt8 direction;
			UInt8 number;
			UInt8 transferType;
			UInt16 maxPacketSize;
			UInt8 interval;
			kr = (*interface)->GetPipeProperties(interface, pipeRef, &direction, &number, &transferType, &maxPacketSize, &interval);
			if (kr != kIOReturnSuccess)
			{
				printf("Unable to get properties of pipe(%08x)\n", kr);
				continue;
			}

			const char* message;
			switch (direction)
			{
				case kUSBOut:
					message = "out";
					break;

				case kUSBIn:
					message = "in";
					break;

				case kUSBNone:
					message = "none";
					break;

				case kUSBAnyDirn:
					message = "any";
					break;

				default:
					message = "???";
					break;
			}
			printf("direction %s, ", message);

			switch (transferType)
			{
				case kUSBControl:
					message = "control";
					break;

				case kUSBIsoc:
					message = "isoc";
					break;

				case kUSBBulk:
					message = "bulk";
					break;

				case kUSBInterrupt:
					message = "interrupt";
					break;

				case kUSBAnyType:
					message = "any";
					break;

				default:
					message = "???";
					break;
			}
			printf("transfer type %s, maxPacketSize %d\n", message, maxPacketSize);
		}

		// Query G.I.M.I.C module info.
		m_device = dev;
		m_interface = interface;

		Devinfo info;
		::memset(&info, 0, sizeof(info));
		GetModuleInfo(&info);

		printf("Found G.I.M.I.C!\n");
		printf("Devname: %s\n", info.Devname);
		printf("    Rev: %d\n", info.Rev);
		printf(" Serial: %s\n", info.Serial);

		if (!::memcmp(info.Devname, "GMC-OPN3L", 9))
		{
			m_nChipType = CHIP_OPN3L;
		}
		else if (!::memcmp(info.Devname, "GMC-OPM", 7))
		{
			m_nChipType = CHIP_OPM;
		}
		else if (!::memcmp(info.Devname, "GMC-OPNA", 8))
		{
			m_nChipType = CHIP_OPNA;
		}
		else if (!::memcmp(info.Devname, "GMC-OPL3", 8))
		{
			m_nChipType = CHIP_OPL3;
		}
		return true;
	}

	(*dev)->USBDeviceClose(dev);
	(*dev)->Release(dev);

	return false;
}

/**
 * こんてぃぐあ
 * @param[in] dev Device interface
 * @return IOReturn
 */
IOReturn CGimicUSB::ConfigureDevice(IOUSBDeviceInterface** dev)
{
	// Get the number of configurations. The sample code always chooses
	// the first configuration (at index 0) but your code may need a
	// different one
	UInt8 numConfig = 0;
	IOReturn kr = (*dev)->GetNumberOfConfigurations(dev, &numConfig);
	if (!numConfig)
	{
		return -1;
	}

	//Get the configuration descriptor for index 0
	IOUSBConfigurationDescriptorPtr configDesc;
	kr = (*dev)->GetConfigurationDescriptorPtr(dev, 0, &configDesc);
	if (kr != kIOReturnSuccess)
	{
		printf("Couldn't get configuration descriptor for index %d (err = %08x)\n", 0, kr);
		return kr;
	}

	// Set the device's configuration. The configuration value is found in
	// the bConfigurationValue field of the configuration descriptor
	kr = (*dev)->SetConfiguration(dev, configDesc->bConfigurationValue);
	if (kr != kIOReturnSuccess)
	{
		printf("Couldn't set configuration to value %d (err = %08x)\n", 0, kr);
		return kr;
	}
	return kIOReturnSuccess;
}

/**
 * USB クローズ
 */
void CGimicUSB::Close()
{
	if (m_interface)
	{
		(*m_interface)->USBInterfaceClose(m_interface);
		(*m_interface)->Release(m_interface);
		m_interface = NULL;
	}

	if (m_device)
	{
		(*m_device)->USBDeviceClose(m_device);
		(*m_device)->Release(m_device);
		m_device = NULL;
	}

	m_nChipType = CHIP_UNKNOWN;
	memset(m_sReg, 0, sizeof(m_sReg));
}

/**
 * データ送信
 * @param[in] lpBuffer バッファ
 * @param[in] cbBuffer バッファ長
 * @return C86CTL_ERR
 */
int CGimicUSB::Send(const void* lpBuffer, size_t cbBuffer)
{
	UInt8 sBuffer[64];

	if (m_interface == NULL)
	{
		return C86CTL_ERR_NODEVICE;
	}
	if ((lpBuffer == NULL) || (cbBuffer <= 0) || (cbBuffer > sizeof(sBuffer)))
	{
		return C86CTL_ERR_INVALID_PARAM;
	}

	::memcpy(sBuffer, lpBuffer, cbBuffer);
	const size_t nRemain = sizeof(sBuffer) - cbBuffer;
	if (nRemain)
	{
		::memset(sBuffer + cbBuffer, 0xff, nRemain);
	}

	IOReturn kr = (*m_interface)->WritePipe(m_interface, 1, sBuffer, sizeof(sBuffer));
	if (kr != kIOReturnSuccess)
	{
		::printf("Unable to perform bulk write (%08x)\n", kr);
		Close();
		return C86CTL_ERR_UNKNOWN;
	}
	return C86CTL_ERR_NONE;
}

/**
 * データ受信
 * @param[out] lpBuffer バッファ
 * @param[in] cbBuffer バッファ長
 * @return C86CTL_ERR
 */
int CGimicUSB::Recv(void* lpBuffer, size_t cbBuffer)
{
	if (m_interface == NULL)
	{
		return C86CTL_ERR_NODEVICE;
	}

	UInt8 sBuffer[64];
	UInt32 numBytesRead = sizeof(sBuffer);
	IOReturn kr = (*m_interface)->ReadPipe(m_interface, 2, sBuffer, &numBytesRead);
	if (kr != kIOReturnSuccess)
	{
		::printf("Unable to perform bulk read (%08x)\n", kr);
		Close();
		return C86CTL_ERR_UNKNOWN;
	}

	if (lpBuffer != NULL)
	{
		if (cbBuffer > numBytesRead)
		{
			return C86CTL_ERR_INVALID_PARAM;
		}
		::memcpy(lpBuffer, sBuffer, cbBuffer);
	}
	return C86CTL_ERR_NONE;
}

// ---- G.I.M.I.C インタフェイス

/**
 * SSG ヴォリューム設定
 * @param[in] cVolume ヴォリューム
 * @return C86CTL_ERR
 */
int CGimicUSB::SetSSGVolume(UINT8 cVolume)
{
	if (m_nChipType != CHIP_OPNA)
	{
		return C86CTL_ERR_UNSUPPORTED;
	}

	UINT8 sData[3];
	sData[0] = 0xfd;
	sData[1] = 0x84;
	sData[2] = cVolume;
	return Send(sData, sizeof(sData));
}

/**
 * SSG ヴォリューム取得
 * @param[out] pcVolume ヴォリューム
 * @return C86CTL_ERR
 */
int CGimicUSB::GetSSGVolume(UINT8* pcVolume)
{
	if (m_nChipType != CHIP_OPNA)
	{
		return C86CTL_ERR_UNSUPPORTED;
	}

	static const UINT8 sData[2] = {0xfd, 0x86};
	int ret = Send(sData, sizeof(sData));
	if (ret == C86CTL_ERR_NONE)
	{
		ret = Recv(pcVolume, sizeof(*pcVolume));
	}
	return ret;
}

/**
 * クロック設定
 * @param[in] nClock クロック
 * @return C86CTL_ERR
 */
int CGimicUSB::SetPLLClock(UINT nClock)
{
	if ((m_nChipType != CHIP_OPNA) && (m_nChipType != CHIP_OPM) && (m_nChipType != CHIP_OPL3))
	{
		return C86CTL_ERR_UNSUPPORTED;
	}

	UINT8 sData[6];
	sData[0] = 0xfd;
	sData[1] = 0x83;
	sData[2] = static_cast<UINT8>((nClock >> 0) & 0xff);
	sData[3] = static_cast<UINT8>((nClock >> 8) & 0xff);
	sData[4] = static_cast<UINT8>((nClock >> 16) & 0xff);
	sData[5] = static_cast<UINT8>((nClock >> 24) & 0xff);
	return Send(sData, sizeof(sData));
}

/**
 * クロック取得
 * @param[out] pnClock クロック
 * @return C86CTL_ERR
 */
int CGimicUSB::GetPLLClock(UINT* pnClock)
{
	if ((m_nChipType != CHIP_OPNA) && (m_nChipType != CHIP_OPM) && (m_nChipType != CHIP_OPL3))
	{
		return C86CTL_ERR_UNSUPPORTED;
	}

	static const UINT8 sData[2] = {0xfd, 0x85};
	int ret = Send(sData, sizeof(sData));
	if (ret != C86CTL_ERR_NONE)
	{
		return ret;
	}

	UINT8 sRecv[4];
	ret = Recv(sRecv, sizeof(sRecv));
	if ((ret == C86CTL_ERR_NONE) && (pnClock))
	{
		*pnClock = (sRecv[0] << 0) | (sRecv[1] << 8) | (sRecv[2] << 16) | (sRecv[3] << 24);
	}
	return ret;
}

/**
 * ファームウェア バージョン取得
 * @param[out] pnMajor メジャー
 * @param[out] pnMinor マイナー
 * @param[out] pnRev リビジョン
 * @param[out] pnBuild ビルド
 * @return C86CTL_ERR
 */
int CGimicUSB::GetFWVer(UINT* pnMajor, UINT* pnMinor, UINT* pnRev, UINT* pnBuild)
{
	static const UINT8 sData[2] = {0xfd, 0x92};
	int ret = Send(sData, sizeof(sData));
	if (ret != C86CTL_ERR_NONE)
	{
		return ret;
	}

	UINT8 sRecv[16];
	ret = Recv(sRecv, sizeof(sRecv));
	if (ret == C86CTL_ERR_NONE)
	{
		if (pnMajor != NULL)
		{
			*pnMajor = (sRecv[0] << 0) | (sRecv[1] << 8) | (sRecv[2] << 16) | (sRecv[3] << 24);
		}
		if (pnMinor != NULL)
		{
			*pnMinor = (sRecv[4] << 0) | (sRecv[5] << 8) | (sRecv[6] << 16) | (sRecv[7] << 24);
		}
		if (pnRev != NULL)
		{
			*pnRev = (sRecv[8] << 0) | (sRecv[9] << 8) | (sRecv[10] << 16) | (sRecv[11] << 24);
		}
		if (pnBuild != NULL)
		{
			*pnBuild = (sRecv[12] << 0) | (sRecv[13] << 8) | (sRecv[14] << 16) | (sRecv[15] << 24);
		}
	}
	return ret;
}

/**
 * マザーボード情報取得
 * @param[out] pInfo 情報構造体のポインタ
 * @return C86CTL_ERR
 */
int CGimicUSB::GetMBInfo(Devinfo* pInfo)
{
	return GetInfo(0xff, pInfo);
}

/**
 * モジュール情報取得
 * @param[out] pInfo 情報構造体のポインタ
 * @return C86CTL_ERR
 */
int CGimicUSB::GetModuleInfo(Devinfo* pInfo)
{
	return GetInfo(0, pInfo);
}

/**
 * 情報取得
 * @param[in] cParam パラメタ
 * @param[out] pInfo 情報構造体のポインタ
 * @return C86CTL_ERR
 */
int CGimicUSB::GetInfo(UINT8 cParam, Devinfo* pInfo)
{
	UINT8 sData[3];
	sData[0] = 0xfd;
	sData[1] = 0x91;
	sData[2] = cParam;
	int ret = Send(sData, sizeof(sData));
	if (ret != C86CTL_ERR_NONE)
	{
		return ret;
	}

	ret = Recv(pInfo, sizeof(*pInfo));
	if ((ret == C86CTL_ERR_NONE) && (pInfo != NULL))
	{
		TailZeroFill(pInfo->Devname, sizeof(pInfo->Devname));
		TailZeroFill(pInfo->Serial, sizeof(pInfo->Serial));
	}
	return ret;
}

/**
 * おしりを ZeroFill
 * @param[in] lpBuffer バッファ
 * @param[in] cbBuffer バッファ長
 */
void CGimicUSB::TailZeroFill(char* lpBuffer, size_t cbBuffer)
{
	while (cbBuffer > 0)
	{
		cbBuffer--;
		if (lpBuffer[cbBuffer] == -1)
		{
			lpBuffer[cbBuffer] = '\0';
		}
		else if (lpBuffer[cbBuffer] != '\0')
		{
			break;
		}
	}
}

/**
 * リセット
 * @return C86CTL_ERR
 */
int CGimicUSB::Reset()
{
	static const UINT8 sData[2] = {0xfd, 0x82};
	return Send(sData, sizeof(sData));
}

/**
 * Output
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CGimicUSB::Out(UINT nAddr, UINT8 cData)
{
	if (nAddr < sizeof(m_sReg))
	{
		m_sReg[nAddr] = cData;
	}

	switch (m_nChipType)
	{
		case CHIP_OPNA:
		case CHIP_OPN3L:
			if ((nAddr >= 0x100) && (nAddr <= 0x110))
			{
				nAddr -= 0x40;
			}
			break;

		case CHIP_OPM:
			if ((nAddr >= 0xfc) && (nAddr <= 0xff))
			{
				nAddr -= 0xe0;
			}
			break;

		default:
			break;
	}

	if (nAddr < 0xfc)
	{
		UINT8 sData[2];
		sData[0] = static_cast<UINT8>(nAddr & 0xff);
		sData[1] = cData;
		Send(sData, sizeof(sData));
	}
	else if ((nAddr >= 0x100) && (nAddr <= 0x1fb))
	{
		UINT8 sData[3];
		sData[0] = 0xfe;
		sData[1] = static_cast<UINT8>(nAddr & 0xff);
		sData[2] = cData;
		Send(sData, sizeof(sData));
	}
}

/**
 * Input
 * @param[in] nAddr アドレス
 * @return データ
 */
UINT8 CGimicUSB::In(UINT nAddr)
{
	if (nAddr < sizeof(m_sReg))
	{
		return m_sReg[nAddr];
	}
	return 0xff;
}
