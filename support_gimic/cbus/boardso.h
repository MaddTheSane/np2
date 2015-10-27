/**
 * @file	boardso.h
 * @brief	Interface of Sound Orchestra
 */

#pragma once

#include "pccore.h"

#ifdef __cplusplus
extern "C"
{
#endif

void boardso_reset(const NP2CFG *pConfig);
void boardso_bind(void);

#ifdef __cplusplus
}
#endif
