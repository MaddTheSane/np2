/**
 * @file	extendmodule.h
 * @brief	�O�t���W���[�� �C���^�[�t�F�C�X�̒�`���s���܂�
 */

#pragma once

/**
 * @brief �O�t���W���[�� �C���^�[�t�F�C�X
 */
class IExtendModule
{
public:
	/**
	 * �f�X�g���N�^
	 */
	virtual ~IExtendModule() { }

	/**
	 * ������
	 * @retval true ����
	 * @retval false ���s
	 */
	virtual bool Initialize() = 0;

	/**
	 * ���
	 */
	virtual void Deinitialize() = 0;

	/**
	 * �f�o�C�X�͗L��?
	 * @retval true �L��
	 * @retval false ����
	 */
	virtual bool IsEnabled() = 0;

	/**
	 * �r�W�[?
	 * @retval true �r�W�[
	 * @retval false ���f�B
	 */
	virtual bool IsBusy() = 0;

	/**
	 * �������Z�b�g
	 */
	virtual void Reset() = 0;

	/**
	 * ���W�X�^��������
	 * @param[in] nAddr �A�h���X
	 * @param[in] cData �f�[�^
	 */
	virtual void WriteRegister(UINT nAddr, UINT8 cData) = 0;
};
