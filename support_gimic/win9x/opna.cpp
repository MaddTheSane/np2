/**
 * @file	opna.cpp
 * @brief	Implementation of OPNA
 */

#include "compiler.h"
#include "opna.h"
#include "pccore.h"
#include "sound.h"
#include "fmboard.h"
#include "s98.h"
#include "keydisp.h"
#include "ext/externalopna.h"

#if !defined(SUPPORT_ROMEO)
#error Not support ROMEO
#endif

/**
 * Initialize instance
 * @param[in] opna The instance
 */
void opna_initialize(POPNA opna)
{
	int i;

	memset(opna, 0, sizeof(*opna));
	opna->adpcmmask = ~(0x1c);

	for (i = 0; i < 4; i++)
	{
		memset(opna->reg + (i * 0x100) + 0x30, 0xff, 0x60);
		memset(opna->reg + (i * 0x100) + 0xb4, 0xc0, 0x04);
	}
}

/**
 * Reset
 * @param[in] opna The instance
 * @param[in] cCaps
 */
void opna_reset(POPNA opna, REG8 cCaps)
{
	opna->cCaps = cCaps;

	CExternalOpna::GetInstance()->Reset();
}

/**
 * Bind
 * @param[in] opna The instance
 */
void opna_bind(POPNA opna)
{
	const UINT8 cCaps = opna->cCaps;

	if (CExternalOpna::GetInstance()->IsEnabled())
	{
		if (cCaps & OPNA_HAS_ADPCM)
		{
			sound_streamregist(&g_adpcm, (SOUNDCB)adpcm_getpcm_dummy);
		}
		return;
	}

	psggen_restore(&g_psg1);
	sound_streamregist(&g_psg1, (SOUNDCB)psggen_getpcm);

	fmboard_fmrestore(opna, 0, 0);
	if (cCaps & OPNA_HAS_EXTENDEDFM)
	{
		fmboard_fmrestore(opna, 3, 1);
	}
	if (cCaps & OPNA_HAS_YM3438)
	{
		fmboard_fmrestore(opna, 6, 2);
		fmboard_fmrestore(opna, 9, 3);
	}
	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);

	if (cCaps & OPNA_HAS_EXTENDEDFM)
	{
		fmboard_rhyrestore(opna, &g_rhythm, 0);
		rhythm_bind(&g_rhythm);
	}
	if (cCaps & OPNA_HAS_ADPCM)
	{
		sound_streamregist(&g_adpcm, (SOUNDCB)adpcm_getpcm);
	}
}

/**
 * Status
 * @param[in] opna The instance
 * @return Status
 */
REG8 opna_readStatus(POPNA opna)
{
	if (opna->cCaps & OPNA_HAS_TIMER)
	{
		return g_fmtimer.status;
	}
	return 0;
}

/**
 * Status
 * @param[in] opna The instance
 * @return Status
 */
REG8 opna_readExtendedStatus(POPNA opna)
{
	const UINT8 cCaps = opna->cCaps;
	REG8 ret = 0;

	if (cCaps & OPNA_HAS_ADPCM)
	{
		ret = adpcm_status(&g_adpcm);
	}
	else
	{
		ret = opna->adpcmmask & 8;
	}

	if (cCaps & OPNA_HAS_TIMER)
	{
		ret |= g_fmtimer.status;
	}

	return ret;
}

/**
 * Writes register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
void opna_writeRegister(POPNA opna, UINT nAddress, REG8 cData)
{
	const UINT8 cCaps = opna->cCaps;
	REG8 cChannel;

	opna->reg[nAddress] = cData;

	if (cCaps & OPNA_S98)
	{
		S98_put(NORMAL2608, nAddress, cData);
	}

	if (nAddress < 0x10)
	{
		if (nAddress != 0x0e)
		{
			psggen_setreg(&g_psg1, nAddress, cData);
		}
	}
	else if (nAddress < 0x20)
	{
		if (cCaps & OPNA_HAS_EXTENDEDFM)
		{
			rhythm_setreg(&g_rhythm, nAddress, cData);
		}
	}
	else if (nAddress < 0x30)
	{
		if (nAddress == 0x28)
		{
			cChannel = cData & 0x0f;
			if (cChannel < 3)
			{
				opngen_keyon(cChannel, cData);
			}
			else if ((cCaps & OPNA_HAS_EXTENDEDFM) && (cChannel >= 4) && (cChannel < 7))
			{
				opngen_keyon(cChannel - 1, cData);
			}
		}
		else
		{
			if (cCaps & OPNA_HAS_TIMER)
			{
				fmtimer_setreg(nAddress, cData);
			}
			if (nAddress == 0x27)
			{
				opnch[2].extop = cData & 0xc0;
			}
		}
	}
	else if (nAddress < 0xc0)
	{
		opngen_setreg(0, nAddress, cData);
	}

	CExternalOpna::GetInstance()->WriteRegister(nAddress, cData);
}

/**
 * Writes extened register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
void opna_writeExtendedRegister(POPNA opna, UINT nAddress, REG8 cData)
{
	const UINT8 cCaps = opna->cCaps;

	opna->reg[nAddress + 0x100] = cData;

	if (cCaps & OPNA_S98)
	{
		S98_put(EXTEND2608, nAddress, cData);
	}

	if (nAddress < 0x12)
	{
		if (cCaps & OPNA_HAS_ADPCM)
		{
			adpcm_setreg(&g_adpcm, nAddress, cData);
		}
		else
		{
			if (nAddress == 0x10)
			{
				if (!(cData & 0x80))
				{
					opna->adpcmmask = ~(cData & 0x1c);
				}
			}
		}
	}
	else if (nAddress >= 0x30)
	{
		if (cCaps & OPNA_HAS_EXTENDEDFM)
		{
			opngen_setreg(3, nAddress, cData);
		}
	}

	CExternalOpna::GetInstance()->WriteRegister(nAddress + 0x100, cData);
}

/**
 * Reads register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @return data
 */
REG8 opna_readRegister(POPNA opna, UINT nAddress)
{
	if (nAddress == 0xff)
	{
		return (opna->cCaps & OPNA_HAS_EXTENDEDFM) ? 1 : 0;
	}
	return opna->reg[nAddress];
}

/**
 * Reads extened register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @return data
 */
REG8 opna_readExtenedRegister(POPNA opna, UINT nAddress)
{
	if ((opna->cCaps & OPNA_HAS_ADPCM) && (nAddress == 0x08))
	{
		return adpcm_readsample(&g_adpcm);
	}
	return opna->reg[nAddress + 0x100];
}
