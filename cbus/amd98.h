/**
 * @file	amd98.h
 * @brief	Interface of AMD-98
 */

#pragma once

/**
 * @breif The sturecture of AMD-98
 */
struct amd98_t
{
	UINT16	port;
	UINT8	psg3reg;
	UINT8	rhythm;
};

typedef struct amd98_t AMD98;

#ifdef __cplusplus
extern "C"
{
#endif

extern	AMD98	g_amd98;

void amd98_initialize(UINT rate);
void amd98_deinitialize(void);

void amd98int(NEVENTITEM item);

void amd98_reset(const NP2CFG *pConfig);
void amd98_bind(void);

#ifdef __cplusplus
}
#endif

