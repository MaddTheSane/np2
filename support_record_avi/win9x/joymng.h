/**
 *	@file	joymng.h
 *	@brief	ジョイパッド入力の宣言およびインターフェイスの定義をします
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
