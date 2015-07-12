/**
 * @file	ttyl.cpp
 * @brief	�V���A���ʐM�N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "tty.h"

/**
 * �R���X�g���N�^
 */
CTty::CTty()
	: m_hFile(INVALID_HANDLE_VALUE)
{
}

/**
 * �f�X�g���N�^
 */
CTty::~CTty()
{
	Close();
}

/**
 * �I�[�v������
 * @param[in] nPort �|�[�g�ԍ�
 * @param[in] nSpeed �{�[���[�g
 * @param[in] lpcszParam �p�����^
 * @retval true ����
 * @retval false ���s
 */
bool CTty::Open(int nPort, UINT nSpeed, LPCTSTR lpcszParam)
{
	Close();

	if (!SetParam(lpcszParam, NULL))
	{
		return false;
	}

	TCHAR szPort[16];
	::wsprintf(szPort, TEXT("COM%u"), nPort);
	HANDLE hFile = ::CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DCB dcb;
	::GetCommState(hFile, &dcb);
	if (nSpeed != 0)
	{
		dcb.BaudRate = nSpeed;
	}
	SetParam(lpcszParam, &dcb);

	dcb.fOutxCtsFlow = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;

	if (!::SetCommState(hFile, &dcb))
	{
		::CloseHandle(hFile);
		return false;
	}

	m_hFile = hFile;
	return true;
}

/**
 * �N���[�Y����
 */
void CTty::Close()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

/**
 * �f�[�^��M
 * @param[in] lpcvData ���M�f�[�^�̃|�C���^
 * @param[in] nDataSize ���M�f�[�^�̃T�C�Y
 * @return ���M�o�C�g��
 */
int CTty::Read(LPVOID lpcvData, int nDataSize)
{
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	if ((lpcvData == NULL) || (nDataSize <= 0))
	{
		return 0;
	}

	DWORD dwReadSize = 0;
	if (!::ReadFile(m_hFile, lpcvData, nDataSize, &dwReadSize, NULL))
	{
		// DEBUGLOG(_T("Failed to write."));
		return 0;
	}
	const int r = static_cast<int>(dwReadSize);
	if (r != nDataSize)
	{
		// DEBUGLOG(_T("Failed to write (%d/%d)."), nDataSize, r);
	}
	return r;
}

/**
 * �f�[�^���M
 * @param[in] lpcvData ���M�f�[�^�̃|�C���^
 * @param[in] nDataSize ���M�f�[�^�̃T�C�Y
 * @return ���M�o�C�g��
 */
int CTty::Write(LPCVOID lpcvData, int nDataSize)
{
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	if ((lpcvData == NULL) || (nDataSize <= 0))
	{
		return 0;
	}

	DWORD dwWrittenSize = 0;
	if (!::WriteFile(m_hFile, lpcvData, nDataSize, &dwWrittenSize, NULL))
	{
		// DEBUGLOG(_T("Failed to write."));
		return 0;
	}
	const int r = static_cast<int>(dwWrittenSize);
	if (r != nDataSize)
	{
		// DEBUGLOG(_T("Failed to write (%d/%d)."), nDataSize, r);
	}
	return r;
}

/**
 * �p�����[�^�ݒ�
 * @param[in] lpcszParam �p�����^
 * @param[in, out] dcb DCB �\���̂̃|�C���^
 * @retval true ����
 * @retval false ���s
 */
bool CTty::SetParam(LPCTSTR lpcszParam, DCB* dcb)
{
	BYTE cByteSize = 8;
	BYTE cParity = NOPARITY;
	BYTE cStopBits = ONESTOPBIT;

	if (lpcszParam != NULL)
	{
		TCHAR c = lpcszParam[0];
		if ((c < TEXT('4')) || (c > TEXT('8')))
		{
			return false;
		}
		cByteSize = static_cast<BYTE>(c - TEXT('0'));

		c = lpcszParam[1];
		switch (c & (~0x20))
		{
			case TEXT('N'):		// for no parity
				cParity = NOPARITY;
				break;

			case TEXT('E'):		// for even parity
				cParity = EVENPARITY;
				break;

			case TEXT('O'):		// for odd parity
				cParity = ODDPARITY;
				break;

			case TEXT('M'):		// for mark parity
				cParity = MARKPARITY;
				break;

			case TEXT('S'):		// for for space parity
				cParity = SPACEPARITY;
				break;

			default:
				return false;
		}

		if (::lstrcmp(lpcszParam + 2, TEXT("1")) == 0)
		{
			cStopBits = ONESTOPBIT;
		}
		else if (::lstrcmp(lpcszParam + 2, TEXT("1.5")) == 0)
		{
			cStopBits = ONE5STOPBITS;
		}
		else if (::lstrcmp(lpcszParam + 2, TEXT("2")) == 0)
		{
			cStopBits = TWOSTOPBITS;
		}
		else
		{
			return false;
		}
	}

	if (dcb != NULL)
	{
		dcb->ByteSize = cByteSize;
		dcb->Parity = cParity;
		dcb->StopBits = cStopBits;
	}
	return true;
}
