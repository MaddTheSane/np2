/**
 * @file	opna.h
 * @brief	Interface of OPNA
 */

#pragma once

#include "adpcm.h"
#include "opngen.h"
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
	OPNA_HAS_YM3438		= 0x08,		/*!< Has YM3438 */
	OPNA_HAS_VR			= 0x10,		/*!< Has VR */
	OPNA_REALCHIP		= 0x20,		/*!< Allow realchip */
	OPNA_S98			= 0x40		/*!< Supports S98 */
};

/**
 * @brief opna
 */
struct tagOpnaState
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
	UINT8	keyreg[16];
	UINT8	reg[0x400];
};

/**
 * @brief opna
 */
struct tagOpna
{
	struct tagOpnaState s;
	_OPNGEN opngen;
	_PSGGEN psg;
	_RHYTHM rhythm;
	_ADPCM adpcm;
};

typedef struct tagOpna _OPNA;
typedef struct tagOpna* POPNA;
typedef const struct tagOpna* PCOPNA;

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
void opna_write3438Register(POPNA opna, UINT nAddress, REG8 cData);
void opna_write3438ExtRegister(POPNA opna, UINT nAddress, REG8 cData);

REG8 opna_readRegister(POPNA opna, UINT nAddress);
REG8 opna_readExtendedRegister(POPNA opna, UINT nAddress);
REG8 opna_read3438Register(POPNA opna, UINT nAddress);
REG8 opna_read3438ExtRegister(POPNA opna, UINT nAddress);

#ifdef __cplusplus
}
#endif
