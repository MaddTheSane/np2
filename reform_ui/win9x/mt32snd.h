/**
 * @file	mt32snd.h
 * @brief	MT32Sound �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#if defined(MT32SOUND_DLL)

/**
 * @brief MT32Sound �A�N�Z�X �N���X
 */
class MT32Sound
{
public:
	static MT32Sound* GetInstance();

	MT32Sound();
	~MT32Sound();
	bool Initialize();
	void Deinitialize();
	bool IsEnabled() const;
	void SetRate(UINT nRate);

	bool Open();
	void Close();
	void ShortMsg(UINT32 msg);
	void LongMsg(const UINT8* lpBuffer, UINT cchBuffer);
	UINT Mix(SINT32* lpBuffer, UINT cchBuffer);

private:
	static MT32Sound sm_instance;	//!< �B��̃C���X�^���X�ł�

	//! @brief ���[�h�֐�
	struct ProcItem
	{
		LPCSTR lpSymbol;			//!< �֐���
		size_t nOffset;				//!< �I�t�Z�b�g
	};

	// ��`
	typedef int (WINAPI * FnOpen)(int rate, int reverb, int def, int revtype, int revtime, int revlvl); 	/*!< �I�[�v�� */
	typedef int (WINAPI * FnClose)(void);																	/*!< �N���[�Y */
	typedef int (WINAPI * FnWrite)(unsigned char data);														/*!< ���C�g */
	typedef int (WINAPI * FnMix)(void *buff, unsigned long size);											/*!< �~�b�N�X */

	HMODULE m_hModule;	/*!< ���W���[�� */
	bool m_bOpened;		/*!< �I�[�v�� �t���O */
	UINT m_nRate;		/*!< �T���v�����O ���[�g */
	FnOpen m_fnOpen;	/*!< �I�[�v���֐� */
	FnClose m_fnClose;	/*!< �N���[�Y�֐� */
	FnWrite m_fnWrite;	/*!< ���C�g�֐� */
	FnMix m_fnMix;		/*!< �~�b�N�X�֐� */
};

/**
 * �C���X�^���X���擾
 * @return �C���X�^���X
 */
inline MT32Sound* MT32Sound::GetInstance()
{
	return &sm_instance;
}

/**
 * �L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool MT32Sound::IsEnabled() const
{
	return (m_hModule != NULL);
}

/**
 * �T���v�����O ���[�g��ݒ�
 * @param[in] nRate ���[�g
 */
inline void MT32Sound::SetRate(UINT nRate)
{
	m_nRate = nRate;
}


// ----

inline BRESULT mt32sound_initialize(void)
{
	return MT32Sound::GetInstance()->Initialize() ? SUCCESS : FAILURE;
}

inline void mt32sound_deinitialize()
{
	MT32Sound::GetInstance()->Deinitialize();
}

inline BOOL mt32sound_isenable(void)
{
	return MT32Sound::GetInstance()->IsEnabled() ? TRUE : FALSE;
}

inline void mt32sound_setrate(UINT rate)
{
	MT32Sound::GetInstance()->SetRate(rate);
}

inline BRESULT mt32sound_open(void)
{
	return MT32Sound::GetInstance()->Open() ? SUCCESS : FAILURE;
}

inline void mt32sound_close(void)
{
	MT32Sound::GetInstance()->Close();
}

inline void mt32sound_shortmsg(UINT32 msg)
{
	MT32Sound::GetInstance()->ShortMsg(msg);
}

inline void mt32sound_longmsg(const UINT8 *ptr, UINT32 leng)
{
	MT32Sound::GetInstance()->LongMsg(ptr, leng);
}

inline UINT mt32sound_mix32(SINT32 *buff, UINT leng)
{
	return MT32Sound::GetInstance()->Mix(buff, leng);
}

#endif
