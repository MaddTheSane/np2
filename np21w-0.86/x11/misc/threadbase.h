/**
 * @file	threadbase.h
 * @brief	�X���b�h���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <pthread.h>
#include <unistd.h>

/**
 * @brief �X���b�h���N���X
 */
class CThreadBase
{
public:
	CThreadBase();
	virtual ~CThreadBase();

	bool Start();
	void Stop();
	void SetStackSize(size_t stack_size);
	static void Delay(unsigned int usec);

protected:
	virtual bool Task()=0;		//!< �X���b�h �^�X�N

private:
	pthread_t m_thread;			//!< �X���b�h �t���O
	bool m_bCreated;			//!< �X���b�h�쐬�t���O
	bool m_bDone;				//!< �I���t���O
	size_t m_stack_size;		//!< �X�^�b�N �T�C�Y
	static void* StartRoutine(void* arg);
};

/**
 * �X�^�b�N �T�C�Y�̐ݒ�
 * @param[in] stack_size �X�^�b�N �T�C�Y
 */
inline void CThreadBase::SetStackSize(size_t stack_size)
{
	m_stack_size = stack_size;
}

/**
 * �X���[�v
 * @param[in] usec �}�C�N���b
 */
inline void CThreadBase::Delay(unsigned int usec)
{
	::usleep(usec);
}
