/**
 * @file	opna.h
 * @brief	Interface of OPNA
 */

#pragma once

#include "psggen.h"
#include "rhythm.h"

/**
 * Chips flags
 */
enum
{
	OPNA_HAS_TIMER		= 0x01,		/*!< Has timer */
	OPNA_HAS_EXTENDEDFM	= 0x02,		/*!< Is OPNA */
	OPNA_HAS_ADPCM		= 0x04,		/*!< Has ADPCM DRAM */
	OPNA_HAS_VR			= 0x10,		/*!< Has VR */
	OPNA_REALCHIP		= 0x20,		/*!< Allow realchip */
	OPNA_S98			= 0x40		/*!< Supports S98 */
};

/**
 * @brief opna
 */
struct tagOpna
{
	UINT8	addr1l;
	UINT8	addr1h;
	UINT8	addr2l;
	UINT8	addr2h;
	UINT8	data1;
	UINT8	data2;
	UINT16	base;
	UINT8	adpcmmask;
	UINT8	channels;
	UINT8	extend;
	UINT8	cCaps;
	UINT8	reg[0x400];
};

typedef struct tagOpna OPN_T;
typedef struct tagOpna* POPNA;

#ifdef __cplusplus
extern "C"
{
#endif

void opna_initialize(void);
void opna_deinitialize(void);

void opna_construct(POPNA opna);
void opna_reset(POPNA opna, REG8 cCaps);
void opna_bind(POPNA opna);

REG8 opna_readStatus(POPNA opna);
REG8 opna_readExtendedStatus(POPNA opna);

void opna_writeRegister(POPNA opna, UINT nAddress, REG8 cData);
void opna_writeExtendedRegister(POPNA opna, UINT nAddress, REG8 cData);

REG8 opna_readRegister(POPNA opna, UINT nAddress);
REG8 opna_readExtendedRegister(POPNA opna, UINT nAddress);

void fmboard_fmrestore(OPN_T* pOpn, REG8 chbase, UINT bank);
void fmboard_psgrestore(OPN_T* pOpn, PSGGEN psg, UINT bank);
void fmboard_rhyrestore(OPN_T* pOpn, RHYTHM rhy, UINT bank);

#ifdef __cplusplus
}
#endif
