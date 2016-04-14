/*!
 * @file	guard.h
 * @brief	�N���e�B�J�� �Z�N�V���� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <pthread.h>

/*!
 * @brief �N���e�B�J�� �Z�N�V���� �N���X
 */
class CGuard
{
public:
	/*! �R���X�g���N�^ */
	CGuard() { ::pthread_mutex_init(&m_cs, NULL); }

	/*! �f�X�g���N�^ */
	~CGuard() { ::pthread_mutex_destroy(&m_cs); }

	/*! �N���e�B�J�� �Z�N�V�����J�n */
	void Enter() { ::pthread_mutex_lock(&m_cs); }

	/*! �N���e�B�J�� �Z�N�V�����I�� */
	void Leave() { ::pthread_mutex_unlock(&m_cs); }

private:
	pthread_mutex_t m_cs;		//!< �N���e�B�J�� �Z�N�V�������
};
