/**
 * @file	cmserial.h
 * @brief	�V���A�� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "commng.h"

extern const UINT32 cmserial_speed[10];

COMMNG cmserial_create(UINT port, UINT8 param, UINT32 speed);
