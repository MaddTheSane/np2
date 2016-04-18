/**
 * @file	net.c
 * @brief	Virtual LAN Interface
 *
 * @author	$Author: SimK $
 */

#include	"compiler.h"

#if defined(SUPPORT_NET)

#include	"pccore.h"
#include	"net.h"
#include <winioctl.h>
#include <tchar.h>

#pragma warning(disable: 4996)
#pragma comment(lib, "Advapi32.lib")

#define DEVICE_PATH_FMT _T("\\\\.\\Global\\%s.tap")
 
#define TAP_CONTROL_CODE(request,method) \
  CTL_CODE (FILE_DEVICE_UNKNOWN, request, method, FILE_ANY_ACCESS)
 
#define TAP_IOCTL_SET_MEDIA_STATUS \
  TAP_CONTROL_CODE (6, METHOD_BUFFERED)
 
#define NET_BUFLEN (10*1024) // �o�b�t�@1�̒����iXXX: �p�P�b�g�T�C�Y�̍ő�l�ɂ��Ȃ��Ɩ��ʁB�����ƌ����Ήϒ��ő傫��1�̃o�b�t�@�ɓ����ׂ��H�j
#define NET_ARYLEN (128) // �o�b�t�@�̐�

	NP2NET	np2net;
	
static TCHAR *GetNetWorkDeviceGuid(CONST TCHAR *, TCHAR *, DWORD); // TAP�f�o�C�X������GUID���擾����

static TCHAR np2net_tapName[MAX_PATH]; // TAP�f�o�C�X��

static HANDLE	np2net_hTap = NULL; // TAP�f�o�C�X�̓ǂݏ����n���h��
static HANDLE	np2net_hThreadR = NULL; // Read�p�X���b�h
static HANDLE	np2net_hThreadW = NULL; // Write�p�X���b�h
static int		np2net_hThreadexit = 0; // �X���b�h�I���t���O

static REG8		np2net_membuf[NET_ARYLEN][NET_BUFLEN]; // ���M�p�o�b�t�@
static int		np2net_membuflen[NET_ARYLEN]; // ���M�p�o�b�t�@�ɂ���f�[�^�̒���
static int		np2net_membuf_readpos = 0; // �o�b�t�@�ǂݎ��ʒu
static int		np2net_membuf_writepos = 0; // �o�b�t�@�������݈ʒu

static HANDLE		np2net_write_hEvent;
static OVERLAPPED	np2net_write_ovl;

// �p�P�b�g�f�[�^�� TAP �֏����o��
static int doWriteTap(HANDLE hTap, const UCHAR *pSendBuf, DWORD len)
{
	#define ETHERDATALEN_MIN 46
	DWORD dwWriteLen;

	if (!WriteFile(hTap, pSendBuf, len, &dwWriteLen, &np2net_write_ovl)) {
		DWORD err = GetLastError();
		if (err == ERROR_IO_PENDING) {
			WaitForSingleObject(np2net_write_hEvent, INFINITE); // �����҂�
			GetOverlappedResult(hTap, &np2net_write_ovl, &dwWriteLen, FALSE);
		} else {
			TRACEOUT(("LGY-98: WriteFile err=0x%08X\n", err));
			return -1;
		}
	}
	//TRACEOUT(("LGY-98: send %u bytes\n", dwWriteLen));
	return 0;
}

// �p�P�b�g�f�[�^���o�b�t�@�ɑ���i���ۂ̑��M��np2net_ThreadFuncW���ōs����j
static int sendDataToBuffer(UCHAR *pSendBuf, DWORD len){
	if(len > NET_BUFLEN){
		TRACEOUT(("LGY-98: too large packet!! %d bytes", len));
		return 1;
	}
	if(np2net_membuf_readpos==(np2net_membuf_writepos+1)%NET_ARYLEN){
		TRACEOUT(("LGY-98: buffer full"));
		while(np2net_membuf_readpos==(np2net_membuf_writepos+1)%NET_ARYLEN){
			Sleep(0); // �o�b�t�@�������ς��Ȃ̂ő҂�
			//return 1; // �o�b�t�@�������ς��Ȃ̂Ŏ̂Ă�
		}
	}
	memcpy(np2net_membuf[np2net_membuf_writepos], pSendBuf, len);
	np2net_membuflen[np2net_membuf_writepos] = len;
	np2net_membuf_writepos = (np2net_membuf_writepos+1)%NET_ARYLEN;
	return 0;
}

