/**
 * @file	soundmng.h
 * @brief	�T�E���h �}�l�[�W���̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * PCM �ԍ�
 */
enum SoundPCMNumber
{
	SOUND_PCMSEEK		= 0,		/*!< �w�b�h�ړ� */
	SOUND_PCMSEEK1					/*!< 1�N���X�^�ړ� */
};

#ifdef __cplusplus
extern "C"
{
#endif

UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
void soundmng_reset(void);
void soundmng_play(void);
void soundmng_stop(void);
void soundmng_sync(void);
void soundmng_setreverse(BOOL bReverse);

BRESULT soundmng_pcmplay(enum SoundPCMNumber nNum, BOOL bLoop);
void soundmng_pcmstop(enum SoundPCMNumber nNum);

#ifdef __cplusplus
}



// ---- for windows

BRESULT soundmng_initialize(void);
void soundmng_deinitialize(void);

void soundmng_pcmload(SoundPCMNumber nNum, LPCTSTR lpFilename);
void soundmng_pcmvolume(SoundPCMNumber nNum, int nVolume);

/**
 * �T�E���h �v���V�[�W��
 */
enum SoundProc
{
	SNDPROC_MASTER		= 0,
	SNDPROC_MAIN,
	SNDPROC_TOOL,
	SNDPROC_SUBWIND
};

void soundmng_enable(SoundProc nProc);
void soundmng_disable(SoundProc nProc);

#endif
