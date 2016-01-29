/**
 * @file	sdbase.h
 * @brief	�T�E���h �f�o�C�X���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief �T�E���h �f�[�^�擾�C���^�t�F�C�X
 */
class ISoundData
{
public:
	/**
	 * �X�g���[�� �f�[�^�𓾂�
	 * @param[out] lpBuffer �o�b�t�@
	 * @param[in] nBufferCount �o�b�t�@ �J�E���g
	 * @return �T���v����
	 */
	virtual UINT Get16(SINT16* lpBuffer, UINT nBufferCount) = 0;
};

/**
 * @brief �T�E���h �f�o�C�X���N���X
 */
class CSoundDeviceBase
{
public:
	/**
	 * �R���X�g���N�^
	 */
	CSoundDeviceBase()
		: m_pSoundData(NULL)
	{
	}

	/**
	 * �X�g���[�� �f�[�^�̐ݒ�
	 * @param[in] pSoundData �T�E���h �f�[�^
	 */
	void SetStreamData(ISoundData* pSoundData)
	{
		m_pSoundData = pSoundData;
	}

protected:
	ISoundData* m_pSoundData;		/*!< �T�E���h �f�[�^ �C���X�^���X */
};