// �p�P�b�g���M���ɌĂ΂��i�f�t�H���g�����j
static void np2net_default_send_packet(const UINT8 *buf, int size)
{
	sendDataToBuffer((UCHAR*)buf, size);
}
// �p�P�b�g��M���ɌĂ΂��i�f�t�H���g�����j
static void np2net_default_recieve_packet(const UINT8 *buf, int size)
{
	// �������Ȃ�
}

//  �񓯊��ŒʐM���Ă݂�iWrite�j
static DWORD WINAPI np2net_ThreadFuncW(LPVOID vdParam) {
	while (!np2net_hThreadexit) {
		if(np2net.recieve_packet != np2net_default_recieve_packet){
			if(np2net_membuf_readpos!=np2net_membuf_writepos){
				doWriteTap(np2net_hTap, (UCHAR*)(np2net_membuf[np2net_membuf_readpos]), np2net_membuflen[np2net_membuf_readpos]);
				np2net_membuf_readpos = (np2net_membuf_readpos+1)%NET_ARYLEN;
			}else{
				Sleep(0);
			}
		}else{
			Sleep(1000);
		}
	}
	return 0;
}
//  �񓯊��ŒʐM���Ă݂�iRead�j
static DWORD WINAPI np2net_ThreadFuncR(LPVOID vdParam) {
	HANDLE hEvent = NULL;
	DWORD dwLen;
	OVERLAPPED ovl;
	int nodatacount = 0;
	int sleepcount = 0;
	CHAR np2net_Buf[NET_BUFLEN];

	// OVERLAPPED�񓯊��ǂݎ�菀��
	memset(&ovl, 0, sizeof(OVERLAPPED));
	ovl.hEvent = hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	ovl.Offset = 0;
	ovl.OffsetHigh = 0;
 
	while (!np2net_hThreadexit) {
		if (!ReadFile(np2net_hTap, np2net_Buf, sizeof(np2net_Buf), &dwLen, &ovl)) {
			DWORD err = GetLastError();
			if (err == ERROR_IO_PENDING) {
				// �ǂݎ��҂�
				WaitForSingleObject(hEvent, INFINITE); // ��M�����҂�
				GetOverlappedResult(np2net_hTap, &ovl, &dwLen, FALSE);
				if(dwLen>0){
					//TRACEOUT(("LGY-98: recieve %u bytes\n", dwLen));
					np2net.recieve_packet((UINT8*)np2net_Buf, dwLen); // ��M�ł����̂Œʒm����
				}
			} else {
				// �ǂݎ��G���[
				printf("TAP-Win32: ReadFile err=0x%08X\n", err);
				//CloseHandle(hTap);
				//return -1;
				Sleep(0);
			}
		} else {
			// �ǂݎ�萬��
			if(dwLen>0){
				//TRACEOUT(("LGY-98: recieve %u bytes\n", dwLen));
				np2net.recieve_packet((UINT8*)np2net_Buf, dwLen); // ��M�ł����̂Œʒm����
			}else{
				Sleep(0);
			}
		}
	}
	CloseHandle(hEvent);
	hEvent = NULL;
	return 0;
}

