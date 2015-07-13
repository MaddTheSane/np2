/**
 * @file	tty.h
 * @brief	�V���A���ʐM�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief �V���A���ʐM
 */
class CTty
{
public:
	CTty();
	~CTty();
	bool Open(int nPort, UINT nSpeed = 0, LPCTSTR lpcszParam = NULL);
	bool Open(LPCTSTR lpDevName, UINT nSpeed = 0, LPCTSTR lpcszParam = NULL);
	bool IsOpened() const;
	void Close();
	int Read(LPVOID lpcvData, int nDataSize);
	int Write(LPCVOID lpcvData, int nDataSize);

private:
	HANDLE m_hFile;				/*!< �t�@�C�� �n���h�� */
	bool OpenPort(LPCTSTR lpPortName, UINT nSpeed, LPCTSTR lpcszParam);
	static bool SetParam(LPCTSTR lpcszParam, DCB* dcb = NULL);
	static bool GetPortName(LPCTSTR lpDevName, LPTSTR lpDosName, UINT cchDosName);
};

/**
 * �I�[�v����?
 * @retval true �I�[�v����
 * @retval false ���I�[�v��
 */
inline bool CTty::IsOpened() const
{
	return (m_hFile != INVALID_HANDLE_VALUE);
}
