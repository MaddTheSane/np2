/**
 * @file	opna.cpp
 * @brief	Implementation of OPNA
 */

#include "compiler.h"
#include "opna.h"
#include "pccore.h"
#include "iocore.h"
#include "sound.h"
#include "fmboard.h"
#include "s98.h"
#include "keydisp.h"
#include "externalopna.h"

/**
 * Initialize
 */
void opna_initialize(void)
{
}

/**
 * Deinitialize
 */
void opna_deinitialize(void)
{
	CExternalOpna::GetInstance()->Reset();
	CExternalOpna::GetInstance()->Deinitialize();
}

/**
 * Initialize instance
 * @param[in] opna The instance
 */
void opna_construct(POPNA opna)
{
	memset(opna, 0, sizeof(*opna));
	opna->adpcmmask = ~(0x1c);

	for (int i = 0; i < 4; i++)
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

	CExternalOpna* pExt = CExternalOpna::GetInstance();
	if (!pExt->IsEnabled())
	{
		pExt->Initialize();
		pExt->Reset();
	}

	if (pExt->IsEnabled())
	{
		pExt->WriteRegister(0x22, 0x00);
		pExt->WriteRegister(0x29, 0x80);
		pExt->WriteRegister(0x10, 0xbf);
		pExt->WriteRegister(0x11, 0x30);
		pExt->Restore(opna->reg, (cCaps & OPNA_HAS_EXTENDEDFM) ? true : false);

		if (cCaps & OPNA_HAS_ADPCM)
		{
			if (pExt->HasADPCM())
			{
				sound_streamregist(&g_adpcm, (SOUNDCB)adpcm_getpcm_dummy);
			}
			else
			{
				sound_streamregist(&g_adpcm, (SOUNDCB)adpcm_getpcm);
			}
		}

		if (cCaps & OPNA_HAS_YM3438)
		{
			fmboard_fmrestore(opna, 6, 2);
			fmboard_fmrestore(opna, 9, 3);
			if (cCaps & OPNA_HAS_VR)
			{
				sound_streamregist(&opngen, (SOUNDCB)opngen_getpcmvr);
			}
			else
			{
				sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
			}
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
	if (cCaps & OPNA_HAS_VR)
	{
		sound_streamregist(&opngen, (SOUNDCB)opngen_getpcmvr);
	}
	else
	{
		sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
	}
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
	CExternalOpna* pExt = CExternalOpna::GetInstance();

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
			if (pExt->IsEnabled())
			{
				pExt->WriteRegister(nAddress, cData);
				if (nAddress == 0x07)
				{
					keydisp_psgmix(&g_psg1);
				}
				else if ((nAddress >= 0x08) && (nAddress <= 0x0a))
				{
					keydisp_psgvol(&g_psg1, static_cast<UINT8>(nAddress - 8));
				}
			}
		}
	}
	else if (nAddress < 0x20)
	{
		if (cCaps & OPNA_HAS_EXTENDEDFM)
		{
			if (!pExt->IsEnabled())
			{
				rhythm_setreg(&g_rhythm, nAddress, cData);
			}
			else
			{
				pExt->WriteRegister(nAddress, cData);
			}
		}
	}
	else if (nAddress < 0x30)
	{
		if (nAddress == 0x28)
		{
			REG8 cChannel = cData & 0x0f;
			if (cChannel < 3)
			{
				opngen_keyon(cChannel, cData);
			}
			else if ((cCaps & OPNA_HAS_EXTENDEDFM) && (cChannel >= 4) && (cChannel < 7))
			{
				cChannel--;
			}
			else
			{
				return;
			}

			if (!pExt->IsEnabled())
			{
				opngen_keyon(cChannel, cData);
			}
			else
			{
				keydisp_fmkeyon(cChannel, cData);
				pExt->WriteRegister(nAddress, cData);
			}
		}
		else
		{
			if (cCaps & OPNA_HAS_TIMER)
			{
				fmtimer_setreg(nAddress, cData);
			}

			if (!pExt->IsEnabled())
			{
				if (nAddress == 0x27)
				{
					opnch[2].extop = cData & 0xc0;
				}
			}
			else
			{
				if ((nAddress == 0x22) || (nAddress == 0x27))
				{
					pExt->WriteRegister(nAddress, cData);
				}
			}
		}
	}
	else if (nAddress < 0xc0)
	{
		if (!pExt->IsEnabled())
		{
			opngen_setreg(0, nAddress, cData);
		}
		else
		{
			pExt->WriteRegister(nAddress, cData);
		}
	}
}

/**
 * Writes extended register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
void opna_writeExtendedRegister(POPNA opna, UINT nAddress, REG8 cData)
{
	const UINT8 cCaps = opna->cCaps;
	CExternalOpna* pExt = CExternalOpna::GetInstance();

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
			if (pExt->HasADPCM())
			{
				pExt->WriteRegister(nAddress + 0x100, cData);
			}
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
			if (!pExt->IsEnabled())
			{
				opngen_setreg(3, nAddress, cData);
			}
			else
			{
				pExt->WriteRegister(nAddress + 0x100, cData);
			}
		}
	}
}

/**
 * Writes 3438 register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
void opna_write3438Register(POPNA opna, UINT nAddress, REG8 cData)
{
	REG8 cChannel;

	if (opna->cCaps & OPNA_HAS_YM3438)
	{
		opna->reg[nAddress + 0x200] = cData;

		if (nAddress < 0x30)
		{
			if (nAddress == 0x28)
			{
				cChannel = cData & 0x0f;
				if (cChannel < 3)
				{
					opngen_keyon(cChannel + 6, cData);
				}
				else if ((cChannel >= 4) && (cChannel < 7))
				{
					opngen_keyon(cChannel + 5, cData);
				}
			}
			else
			{
				if (nAddress == 0x27)
				{
					opnch[8].extop = cData & 0xc0;
				}
			}
		}
		else if (nAddress < 0xc0)
		{
			opngen_setreg(6, nAddress, cData);
		}
	}
}

/**
 * Writes 3438 extened register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @param[in] cData The data
 */
void opna_write3438ExtRegister(POPNA opna, UINT nAddress, REG8 cData)
{
	if (opna->cCaps & OPNA_HAS_YM3438)
	{
		opna->reg[nAddress + 0x300] = cData;
		opngen_setreg(9, nAddress, cData);
	}
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
 * Reads extended register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @return data
 */
REG8 opna_readExtendedRegister(POPNA opna, UINT nAddress)
{
	if ((opna->cCaps & OPNA_HAS_ADPCM) && (nAddress == 0x08))
	{
		return adpcm_readsample(&g_adpcm);
	}
	return opna->reg[nAddress + 0x100];
}

/**
 * Reads 3438 register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @return data
 */
REG8 opna_read3438Register(POPNA opna, UINT nAddress)
{
	if (opna->cCaps & OPNA_HAS_YM3438)
	{
		if (nAddress == 0xff)
		{
			return 0;
		}
		else if (nAddress >= 0x20)
		{
			return opna->reg[nAddress + 0x200];
		}
	}
	return 0xff;
}

/**
 * Reads 3438 extended register
 * @param[in] opna The instance
 * @param[in] nAddress The address
 * @return data
 */
REG8 opna_read3438ExtRegister(POPNA opna, UINT nAddress)
{
	if (opna->cCaps & OPNA_HAS_YM3438)
	{
		return opna->reg[nAddress + 0x200];
	}
	else
	{
		return 0xff;
	}
}
