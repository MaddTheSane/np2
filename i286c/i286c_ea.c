#include	"compiler.h"
#include	"i286.h"
#include	"i286c.h"
#include	"i286c.mcr"
#include	"memory.h"


// ---------------------------------------------------------------------------

static UINT32 ea_bx_si(void) {

	return(((I286_BX + I286_SI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_bx_si_disp8(void) {

	SINT32	adrs;

	GET_PCBYTESD(adrs);
	return(((adrs + I286_BX + I286_SI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_bx_si_disp16(void) {

	UINT32	adrs;

	GET_PCWORD(adrs);
	return(((adrs + I286_BX + I286_SI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_bx_di(void) {

	return(((I286_BX + I286_DI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_bx_di_disp8(void) {

	SINT32	adrs;

	GET_PCBYTESD(adrs);
	return(((adrs + I286_BX + I286_DI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_bx_di_disp16(void) {

	UINT32	adrs;

	GET_PCWORD(adrs);
	return(((adrs + I286_BX + I286_DI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_bp_si(void) {

	return(((I286_BP + I286_SI) & 0x0000ffff) + SS_FIX);
}

static UINT32 ea_bp_si_disp8(void) {

	SINT32	adrs;

	GET_PCBYTESD(adrs);
	return(((adrs + I286_BP + I286_SI) & 0x0000ffff) + SS_FIX);
}

static UINT32 ea_bp_si_disp16(void) {

	UINT32	adrs;

	GET_PCWORD(adrs);
	return(((adrs + I286_BP + I286_SI) & 0x0000ffff) + SS_FIX);
}

static UINT32 ea_bp_di(void) {

	return(((I286_BP + I286_DI) & 0x0000ffff) + SS_FIX);
}

static UINT32 ea_bp_di_disp8(void) {

	SINT32	adrs;

	GET_PCBYTESD(adrs);
	return(((adrs + I286_BP + I286_DI) & 0x0000ffff) + SS_FIX);
}

static UINT32 ea_bp_di_disp16(void) {

	UINT32	adrs;

	GET_PCWORD(adrs);
	return(((adrs + I286_BP + I286_DI) & 0x0000ffff) + SS_FIX);
}

static UINT32 ea_si(void) {

	return(I286_SI + DS_FIX);
}

static UINT32 ea_si_disp8(void) {

	SINT32	adrs;

	GET_PCBYTESD(adrs);
	return(((adrs + I286_SI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_si_disp16(void) {

	UINT32	adrs;

	GET_PCWORD(adrs);
	return(((adrs + I286_SI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_di(void) {

	return(I286_DI + DS_FIX);
}

static UINT32 ea_di_disp8(void) {

	SINT32	adrs;

	GET_PCBYTESD(adrs);
	return(((adrs + I286_DI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_di_disp16(void) {

	UINT32	adrs;

	GET_PCWORD(adrs);
	return(((adrs + I286_DI) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_disp16(void) {

	UINT32	adrs;

	GET_PCWORD(adrs);
	return((adrs & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_bp_disp8(void) {

	SINT32	adrs;

	GET_PCBYTESD(adrs);
	return(((adrs + I286_BP) & 0x0000ffff) + SS_FIX);
}

static UINT32 ea_bp_disp16(void) {

	UINT32	adrs;

	GET_PCWORD(adrs);
	return(((adrs + I286_BP) & 0x0000ffff) + SS_FIX);
}

static UINT32 ea_bx(void) {

	return(I286_BX + DS_FIX);
}

static UINT32 ea_bx_disp8(void) {

	SINT32	adrs;

	GET_PCBYTESD(adrs);
	return(((adrs + I286_BX) & 0x0000ffff) + DS_FIX);
}

static UINT32 ea_bx_disp16(void) {

	UINT32	adrs;

	GET_PCWORD(adrs);
	return(((adrs + I286_BX) & 0x0000ffff) + DS_FIX);
}

const CALCEA i286c_ea_dst_tbl[] = {
			ea_bx_si,			ea_bx_di,
			ea_bp_si,			ea_bp_di,
			ea_si,				ea_di,
			ea_disp16,			ea_bx,
			ea_bx_si_disp8,		ea_bx_di_disp8,
			ea_bp_si_disp8,		ea_bp_di_disp8,
			ea_si_disp8,		ea_di_disp8,
			ea_bp_disp8,		ea_bx_disp8,
			ea_bx_si_disp16,	ea_bx_di_disp16,
			ea_bp_si_disp16,	ea_bp_di_disp16,
			ea_si_disp16,		ea_di_disp16,
			ea_bp_disp16,		ea_bx_disp16};


// --------------------------------------------------------------------------

static UINT16 lea_bx_si(void) {

	return(I286_BX + I286_SI);
}

static UINT16 lea_bx_si_disp8(void) {

	UINT16	adrs;

	GET_PCBYTES(adrs);
	return(adrs + I286_BX + I286_SI);
}

static UINT16 lea_bx_si_disp16(void) {

	UINT16	adrs;

	GET_PCWORD(adrs);
	return(adrs + I286_BX + I286_SI);
}

static UINT16 lea_bx_di(void) {

	return(I286_BX + I286_DI);
}

static UINT16 lea_bx_di_disp8(void) {

	UINT16	adrs;

	GET_PCBYTES(adrs);
	return(adrs + I286_BX + I286_DI);
}

static UINT16 lea_bx_di_disp16(void) {

	UINT16	adrs;

	GET_PCWORD(adrs);
	return(adrs + I286_BX + I286_DI);
}

static UINT16 lea_bp_si(void) {

	return(I286_BP + I286_SI);
}

static UINT16 lea_bp_si_disp8(void) {

	UINT16	adrs;

	GET_PCBYTES(adrs);
	return(adrs + I286_BP + I286_SI);
}

static UINT16 lea_bp_si_disp16(void) {

	UINT16	adrs;

	GET_PCWORD(adrs);
	return(adrs + I286_BP + I286_SI);
}

static UINT16 lea_bp_di(void) {

	return(I286_BP + I286_DI);
}

static UINT16 lea_bp_di_disp8(void) {

	UINT16	adrs;

	GET_PCBYTES(adrs);
	return(adrs + I286_BP + I286_DI);
}

static UINT16 lea_bp_di_disp16(void) {

	UINT16	adrs;

	GET_PCWORD(adrs);
	return(adrs + I286_BP + I286_DI);
}

static UINT16 lea_si(void) {

	return(I286_SI);
}

static UINT16 lea_si_disp8(void) {

	UINT16	adrs;

	GET_PCBYTES(adrs);
	return(adrs + I286_SI);
}

static UINT16 lea_si_disp16(void) {

	UINT16	adrs;

	GET_PCWORD(adrs);
	return(adrs + I286_SI);
}

static UINT16 lea_di(void) {

	return(I286_DI);
}

static UINT16 lea_di_disp8(void) {

	UINT16	adrs;

	GET_PCBYTES(adrs);
	return(adrs + I286_DI);
}

static UINT16 lea_di_disp16(void) {

	UINT16	adrs;

	GET_PCWORD(adrs);
	return(adrs + I286_DI);
}

static UINT16 lea_disp16(void) {

	UINT16	adrs;

	GET_PCWORD(adrs);
	return(adrs);
}

static UINT16 lea_bp_disp8(void) {

	UINT16	adrs;

	GET_PCBYTES(adrs);
	return(adrs + I286_BP);
}

static UINT16 lea_bp_disp16(void) {

	UINT16	adrs;

	GET_PCWORD(adrs);
	return(adrs + I286_BP);
}

static UINT16 lea_bx(void) {

	return(I286_BX);
}

static UINT16 lea_bx_disp8(void) {

	UINT16	adrs;

	GET_PCBYTES(adrs);
	return(adrs + I286_BX);
}

static UINT16 lea_bx_disp16(void) {

	UINT16	adrs;

	GET_PCWORD(adrs);
	return(adrs + I286_BX);
}

const CALCLEA i286c_lea_tbl[] = {
			lea_bx_si,			lea_bx_di,
			lea_bp_si,			lea_bp_di,
			lea_si,				lea_di,
			lea_disp16,			lea_bx,
			lea_bx_si_disp8,	lea_bx_di_disp8,
			lea_bp_si_disp8,	lea_bp_di_disp8,
			lea_si_disp8,		lea_di_disp8,
			lea_bp_disp8,		lea_bx_disp8,
			lea_bx_si_disp16,	lea_bx_di_disp16,
			lea_bp_si_disp16,	lea_bp_di_disp16,
			lea_si_disp16,		lea_di_disp16,
			lea_bp_disp16,		lea_bx_disp16};

// --------------------------------------------------------------------------

static UINT16 a_bx_si(void) {

	EA_FIX = DS_FIX;
	return(I286_BX + I286_SI);
}

static UINT16 a_bx_si_disp8(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCBYTES(adrs);
	return(adrs + I286_BX + I286_SI);
}

static UINT16 a_bx_si_disp16(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCWORD(adrs);
	return(adrs + I286_BX + I286_SI);
}

static UINT16 a_bx_di(void) {

	EA_FIX = DS_FIX;
	return(I286_BX + I286_DI);
}

static UINT16 a_bx_di_disp8(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCBYTES(adrs);
	return(adrs + I286_BX + I286_DI);
}

static UINT16 a_bx_di_disp16(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCWORD(adrs);
	return(adrs + I286_BX + I286_DI);
}

static UINT16 a_bp_si(void) {

	EA_FIX = SS_FIX;
	return(I286_BP + I286_SI);
}

static UINT16 a_bp_si_disp8(void) {

	UINT16	adrs;

	EA_FIX = SS_FIX;
	GET_PCBYTES(adrs);
	return(adrs + I286_BP + I286_SI);
}

static UINT16 a_bp_si_disp16(void) {

	UINT16	adrs;

	EA_FIX = SS_FIX;
	GET_PCWORD(adrs);
	return(adrs + I286_BP + I286_SI);
}

static UINT16 a_bp_di(void) {

	EA_FIX = SS_FIX;
	return(I286_BP + I286_DI);
}

static UINT16 a_bp_di_disp8(void) {

	UINT16	adrs;

	EA_FIX = SS_FIX;
	GET_PCBYTES(adrs);
	return(adrs + I286_BP + I286_DI);
}

static UINT16 a_bp_di_disp16(void) {

	UINT16	adrs;

	EA_FIX = SS_FIX;
	GET_PCWORD(adrs);
	return(adrs + I286_BP + I286_DI);
}

static UINT16 a_si(void) {

	EA_FIX = DS_FIX;
	return(I286_SI);
}

static UINT16 a_si_disp8(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCBYTES(adrs);
	return(adrs + I286_SI);
}

static UINT16 a_si_disp16(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCWORD(adrs);
	return(adrs + I286_SI);
}

static UINT16 a_di(void) {

	EA_FIX = DS_FIX;
	return(I286_DI);
}

static UINT16 a_di_disp8(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCBYTES(adrs);
	return(adrs + I286_DI);
}

static UINT16 a_di_disp16(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCWORD(adrs);
	return(adrs + I286_DI);
}

static UINT16 a_disp16(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCWORD(adrs);
	return(adrs);
}

static UINT16 a_bp_disp8(void) {

	UINT16	adrs;

	EA_FIX = SS_FIX;
	GET_PCBYTES(adrs);
	return(adrs + I286_BP);
}

static UINT16 a_bp_disp16(void) {

	UINT16	adrs;

	EA_FIX = SS_FIX;
	GET_PCWORD(adrs);
	return(adrs + I286_BP);
}

static UINT16 a_bx(void) {

	EA_FIX = DS_FIX;
	return(I286_BX);
}

static UINT16 a_bx_disp8(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCBYTES(adrs);
	return(adrs + I286_BX);
}

static UINT16 a_bx_disp16(void) {

	UINT16	adrs;

	EA_FIX = DS_FIX;
	GET_PCWORD(adrs);
	return(adrs + I286_BX);
}

const GETLEA i286c_ea_tbl[] = {
			a_bx_si,			a_bx_di,
			a_bp_si,			a_bp_di,
			a_si,				a_di,
			a_disp16,			a_bx,
			a_bx_si_disp8,		a_bx_di_disp8,
			a_bp_si_disp8,		a_bp_di_disp8,
			a_si_disp8,			a_di_disp8,
			a_bp_disp8,			a_bx_disp8,
			a_bx_si_disp16,		a_bx_di_disp16,
			a_bp_si_disp16,		a_bp_di_disp16,
			a_si_disp16,		a_di_disp16,
			a_bp_disp16,		a_bx_disp16};

