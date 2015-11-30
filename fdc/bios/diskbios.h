/**
 * @file	diskbios.h
 * @brief	Defines of disk bios
 */

#pragma once

#define DISK_READY				0x00		/*!< �������� */
#define DISK_WRITEPROTECT		0x10		/*!< ���C�g�v���e�N�g */
#define DISK_DMABOUNDARY		0x20		/*!< DMA �A�h���X �G���[ */
#define DISK_EQUIPMENTCHECK		0x40		/*!< �h���C�u�������� */
#define DISK_NOREADY			0x60		/*!< �h���C�u�̏������ł��Ă��Ȃ� */
#define DISK_NOTWRITABLE		0x70		/*!< ���C�g�E�v���e�N�g����Ă��� */
#define DISK_IDERROR			0xa0		/*!< CRC�G���[(ID��) */
#define DISK_DATAERROR			0xb0		/*!< CRC�G���[(�f�[�^��) */
#define DISK_NODATA				0xc0		/*!< �w�肵���Z�N�^�����݂��Ȃ� */
#define DISK_BADCYLINDER		0xd0		/*!< �w�肵���V�����_�����݂��Ȃ� */
#define DISK_MISSINGID			0xe0		/*!< �g���b�N��ID�����݂��Ȃ� */
