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
	bool IsOpened() const;
	void Close();
	int Read(LPVOID lpcvData, int nDataSize);
	int Write(LPCVOID lpcvData, int nDataSize);

private:
	HANDLE m_hFile;				/*!< �t�@�C�� �n���h�� */
	static bool SetParam(LPCTSTR lpcszParam, DCB* dcb = NULL);
	static LPCTSTR GetBoolString(BOOL f);
};

/**
 *
 */
inline bool CTty::IsOpened() const
{
	return (m_hFile != INVALID_HANDLE_VALUE);
}

/**
 * TRUE/FALSE �������Ԃ�
 * @param[in] f BOOL�l
 * @return ������|�C���^
 */
inline LPCTSTR CTty::GetBoolString(BOOL f)
{
	return (f) ? TEXT("TRUE") : TEXT("FALSE");
}
