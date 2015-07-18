#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board118.h"
#include	"cs4231io.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"
#include	"keydisp.h"
#include "gimic/gimic.h"
#include "spfm/spfmlight.h"

/**! The instance of external modules */
static IExtendModule* s_ext = NULL;

static void IOOUTCALL ymf_o188(UINT port, REG8 dat) {

	g_opn.addr1l = dat;
	g_opn.addr1h = 0;
	g_opn.data1 = dat;
	(void)port;
}

static void IOOUTCALL ymf_o18a(UINT port, REG8 dat) {

	UINT	addr;

	g_opn.data1 = dat;
	if (g_opn.addr1h != 0) {
		return;
	}

	addr = g_opn.addr1l;
	S98_put(NORMAL2608, addr, dat);
	if (addr < 0x10) {
		if (addr != 0x0e) {
			psggen_setreg(&g_psg1, addr, dat);
		}
	}
	else {
		if (addr < 0x20) {
			rhythm_setreg(&g_rhythm, addr, dat);
		}
		else if (addr < 0x30) {
			if (addr == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon(dat & 0x0f, dat);
				}
				else if (((dat & 0x0f) != 3) &&
						((dat & 0x0f) < 7)) {
					opngen_keyon((dat & 0x07) - 1, dat);
				}
			}
			else {
				fmtimer_setreg(addr, dat);
			}
		}
		else if (addr < 0xc0) {
			opngen_setreg(0, addr, dat);
		}
		g_opn.reg[addr] = dat;
	}
	(void)port;
}

static void IOOUTCALL ymf_o18c(UINT port, REG8 dat) {

	if (g_opn.extend) {
		g_opn.addr1l = dat;
		g_opn.addr1h = 1;
		g_opn.data1 = dat;
	}
	(void)port;
}

static void IOOUTCALL ymf_o18e(UINT port, REG8 dat) {

	UINT	addr;

	if (!g_opn.extend) {
		return;
	}
	g_opn.data1 = dat;

	if (g_opn.addr1h != 1) {
		return;
	}
	addr = g_opn.addr1l;
	S98_put(EXTEND2608, addr, dat);
	g_opn.reg[addr + 0x100] = dat;
	if (addr >= 0x30) {
		opngen_setreg(3, addr, dat);
	}
	else {
		if (addr == 0x10) {
			if (!(dat & 0x80)) {
				g_opn.adpcmmask = ~(dat & 0x1c);
			}
		}
	}
	(void)port;
}

static REG8 IOINPCALL ymf_i188(UINT port) {

	(void)port;
	return(g_fmtimer.status);
}

static REG8 IOINPCALL ymf_i18a(UINT port) {

	UINT	addr;

	if (g_opn.addr1h == 0) {
		addr = g_opn.addr1l;
		if (addr == 0x0e) {
			return(fmboard_getjoy(&g_psg1));
		}
		else if (addr < 0x10) {
			return(psggen_getreg(&g_psg1, addr));
		}
		else if (addr == 0xff) {
			return(1);
		}
	}
	(void)port;
	return(g_opn.data1);
}

static REG8 IOINPCALL ymf_i18c(UINT port) {

	if (g_opn.extend) {
		return(g_fmtimer.status & 3);
	}
	(void)port;
	return(0xff);
}

static void extendchannel(REG8 enable) {

	g_opn.extend = enable;
	if (enable) {
		g_opn.channels = 6;
		opngen_setcfg(6, OPN_STEREO | 0x007);
	}
	else {
		g_opn.channels = 3;
		opngen_setcfg(3, OPN_MONORAL | 0x007);
		rhythm_setreg(&g_rhythm, 0x10, 0xff);
	}
}

static void IOOUTCALL ymf_oa460(UINT port, REG8 dat) {

	cs4231.extfunc = dat;
	extendchannel((REG8)(dat & 1));
	(void)port;
}

static REG8 IOINPCALL ymf_ia460(UINT port) {

	(void)port;
	return(0x80 | (cs4231.extfunc & 1));
}


// ---- with romeo

/**
 * Restore OPNA
 * @param[in] ext instance
 */
static void RestoreRomeo(IExtendModule* ext)
{
	const UINT8* data = g_opn.reg;
	for (UINT i = 0x30; i < 0xa0; i++)
	{
		ext->WriteRegister(i, data[i]);
	}
	for (UINT ch = 0; ch < 3; ch++)
	{
		ext->WriteRegister(ch + 0xa4, data[ch + 0x0a4]);
		ext->WriteRegister(ch + 0xa0, data[ch + 0x0a0]);
		ext->WriteRegister(ch + 0xb0, data[ch + 0x0b0]);
		ext->WriteRegister(ch + 0xb4, data[ch + 0x0b4]);
	}

	for (UINT i = 0x130; i < 0x1a0; i++)
	{
		ext->WriteRegister(i, data[i]);
	}
	for (UINT ch = 0; ch < 3; ch++)
	{
		ext->WriteRegister(ch + 0x1a4, data[ch + 0x1a4]);
		ext->WriteRegister(ch + 0x1a0, data[ch + 0x1a0]);
		ext->WriteRegister(ch + 0x1b0, data[ch + 0x1b0]);
		ext->WriteRegister(ch + 0x1b4, data[ch + 0x1b4]);
	}
	ext->WriteRegister(0x11, data[0x11]);
	ext->WriteRegister(0x18, data[0x18]);
	ext->WriteRegister(0x19, data[0x19]);
	ext->WriteRegister(0x1a, data[0x1a]);
	ext->WriteRegister(0x1b, data[0x1b]);
	ext->WriteRegister(0x1c, data[0x1c]);
	ext->WriteRegister(0x1d, data[0x1d]);

	const UINT8* psg = reinterpret_cast<UINT8*>(&g_psg1.reg);
	for (UINT i = 0; i < 0x0e; i++)
	{
		ext->WriteRegister(i, psg[i]);
	}
}

