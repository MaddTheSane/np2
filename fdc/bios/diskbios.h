/**
 * @file	diskbios.h
 * @brief	Defines of disk bios
 */

#pragma once

#define DISK_READY				0x00		/*!< 準備完了 */
#define DISK_WRITEPROTECT		0x10		/*!< ライトプロテクト */
#define DISK_DMABOUNDARY		0x20		/*!< DMA アドレス エラー */
#define DISK_EQUIPMENTCHECK		0x40		/*!< ドライブ名が無効 */
#define DISK_NOREADY			0x60		/*!< ドライブの準備ができていない */
#define DISK_NOTWRITABLE		0x70		/*!< ライト・プロテクトされている */
#define DISK_IDERROR			0xa0		/*!< CRCエラー(ID部) */
#define DISK_DATAERROR			0xb0		/*!< CRCエラー(データ部) */
#define DISK_NODATA				0xc0		/*!< 指定したセクタが存在しない */
#define DISK_BADCYLINDER		0xd0		/*!< 指定したシリンダが存在しない */
#define DISK_MISSINGID			0xe0		/*!< トラックにIDが存在しない */
