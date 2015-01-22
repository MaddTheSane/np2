/**
 * @file	viewmem.h
 * @brief	DebugUty 用メモリ読み込みクラスの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * @brief メモリ読み込み
 */
struct DebugUtyViewMemory
{
	UINT8	vram;
	UINT8	itf;
	UINT8	A20;

	DebugUtyViewMemory();
	void Update();
	void Read(UINT32 nAddress, LPVOID lpBuffer, UINT32 cbBuffer) const;
};
