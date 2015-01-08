/**
 * @file	viewmem.h
 * @brief	DebugUty �p�������ǂݍ��݃N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief �������ǂݍ���
 */
struct DebugUtyViewMemory
{
	UINT8	vram;
	UINT8	itf;
	UINT8	A20;

	void Update();
	void Read(UINT32 nAddress, LPVOID lpBuffer, UINT32 cbBuffer);
};
