#include	"compiler.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"gdc_pset.h"
#include	"vram.h"


static void MEMCALL _nop(GDCPSET pset, UINT addr, UINT bit) {

	pset->pattern = (pset->pattern << 1) + (pset->pattern >> 15);
	(void)addr;
	(void)bit;
}

static void MEMCALL _replace(GDCPSET pset, UINT addr, UINT bit) {

	vramupdate[addr] |= pset->update;
	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		pset->base[addr] |= (0x80 >> bit);
	}
	else {
		pset->pattern <<= 1;
		pset->base[addr] &= ~(0x80 >> bit);
	}
}

static void MEMCALL _complemnt(GDCPSET pset, UINT addr, UINT bit) {

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		vramupdate[addr] |= pset->update;
		pset->base[addr] ^= (0x80 >> bit);
	}
	else {
		pset->pattern <<= 1;
	}
}

static void MEMCALL _clear(GDCPSET pset, UINT addr, UINT bit) {

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		vramupdate[addr] |= pset->update;
		pset->base[addr] &= ~(0x80 >> bit);
	}
	else {
		pset->pattern <<= 1;
	}
}

static void MEMCALL _set(GDCPSET pset, UINT addr, UINT bit) {

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		vramupdate[addr] |= pset->update;
		pset->base[addr] |= (0x80 >> bit);
	}
	else {
		pset->pattern <<= 1;
	}
}

static void MEMCALL _tdw(GDCPSET pset, UINT addr, UINT bit) {

	BYTE	*ptr;

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		vramupdate[addr] |= pset->update;
		ptr = pset->base + addr;
		ptr[VRAM_B] = grcg.tile[0].b[0];
		ptr[VRAM_R] = grcg.tile[1].b[0];
		ptr[VRAM_G] = grcg.tile[2].b[0];
		ptr[VRAM_E] = grcg.tile[3].b[0];
	}
	else {
		pset->pattern <<= 1;
	}
	(void)bit;
}

static void MEMCALL _rmw(GDCPSET pset, UINT addr, UINT bit) {

	BYTE	*ptr;
	BYTE	data;
	BYTE	mask;

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		vramupdate[addr] |= pset->update;
		ptr = pset->base + addr;
		data = (0x80 >> bit);
		mask = ~bit;
		ptr[VRAM_B] &= mask;
		ptr[VRAM_B] |= bit & grcg.tile[0].b[0];
		ptr[VRAM_R] &= mask;
		ptr[VRAM_R] |= bit & grcg.tile[1].b[0];
		ptr[VRAM_G] &= mask;
		ptr[VRAM_G] |= bit & grcg.tile[2].b[0];
		ptr[VRAM_E] &= mask;
		ptr[VRAM_E] |= bit & grcg.tile[3].b[0];
	}
	else {
		pset->pattern <<= 1;
	}
}

static const GDCPSFN psettbl[] = {
		_replace,	_complemnt,	_clear,		_set,
		_replace,	_complemnt,	_clear,		_set,
		_tdw,		_tdw,		_nop,		_tdw,
		_rmw,		_rmw,		_nop,		_rmw};


// ----

void MEMCALL gdcpset_prepare(GDCPSET pset, UINT32 csrw, UINT16 pat, BYTE op) {

	BYTE	*base;
	BYTE	update;

	base = mem;
	if (!gdcs.access) {
		update = 1;
	}
	else {
		base += VRAM_STEP;
		update = 2;
	}
	op &= 3;
	if (!(grcg.gdcwithgrcg & 0x8)) {
		base += vramplaneseg[(csrw >> 14) & 3];
	}
	else {
		op += grcg.gdcwithgrcg;
	}
	gdcs.grphdisp |= update;
	pset->func = psettbl[op];
	pset->base = base;
	pset->pattern = pat;
	pset->update = update;
	pset->x = (UINT16)((((csrw & 0x3fff) % 40) << 4) + ((csrw >> 20) & 0x0f));
	pset->y = (UINT16)((csrw & 0x3fff) / 40);
	pset->dots = 0;
}

void MEMCALL gdcpset(GDCPSET pset, UINT16 x, UINT16 y) {

	pset->dots++;
	if (y > 409) {
		goto nopset;
	}
	else if (y == 409) {
		if (x >= 384) {
			goto nopset;
		}
	}
	else {
		if (x >= 640) {
			goto nopset;
		}
	}
	(*pset->func)(pset, (y * 80) + (x >> 3), x & 7);
	return;

nopset:
	pset->pattern = (pset->pattern << 1) + (pset->pattern >> 15);
}

