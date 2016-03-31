/**
 * @file	tty.h
 * @brief	�V���A���ʐM�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <termios.h>

/**
 * @brief �V���A���ʐM
 */
class CTty
{
public:
	CTty();
	~CTty();
	bool Open(const char* dev, unsigned int speed = 0, const char* param = NULL);
	void Close();
	ssize_t Read(void* data_ptr, ssize_t data_size);
	ssize_t Write(const void* data_ptr, ssize_t data_size);
	bool IsOpened() const;

private:
	int m_fd;		//!< �t�@�C�� �f�B�X�N���v�^
	static bool SetParam(const char* param, tcflag_t* cflag_ptr);
};

/**
 * �I�[�v����?
 * @retval true �I�[�v����
 * @retval false ���I�[�v��
 */
inline bool CTty::IsOpened() const
{
	return (m_fd >= 0);
}
