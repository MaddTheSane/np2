/**
 * @file	boardspb.h
 * @brief	Interface of Speak board
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

void boardspb_reset(const NP2CFG *pConfig);
void boardspb_bind(void);

void boardspr_reset(const NP2CFG *pConfig);
void boardspr_bind(void);

#ifdef __cplusplus
}
#endif
