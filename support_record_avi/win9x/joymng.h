/**
 *	@file	joymng.h
 *	@brief	�W���C�p�b�h���͂̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

REG8 joymng_getstat();

#ifdef __cplusplus
}
#endif


// ----

void joymng_initialize();
void joymng_sync();
