/**
 * @file	opl3.c
 * @brief	Implementation of OPL3
 */

#include "compiler.h"
#include "opl3.h"

static void writeRegister(POPL3 opl3, UINT nAddress, REG8 cData);
static void writeExtendedRegister(POPL3 opl3, UINT nAddress, REG8 cData);

/**
 * Initialize instance
 * @param[in] opl3 The instance
 */
void opl3_construct(POPL3 opl3)
{
	memset(opl3, 0, sizeof(*opl3));
}

/**
 * Deinitialize instance
 * @param[in] opl3 The instance
 */
void opl3_destruct(POPL3 opl3)
{
}

/**
 * Reset
 * @param[in] opl3 The instance
 * @param[in] cCaps
 */
void opl3_reset(POPL3 opl3, REG8 cCaps)
{
	memset(&opl3->s, 0, sizeof(opl3->s));
	opl3->s.cCaps = cCaps;
}

/**
 * Restore
 * @param[in] opl3 The instance
 */
static void restore(POPL3 opl3)
{
}

/**
 * Bind
 * @param[in] opl3 The instance
 */
void opl3_bind(POPL3 opl3)
{
	restore(opl3);
}

/**
 * Status
 * @param[in] opl3 The instance
 * @return Status
 */
REG8 opl3_readStatus(POPL3 opl3)
{
	return 0;
}

/**
 * Writes register
 * @param[in] opl3 The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
void opl3_writeRegister(POPL3 opl3, UINT nAddress, REG8 cData)
{
	opl3->s.reg[nAddress] = cData;
	writeRegister(opl3, nAddress, cData);
}

/**
 * Writes register (Inner)
 * @param[in] opl3 The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
static void writeRegister(POPL3 opl3, UINT nAddress, REG8 cData)
{
}

/**
 * Writes extended register
 * @param[in] opl3 The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
void opl3_writeExtendedRegister(POPL3 opl3, UINT nAddress, REG8 cData)
{
	opl3->s.reg[nAddress + 0x100] = cData;
	writeExtendedRegister(opl3, nAddress, cData);
}

/**
 * Writes extended register (Inner)
 * @param[in] opl3 The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
static void writeExtendedRegister(POPL3 opl3, UINT nAddress, REG8 cData)
{
}

/**
 * Reads register
 * @param[in] opl3 The instance
 * @param[in] nAddress The address
 * @return data
 */
REG8 opl3_readRegister(POPL3 opl3, UINT nAddress)
{
	return 0xff;
}

/**
 * Reads extended register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @return data
 */
REG8 opl3_readExtendedRegister(POPL3 opl3, UINT nAddress)
{
	return 0xff;
}



// ---- statsave

/**
 * Save
 * @param[in] opl3 The instance
 * @param[in] sfh The handle of statsave
 * @param[in] tbl The item of statsave
 * @return Error
 */
int opl3_sfsave(PCOPL3 opl3, STFLAGH sfh, const SFENTRY *tbl)
{
	return statflag_write(sfh, &opl3->s, sizeof(opl3->s));
}

/**
 * Load
 * @param[in] opl3 The instance
 * @param[in] sfh The handle of statsave
 * @param[in] tbl The item of statsave
 * @return Error
 */
int opl3_sfload(POPL3 opl3, STFLAGH sfh, const SFENTRY *tbl)
{
	return statflag_read(sfh, &opl3->s, sizeof(opl3->s));
}
