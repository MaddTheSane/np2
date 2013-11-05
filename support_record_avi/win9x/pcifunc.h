////////////////////////////////////////////////////////
//  pcidebug.dll
//                        Aug 20 1999 kashiwano masahiro
//
////////////////////////////////////////////////////////

/*
	PCIDEBUG.DLL export �֐�
*/

#ifndef _NTDDK_ 
#include "pcidef.h"
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* �o�X�ԍ�,�f�o�C�X�ԍ�,�t�@���N�V�����ԍ� ���� PCI�f�o�C�X�A�h���X�ɕϊ� */
#define pciBusDevFunc(Bus,Dev,Func)		((Bus&0xff)<<8)|((Dev&0x1f)<<3)|(Func&7)
/* PCI�f�o�C�X�A�h���X ���� �o�X�ԍ��擾 */
#define pciGetBus(BXreg)		((BXreg>>8)&0xff)
/* PCI�f�o�C�X�A�h���X ���� �f�o�C�X�ԍ��擾 */
#define pciGetDev(BXreg)		((BXreg>>3)&0x1f)
/* PCI�f�o�C�X�A�h���X ���� �t�@���N�V�����ԍ��擾 */
#define pciGetFunc(BXreg)		(BXreg&7)

typedef enum _tag_DLLSTATUS {
	DLLSTATUS_NOERROR = 0,
	DLLSTATUS_DRIVERNOTLOADED,
	DLLSTATUS_NOTSUPPORTEDPLATFORM,
	DLLSTATUS_OTHERERROR,
} DLLSTATUS;

 
DLLSTATUS WINAPI getdllstatus(void);


UCHAR	 WINAPI _pciBusNumber(void);

ULONG	WINAPI _pciFindPciDevice(ULONG vender_id,ULONG device_id,ULONG index);
 
ULONG	WINAPI _pciFindPciClass(UCHAR baseclass, UCHAR subclass, UCHAR programif, ULONG index);

 
ULONG WINAPI pciConfigRead(ULONG pci_address,ULONG reg_address, char *data , ULONG size);
 
ULONG	WINAPI _pciConfigReadLong(ULONG pci_address,ULONG reg_address);
 
USHORT	WINAPI _pciConfigReadShort(ULONG pci_address,ULONG reg_address);

UCHAR	WINAPI _pciConfigReadChar(ULONG pci_address,ULONG reg_address);
 
void WINAPI _pciConfigWriteLong(ULONG pci_address,ULONG reg_address, ULONG data);
 
void WINAPI _pciConfigWriteShort(ULONG pci_address,ULONG reg_address,USHORT data);
 
void WINAPI _pciConfigWriteChar(ULONG pci_address,ULONG reg_address,UCHAR data);
 
ULONG	WINAPI _IoReadLong(ULONG address);
 
USHORT	WINAPI _IoReadShort(ULONG address);
 
UCHAR	WINAPI _IoReadChar(ULONG address);
 
void WINAPI _IoWriteLong(ULONG address, ULONG data);
 
void WINAPI _IoWriteShort(ULONG address, USHORT data);
 
void WINAPI _IoWriteChar(ULONG address, UCHAR data);

 
ULONG	WINAPI _MemReadBlockLong(ULONG address, UCHAR* data, ULONG count);
 
ULONG	WINAPI _MemReadBlockShort(ULONG address, UCHAR* data, ULONG count);
 
ULONG	WINAPI _MemReadBlockChar(ULONG address, UCHAR* data, ULONG count);

 
ULONG	WINAPI _MemReadLong(ULONG address);
 
USHORT	WINAPI _MemReadShort(ULONG address);
 
UCHAR	WINAPI _MemReadChar(ULONG address);

 
ULONG	WINAPI _MemWriteBlockLong(ULONG address, UCHAR* data, ULONG count);
 
ULONG	WINAPI _MemWriteBlockShort(ULONG address, UCHAR* data, ULONG count);
 
ULONG	WINAPI _MemWriteBlockChar(ULONG address, UCHAR* data, ULONG count);

 
void WINAPI _MemWriteLong(ULONG address, ULONG data);
 
void WINAPI _MemWriteShort(ULONG address, USHORT data);
 
void WINAPI _MemWriteChar(ULONG address, UCHAR data);

// add copy and fill function  ver 1.1
ULONG WINAPI _MemCopyLong(ULONG src_address, ULONG dest_address, ULONG count);

ULONG WINAPI _MemCopyShort(ULONG src_address, ULONG dest_address, ULONG count);

ULONG WINAPI _MemCopyChar(ULONG src_address, ULONG dest_address, ULONG count);

ULONG WINAPI _MemFillLong(ULONG  address, ULONG data, ULONG count);

ULONG WINAPI _MemFillShort(ULONG  address, USHORT data, ULONG count);

ULONG WINAPI _MemFillChar(ULONG  address, UCHAR data, ULONG count);

//

int WINAPI _hookIRQ_NT(INTERFACE_TYPE type,
				ULONG busnumber,
				ULONG level,
				KINTERRUPT_MODE		InterruptMode,
				void(WINAPI *isr)(void) );

 
int WINAPI _hookIRQ(ULONG level, void (WINAPI *isr)(void));
 
int WINAPI _freeIRQ(ULONG levelold);

////////////////////////////////////////////////////////////
//
// DOS�pPCI�f�o�b�N���C�u�����[�ƌ݊������������邽�߂�define
//
////////////////////////////////////////////////////////////

#define _getCpuMode() 0;
#define _preInitHimem() 0;

#define _readHimemByte(a) _MemReadChar(a)
#define _readHimemWord(a) _MemReadShort(a)
#define _readHimemLong(a) _MemReadLong(a)

#define _writeHimemByte(a, d) _MemWriteChar(a, d)
#define _writeHimemWord(a, d) _MemWriteShort(a,d)
#define _writeHimemLong(a, d) _MemWriteLong(a, d)

#define _readHimemBlockByte(a, d, s) _MemReadBlockChar(a, d, s)
#define _readHimemBlockWord(a, d, s) _MemReadBlockShort(a, d, s)
#define _readHimemBlockLong(a, d, s) _MemReadBlockLong(a, d, s)

#define _writeHimemBlockByte(a, d, s) _MemWriteBlockChar(a, d, s)
#define _writeHimemBlockWord(a, d, s) _MemWriteBlockShort(a, d, s)
#define _writeHimemBlockLong(a, d, s) _MemWriteBlockLong(a, d, s)

#define _copyHimemByte(a1, a2, s) _MemCopyChar(a1, a2, s)
#define _copyHimemWord(a1, a2, s) _MemCopyShort(a1, a2, s)
#define _copyHimemLong(a1, a2, s) _MemCopyLong(a1, a2, s)

#define _fillHimemByte(a, d, s) _MemFillChar(a, d, s)
#define _fillHimemWord(a, d, s) _MemFillShort(a, d, s)
#define _fillHimemLong(a, d, s) _MemFillLong(a, d, s)

#define _maskNMI()
#define _unmaskNMI()

#define _pciMaxBusNo() (_pciBusNumber()-1)
#define _pciConfigReadByte(dev, ad) _pciConfigReadChar(dev, ad)
#define _pciConfigReadWord(dev, ad) _pciConfigReadShort(dev, ad)
#define _pciConfigWriteByte(dev, ad, d) _pciConfigWriteChar(dev, ad, d)
#define _pciConfigWriteWord(dev, ad, d) _pciConfigWriteShort(dev, ad, d)

#ifdef  __cplusplus
}
#endif
