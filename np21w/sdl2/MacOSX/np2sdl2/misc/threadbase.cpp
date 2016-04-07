/**
 * @file	threadbase.cpp
 * @brief	�X���b�h���N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "threadbase.h"

/**
 * �R���X�g���N�^
 */
CThreadBase::CThreadBase()
	: m_bCreated(false)
	, m_bDone(false)
	, m_stack_size(0)
{
}

/**
 * �f�X�g���N�^
 */
CThreadBase::~CThreadBase()
{
	Stop();
}

/**
 * �X���b�h�J�n
 * @retval true ����
 */
bool CThreadBase::Start()
{
	if (m_bCreated)
	{
		return false;
	}

	/* �X�^�b�N �T�C�Y���� */
	pthread_attr_t tattr;
	::pthread_attr_init(&tattr);
	if (m_stack_size != 0)
	{
		::pthread_attr_setstacksize(&tattr, m_stack_size);
	}

	m_bDone = false;
	if (::pthread_create(&m_thread, &tattr, StartRoutine, this) != 0)
	{
		return false;
	}

	m_bCreated = true;
	return true;
}

/**
 * �X���b�h�I��
 * @retval true ����
 */
void CThreadBase::Stop()
{
	if (m_bCreated)
	{
		m_bDone = true;
		::pthread_join(m_thread, NULL);
		m_bCreated = false;
	}
}

/**
 * �X���b�h����
 * @param[in] arg this �|�C���^
 * @retval 0 ���0
 */
void* CThreadBase::StartRoutine(void* arg)
{
	CThreadBase& obj = *(static_cast<CThreadBase*>(arg));
	while ((!obj.m_bDone) && (obj.Task()))
	{
	}

	return 0;
}
