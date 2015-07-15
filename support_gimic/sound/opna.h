/**
 * @file	opna.h
 * @brief	Interface of OPNA
 */

#pragma once

/**
 * Chips flags
 */
enum
{
	OPNA_HAS_TIMER		= 0x01,		/*!< Has timer */
	OPNA_HAS_EXTENDEDFM	= 0x02,		/*!< Is OPNA */
	OPNA_HAS_ADPCM		= 0x04,		/*!< Has ADPCM DRAM */
	OPNA_HAS_3438		= 0x08,		/*!< Has YM3438 */
	OPNA_REALCHIP		= 0x10,		/*!< Allow realchip */
	OPNA_S98			= 0x20		/*!< Supports S98 */

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

void opna_initialize(POPNA opna);
void opna_reset(POPNA opna, REG8 cCaps);
void opna_bind(POPNA opna);

REG8 opna_readStatus(POPNA opna);
REG8 opna_readExtendedStatus(POPNA opna);

void opna_writeRegister(POPNA opna, UINT nAddress, REG8 cData);
void opna_writeExtendedRegister(POPNA opna, UINT nAddress, REG8 cData);

REG8 opna_readRegister(POPNA opna, UINT nAddress);
REG8 opna_readExtenedRegister(POPNA opna, UINT nAddress);

#ifdef __cplusplus
}
#endif