//  TAP�f�o�C�X�����
static void np2net_closeTAP(){
    if (np2net_hTap != NULL) {
		if(np2net_hThreadR){
			np2net_hThreadexit = 1;
			WaitForSingleObject(np2net_hThreadR,  INFINITE);
			WaitForSingleObject(np2net_hThreadW, INFINITE);
			np2net_membuf_readpos = np2net_membuf_writepos;
			np2net_hThreadexit = 0;
			np2net_hThreadR = NULL;
		}
		CloseHandle(np2net_hTap);
		TRACEOUT(("LGY-98: TAP is closed"));
		np2net_hTap = NULL;
    }
}
//  TAP�f�o�C�X���J��
static int np2net_openTAP(const TCHAR* tapname){
	DWORD dwID;
	DWORD dwLen;
	ULONG status = TRUE;
	TCHAR Buf[2048];
	TCHAR szDevicePath[256];
	TCHAR szTAPname[MAX_PATH] = _T("TAP1");

	if(*tapname){
		_tcscpy(szTAPname, tapname);
	}

	np2net_closeTAP();

	// �w�肳�ꂽ�\�������� TAP �� GUID �𓾂�
	if (!GetNetWorkDeviceGuid(szTAPname, Buf, 2048)) {
		TRACEOUT(("LGY-98: [%s] GUID is not found\n", szTAPname));
		return 1;
	}
	TRACEOUT(("LGY-98: [%s] GUID = %s\n", szTAPname, Buf));
	_stprintf(szDevicePath, DEVICE_PATH_FMT, Buf);
 
	// TAP �f�o�C�X���J��
	np2net_hTap = CreateFile (szDevicePath, GENERIC_READ | GENERIC_WRITE,
		0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_OVERLAPPED, 0);
 
	if (np2net_hTap == INVALID_HANDLE_VALUE) {
		TRACEOUT(("LGY-98: Failed to open [%s]", szDevicePath));
		return 2;
	}

	TRACEOUT(("LGY-98: TAP is opened"));
	
	// TAP �f�o�C�X���A�N�e�B�u��
	status = TRUE;
	if (!DeviceIoControl(np2net_hTap,TAP_IOCTL_SET_MEDIA_STATUS,
				&status, sizeof(status), &status, sizeof(status),
				&dwLen, NULL)) {
		TRACEOUT(("LGY-98: TAP_IOCTL_SET_MEDIA_STATUS err"));
		np2net_closeTAP();
		return 3;
	}
 
	np2net_hThreadR = CreateThread(NULL , 0 , np2net_ThreadFuncR  , NULL , 0 , &dwID);
	np2net_hThreadW = CreateThread(NULL , 0 , np2net_ThreadFuncW , NULL , 0 , &dwID);
	
	return 0;
}

// NP2�N�����̏���
void np2net_init(void)
{
	memset(&np2net_write_ovl, 0, sizeof(OVERLAPPED));
	np2net_write_ovl.hEvent = np2net_write_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	np2net_write_ovl.Offset = 0;
	np2net_write_ovl.OffsetHigh = 0;

	memset(np2net_tapName, 0, sizeof(np2net_tapName));
	np2net.send_packet = np2net_default_send_packet;
	np2net.recieve_packet = np2net_default_recieve_packet;
}
// ���Z�b�g���ɌĂ΂��H
void np2net_reset(const NP2CFG *pConfig){
	_tcscpy(np2net_tapName, pConfig->np2nettap);
	if(pConfig->uselgy98){ // XXX: �g���Ă��Ȃ��Ȃ�TAP�f�o�C�X�̓I�[�v�����Ȃ�
		np2net_openTAP(np2net_tapName);
	}
}
// ���Z�b�g���ɌĂ΂��H�inp2net_reset����Eiocore_attach�`���g����j
void np2net_bind(void){
}
// NP2�I�����̏���
void np2net_shutdown(void)
{
	np2net_hThreadexit = 1;
	np2net_closeTAP();
}

// �Q�l����: http://dsas.blog.klab.org/archives/51012690.html

