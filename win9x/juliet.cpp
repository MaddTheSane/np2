#include	<windows.h>
#include	"common.h"

#include	"romeo.h"
#include	"juliet.h"


enum {
	ROMEO_AVAIL			= 0x01,
	ROMEO_YMF288		= 0x02,			// •K‚¸‘¶Ý‚·‚é”¤H
	ROMEO_YM2151		= 0x04
};

typedef struct {
	HMODULE			mod;

	PCIFINDDEV		finddev;
	PCICFGREAD32	read32;
	PCIMEMWR8		out8;
	PCIMEMWR16		out16;
	PCIMEMWR32		out32;
	PCIMEMRD8		in8;
	PCIMEMRD16		in16;
	PCIMEMRD32		in32;

	ULONG			addr;
	ULONG			irq;
	ULONG			avail;
} _ROMEO;


#define	ROMEO_TPTR(member)	(int)&(((_ROMEO *)NULL)->member)


typedef struct {
	char	*symbol;
	int		addr;
} DLLPROCESS;


static const DLLPROCESS	dllproc[] = {
				{FN_PCIFINDDEV,		ROMEO_TPTR(finddev)},
				{FN_PCICFGREAD32,	ROMEO_TPTR(read32)},
				{FN_PCIMEMWR8,		ROMEO_TPTR(out8)},
				{FN_PCIMEMWR16,		ROMEO_TPTR(out16)},
				{FN_PCIMEMWR32,		ROMEO_TPTR(out32)},
				{FN_PCIMEMRD8,		ROMEO_TPTR(in8)},
				{FN_PCIMEMRD16,		ROMEO_TPTR(in16)},
				{FN_PCIMEMRD32,		ROMEO_TPTR(in32)}};


static	_ROMEO		romeo = {NULL};


BOOL juliet_load(void) {

		int			i;
const	DLLPROCESS	*dp;
		BOOL		r = SUCCESS;

	juliet_unload();

	romeo.mod = LoadLibrary(PCIDEBUG_DLL);
	if (romeo.mod == NULL) {
		return(FAILURE);
	}
	for (i=0, dp=dllproc; i<sizeof(dllproc)/sizeof(DLLPROCESS); i++, dp++) {
		FARPROC proc;
		proc = GetProcAddress(romeo.mod, dp->symbol);
		if (proc == NULL) {
			r = FAILURE;
			break;
		}
		*(DWORD *)(((BYTE *)&romeo) + (dp->addr)) = (DWORD)proc;
	}
	if (r) {
		juliet_unload();
	}
	return(r);
}


void juliet_unload(void) {

	if (romeo.mod) {
		FreeLibrary(romeo.mod);
	}
	ZeroMemory(&romeo, sizeof(romeo));
}


// ----

ULONG juliet_prepare(void) {

	ULONG	r;
	ULONG	pciaddr;

	if (romeo.mod == NULL) {
		return(FAILURE);
	}

	r = romeo.finddev(ROMEO_VENDORID, ROMEO_DEVICEID, 0);
	if (r & 0xff) {
		r = romeo.finddev(ROMEO_VENDORID, ROMEO_DEVICEID2, 0);
	}
	pciaddr = r >> 16;
	r &= 0xff;

	if (!r) {
		romeo.addr = romeo.read32(pciaddr, ROMEO_BASEADDRESS1);
		romeo.irq  = romeo.read32(pciaddr, ROMEO_PCIINTERRUPT) & 0xff;
		if (romeo.addr) {
			romeo.avail = ROMEO_AVAIL | ROMEO_YMF288;
			juliet_YM2151Reset();
			juliet_YMF288Reset();
		}
	}
	return(r);
}


// ---- YM2151•”

void juliet_YM2151Reset(void)
{
	BYTE	flag;

	if (romeo.avail & ROMEO_AVAIL) {
		romeo.out32(romeo.addr + ROMEO_YM2151CTRL, 0x00);
		Sleep(10);
		flag = romeo.in8(romeo.addr + ROMEO_YM2151DATA) + 1;
		romeo.out32(romeo.addr + ROMEO_YM2151CTRL, 0x80);
		Sleep(10);
		flag |= romeo.in8(romeo.addr + ROMEO_YM2151DATA);
		if (!flag) {
			romeo.avail |= ROMEO_YM2151;
		}
	}
}

int juliet_YM2151IsEnable(void)
{
	return (( romeo.avail&ROMEO_YM2151 )?TRUE:FALSE);
}

int juliet_YM2151IsBusy(void)
{

	return ((!( romeo.avail&ROMEO_YM2151 ))|| ( ( romeo.in8(romeo.addr + ROMEO_CMDQUEUE)&0x0f )!=0x0f ));
}

void juliet_YM2151W(BYTE reg, BYTE data)
{
	if ( romeo.avail&ROMEO_YM2151) {
		romeo.out8(romeo.addr + ROMEO_YM2151ADDR, reg);
		while ( (romeo.in8(romeo.addr + ROMEO_CMDQUEUE)&0x0f)!=0x0f ) {
			Sleep(0);
		}
		romeo.out8(romeo.addr + ROMEO_YM2151DATA, data);
	}
}


// ---- YMF288•”

void juliet_YMF288Reset(void) {

	if (romeo.avail & ROMEO_YMF288) {
		romeo.out32(romeo.addr + ROMEO_YMF288CTRL, 0x00);
		Sleep(150);
		romeo.out32(romeo.addr + ROMEO_YMF288CTRL, 0x80);
		Sleep(150);
	}
}

int juliet_YM288IsEnable(void) {

	return(TRUE);
}

int juliet_YM288IsBusy(void) {

	return((!(romeo.avail&ROMEO_YMF288)) ||
			((romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) != 0));
}

void juliet_YMF288A(BYTE addr, BYTE data) {

	if (romeo.avail & ROMEO_YMF288) {
		while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
			Sleep(0);
		}
		romeo.out8(romeo.addr + ROMEO_YMF288ADDR1, addr);
		while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
			Sleep(0);
		}
		romeo.out8(romeo.addr + ROMEO_YMF288DATA1, data);
		{
			char buf[128];
			wsprintf(buf, "A-%02x %02x\n", addr, data);
			OutputDebugString(buf);
		}
	}
}

void juliet_YMF288B(BYTE addr, BYTE data) {

	if (romeo.avail & ROMEO_YMF288) {
		while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
			Sleep(0);
		}
		romeo.out8(romeo.addr + ROMEO_YMF288ADDR2, addr);
		while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
			Sleep(0);
		}
		romeo.out8(romeo.addr + ROMEO_YMF288DATA2, data);
		{
			char buf[128];
			wsprintf(buf, "B-%02x %02x\n", addr, data);
			OutputDebugString(buf);
		}
	}
}

