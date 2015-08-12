#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board118.h"
#include	"cs4231io.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"
#include "sound/soundrom.h"
#include	"keydisp.h"
#include "ext\externalopna.h"

#if !defined(SUPPORT_ROMEO)
#error Not support ROMEO
#endif

// ROMEO‘Î‰ž”Å PC-9801-118

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
	g_opn.reg[addr] = dat;
	if (addr < 0x10) {
		psggen_setreg(&g_psg1, addr, dat);
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
			return g_opn.reg[addr];
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

static void RestoreRomeo()
{
	UINT8 data[0x200];
	CopyMemory(data, g_opn.reg, 0x200);
	CopyMemory(data, &g_psg1.reg, 14);
	CExternalOpna::GetInstance()->Restore(data, true);
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
	g_opn.reg[nAddr] = dat;
	if (nAddr < 0x10)
	{
		(reinterpret_cast<UINT8*>(&g_psg1.reg))[nAddr] = dat;
		if (nAddr < 0x0e)
		{
			CExternalOpna::GetInstance()->WriteRegister(nAddr, dat);
			keydisp_psg(&g_psg1, nAddr);
		}
	}
	else
	{
		if (nAddr < 0x20)
		{
			CExternalOpna::GetInstance()->WriteRegister(nAddr, dat);
		}
		else if (nAddr == 0x28)
		{
			CExternalOpna::GetInstance()->WriteRegister(nAddr, dat);
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
			if ((nAddr == 0x22) || (nAddr == 0x27))
			{
				CExternalOpna::GetInstance()->WriteRegister(nAddr, dat);
			}
			fmtimer_setreg(nAddr, dat);
		}
		else if (nAddr < 0xc0)
		{
			CExternalOpna::GetInstance()->WriteRegister(nAddr, dat);
		}
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
		CExternalOpna::GetInstance()->WriteRegister(0x100 + nAddr, dat);
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

	CExternalOpna::GetInstance()->Reset();
}

void board118_bind(void)
{
	CExternalOpna* pExternalOpna = CExternalOpna::GetInstance();
	if (pExternalOpna->IsEnabled())
	{
		pExternalOpna->WriteRegister(0x22, 0x00);
		pExternalOpna->WriteRegister(0x29, 0x80);
		pExternalOpna->WriteRegister(0x10, 0xbf);
		pExternalOpna->WriteRegister(0x11, 0x30);
		Sleep(100);

		RestoreRomeo();

		cbuscore_attachsndex(0x188, ymfr_o, ymf_i);
	}
	else
	{
		fmboard_fmrestore(&g_opn, 0, 0);
		fmboard_fmrestore(&g_opn, 3, 1);
		fmboard_psgrestore(&g_opn, &g_psg1, 0);
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