// �l�b�g���[�N�f�o�C�X�\��������f�o�C�X GUID �����������
static TCHAR *GetNetWorkDeviceGuid(CONST TCHAR *pDisplayName, TCHAR *pszBuf, DWORD cbBuf)
{
  CONST TCHAR *SUBKEY = _T("SYSTEM\\CurrentControlSet\\Control\\Network");
 
#define BUFSZ 256
  // HKLM\SYSTEM\\CurrentControlSet\\Control\\Network\{id1]\{id2}\Connection\Name ��
  // �l�b�g���[�N�f�o�C�X���i���j�[�N�j�̊i�[���ꂽ�G���g���ł���A
  // {id2} �����̃f�o�C�X�� GUID �ł���
 
  HKEY hKey1, hKey2, hKey3;
  LONG nResult;
  DWORD dwIdx1, dwIdx2;
  TCHAR szData[64], *pKeyName1, *pKeyName2, *pKeyName3, *pKeyName4; 
  DWORD dwSize, dwType = REG_SZ;
  BOOL bDone = FALSE;
  FILETIME ft;

  hKey1 = hKey2 = hKey3 = NULL;
  pKeyName1 = pKeyName2 = pKeyName3 = pKeyName4 = NULL;
 
  // ��L�[�̃I�[�v��
  nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SUBKEY, 0, KEY_READ, &hKey1);
  if (nResult != ERROR_SUCCESS) {
    return NULL;
  }
  pKeyName1 = (TCHAR*)malloc(sizeof(TCHAR)*BUFSZ);
  pKeyName2 = (TCHAR*)malloc(sizeof(TCHAR)*BUFSZ);
  pKeyName3 = (TCHAR*)malloc(sizeof(TCHAR)*BUFSZ);
  pKeyName4 = (TCHAR*)malloc(sizeof(TCHAR)*BUFSZ);
 
  dwIdx1 = 0;
  while (bDone != TRUE) { // {id1} ��񋓂��郋�[�v
 
    dwSize = BUFSZ;
    nResult = RegEnumKeyEx(hKey1, dwIdx1++, pKeyName1,
                          &dwSize, NULL, NULL, NULL, &ft);
    if (nResult == ERROR_NO_MORE_ITEMS) {
      break;
    }
 
    // SUBKEY\{id1} �L�[���I�[�v��
    _stprintf(pKeyName2, _T("%s\\%s"), SUBKEY, pKeyName1);
    nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pKeyName2,
                          0, KEY_READ, &hKey2);
    if (nResult != ERROR_SUCCESS) {
      continue;
    }
    dwIdx2 = 0;
    while (1) { // {id2} ��񋓂��郋�[�v
      dwSize = BUFSZ;
      nResult = RegEnumKeyEx(hKey2, dwIdx2++, pKeyName3,
                          &dwSize, NULL, NULL, NULL, &ft);
      if (nResult == ERROR_NO_MORE_ITEMS) {
        break;
      }
 
      if (nResult != ERROR_SUCCESS) {
        continue;
      }
 
      // SUBKEY\{id1}\{id2]\Connection �L�[���I�[�v��
      _stprintf(pKeyName4, _T("%s\\%s\\%s"),
                      pKeyName2, pKeyName3, _T("Connection"));
      nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                      pKeyName4, 0, KEY_READ, &hKey3);
      if (nResult != ERROR_SUCCESS) {
        continue;
      }
 
      // SUBKEY\{id1}\{id2]\Connection\Name �l���擾
      dwSize = sizeof(szData);
      nResult = RegQueryValueEx(hKey3, _T("Name"),
                      0, &dwType, (LPBYTE)szData, &dwSize);
 
      if (nResult == ERROR_SUCCESS) {
        if (_tcsicmp(szData, pDisplayName) == 0) {
           	_tcscpy(pszBuf, pKeyName3);
          bDone = TRUE;
          break;
        }
      }
      RegCloseKey(hKey3);
      hKey3 = NULL;
    }
    RegCloseKey(hKey2);
    hKey2 = NULL;
  }
 
  if (hKey1) { RegCloseKey(hKey1); }
  if (hKey2) { RegCloseKey(hKey2); }
  if (hKey3) { RegCloseKey(hKey3); }
 
  if (pKeyName1) { free(pKeyName1); }
  if (pKeyName2) { free(pKeyName2); }
  if (pKeyName3) { free(pKeyName3); }
  if (pKeyName4) { free(pKeyName4); }
 
  // GUID �𔭌��ł���
  if (bDone != TRUE) {
    return NULL;
  }
  return pszBuf;
}

#endif	/* SUPPORT_NET */