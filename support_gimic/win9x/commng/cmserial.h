/**
 * @file	cmserial.h
 * @brief	シリアル クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "commng.h"

extern const UINT32 cmserial_speed[10];

COMMNG cmserial_create(UINT port, UINT8 param, UINT32 speed);
