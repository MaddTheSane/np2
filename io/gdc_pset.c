#include	"compiler.h"
#include	"cpucore.h"
#include	"egcmem.h"
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

	vramupdate[addr] |= pset->update.b[0];
	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		pset->base.ptr[addr] |= (0x80 >> bit);
	}
	else {
		pset->pattern <<= 1;
		pset->base.ptr[addr] &= ~(0x80 >> bit);
	}
}

static void MEMCALL _complemnt(GDCPSET pset, UINT addr, UINT bit) {

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		vramupdate[addr] |= pset->update.b[0];
		pset->base.ptr[addr] ^= (0x80 >> bit);
	}
	else {
		pset->pattern <<= 1;
	}
}

static void MEMCALL _clear(GDCPSET pset, UINT addr, UINT bit) {

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		vramupdate[addr] |= pset->update.b[0];
		pset->base.ptr[addr] &= ~(0x80 >> bit);
	}
	else {
		pset->pattern <<= 1;
	}
}

static void MEMCALL _set(GDCPSET pset, UINT addr, UINT bit) {

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		vramupdate[addr] |= pset->update.b[0];
		pset->base.ptr[addr] |= (0x80 >> bit);
	}
	else {
		pset->pattern <<= 1;
	}
}


// ---- grcg

static void MEMCALL withtdw(GDCPSET pset, UINT addr, UINT bit) {

	BYTE	*ptr;

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		addr &= ~1;

		*(UINT16 *)(vramupdate + addr) |= pset->update.w;
		ptr = pset->base.ptr + addr;
		*(UINT16 *)(ptr + VRAM_B) = grcg.tile[0].w;
		*(UINT16 *)(ptr + VRAM_R) = grcg.tile[1].w;
		*(UINT16 *)(ptr + VRAM_G) = grcg.tile[2].w;
		*(UINT16 *)(ptr + VRAM_E) = grcg.tile[3].w;
	}
	else {
		pset->pattern <<= 1;
	}
	(void)bit;
}

static void MEMCALL withrmw(GDCPSET pset, UINT addr, UINT bit) {

	BYTE	*ptr;
	BYTE	data;
	BYTE	mask;

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		vramupdate[addr] |= pset->update.b[0];
		ptr = pset->base.ptr + addr;
		data = (0x80 >> bit);
		mask = ~data;
		ptr[VRAM_B] &= mask;
		ptr[VRAM_B] |= data & grcg.tile[0].b[0];
		ptr[VRAM_R] &= mask;
		ptr[VRAM_R] |= data & grcg.tile[1].b[0];
		ptr[VRAM_G] &= mask;
		ptr[VRAM_G] |= data & grcg.tile[2].b[0];
		ptr[VRAM_E] &= mask;
		ptr[VRAM_E] |= data & grcg.tile[3].b[0];
	}
	else {
		pset->pattern <<= 1;
	}
}


// ---- egc

static void MEMCALL withegc(GDCPSET pset, UINT addr, UINT bit) {

	REG16	data;

	if (pset->pattern & 0x8000) {
		pset->pattern <<= 1;
		pset->pattern++;
		data = (0x80 >> bit);
		if (addr & 1) {
			addr &= ~1;
			data <<= 8;
		}
		egc_write_w(pset->base.addr + addr, data);
	}
	else {
		pset->pattern <<= 1;
	}
}


static const GDCPSFN psettbl[] = {_replace, _complemnt, _clear, _set};


// ----

void MEMCALL gdcpset_prepare(GDCPSET pset, UINT32 csrw, REG16 pat, REG8 op) {

	BYTE	*base;
	BYTE	update;

	if (vramop.operate & VOP_EGCBIT) {
		pset->func = withegc;
		pset->base.addr = vramplaneseg[(csrw >> 14) & 3];
	}
	else {
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
			pset->func = psettbl[op];
			pset->base.ptr = base + vramplaneseg[(csrw >> 14) & 3];
		}
		else {
			pset->func = (grcg.gdcwithgrcg & 0x4)?withrmw:withtdw;
			pset->base.ptr = base;
		}
		gdcs.grphdisp |= update;
		pset->update.b[0] = update;
		pset->update.b[1] = update;
	}
	pset->pattern = pat;
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