static void IOOUTCALL ymfr_o18a(UINT port, REG8 dat)
{
	g_opn.data1 = dat;
	if (g_opn.addr1h != 0)
	{
		return;
	}

	const UINT nAddr = g_opn.addr1l;
	S98_put(NORMAL2608, nAddr, dat);

	if (nAddr < 0x10)
	{
		(reinterpret_cast<UINT8*>(&g_psg1.reg))[nAddr] = dat;
		if (nAddr < 0x0e)
		{
			s_ext->WriteRegister(nAddr, dat);

			if (nAddr == 0x07)
			{
				keydisp_psgmix(&g_psg1);
			}
			else if ((nAddr == 0x08) || (nAddr == 0x09) || (nAddr == 0x0a))
			{
				keydisp_psgvol(&g_psg1, static_cast<UINT8>(nAddr - 8));
			}
		}
	}
	else
	{
		if (nAddr < 0x20)
		{
			s_ext->WriteRegister(nAddr, dat);
		}
		else if (nAddr == 0x28)
		{
			s_ext->WriteRegister(nAddr, dat);
			if ((dat & 0x0f) < 3)
			{
				keydisp_fmkeyon(static_cast<UINT8>(dat & 0x0f), dat);
			}
			else if (((dat & 0x0f) != 3) && ((dat & 0x0f) < 7))
			{
				keydisp_fmkeyon(static_cast<UINT8>((dat & 0x0f) - 1), dat);
			}
		}
		else if (nAddr < 0x30)
		{
			fmtimer_setreg(nAddr, dat);
			if ((nAddr == 0x22) || (nAddr == 0x27))
			{
				s_ext->WriteRegister(nAddr, dat);
			}
		}
		else if (nAddr < 0xc0)
		{
			s_ext->WriteRegister(nAddr, dat);
		}
		g_opn.reg[nAddr] = dat;
	}
	(void)port;
}

static void IOOUTCALL ymfr_o18e(UINT port, REG8 dat)
{
	if (!g_opn.extend)
	{
		return;
	}
	g_opn.data1 = dat;
	if (g_opn.addr1h != 1) {
		return;
	}

	const UINT nAddr = g_opn.addr1l;
	S98_put(EXTEND2608, nAddr, dat);
	g_opn.reg[nAddr + 0x100] = dat;
	if (nAddr >= 0x30)
	{
		s_ext->WriteRegister(0x100 + nAddr, dat);
	}
	else if (nAddr == 0x10)
	{
		if (!(dat & 0x80))
		{
			g_opn.adpcmmask = ~(dat & 0x1c);
		}
	}
	(void)port;
}


// ----

static const IOOUT ymf_o[4] = {
			ymf_o188,	ymf_o18a,	ymf_o18c,	ymf_o18e};

static const IOINP ymf_i[4] = {
			ymf_i188,	ymf_i18a,	ymf_i18c,	NULL};

static const IOOUT ymfr_o[4] =
{
	ymf_o188,	ymfr_o18a,	ymf_o18c,	ymfr_o18e
};

void board118_reset(const NP2CFG *pConfig) {

	fmtimer_reset(0xc0);
	opngen_setcfg(3, OPN_STEREO | 0x038);
	cs4231io_reset();
	soundrom_load(0xcc000, OEMTEXT("118"));
	fmboard_extreg(extendchannel);

	IExtendModule* ext = s_ext;
	if (ext != NULL)
	{
		ext->Reset();
	}
}

void board118_bind(void)
{
	IExtendModule* ext = s_ext;

	if (ext == NULL)
	{
		// G.I.M.I.C オープン
		ext = new CGimic();
		if (ext->Initialize())
		{
			ext->Reset();
		}
		else
		{
			delete ext;
			ext = NULL;
		}
	}
	if (ext == NULL)
	{
		// SPFM Lightオープン
		ext = new CSpfmLight();
		if (ext->Initialize())
		{
			ext->Reset();
		}
		else
		{
			delete ext;
			ext = NULL;
		}
	}
	s_ext = ext;

	if (ext)
	{
		ext->WriteRegister(0x22, 0x00);
		ext->WriteRegister(0x29, 0x80);
		ext->WriteRegister(0x10, 0xbf);
		ext->WriteRegister(0x11, 0x30);

		RestoreRomeo(ext);

		cbuscore_attachsndex(0x188, ymfr_o, ymf_i);
	}
	else
	{
		fmboard_fmrestore(&g_opn, 0, 0);
		fmboard_fmrestore(&g_opn, 3, 1);
		psggen_restore(&g_psg1);
		fmboard_rhyrestore(&g_opn, &g_rhythm, 0);
		sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
		sound_streamregist(&g_psg1, (SOUNDCB)psggen_getpcm);
		rhythm_bind(&g_rhythm);
		cbuscore_attachsndex(0x188, ymf_o, ymf_i);
	}
	cs4231io_bind();
	iocore_attachout(0xa460, ymf_oa460);
	iocore_attachinp(0xa460, ymf_ia460);
}

/**
 * Deinitialize
 */
extern "C" void board118_deinitialize(void)
{
	IExtendModule* ext = s_ext;
	s_ext = NULL;

	if (ext)
	{
		ext->Reset();
		ext->Deinitialize();
		delete ext;
	}
}
