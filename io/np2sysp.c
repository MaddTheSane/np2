#include	"compiler.h"
#include	"strres.h"
#include	"taskmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"sxsi.h"
#if defined(SUPPORT_HOSTDRV)
#include	"hostdrv.h"
#endif


#define		NP2SYSP_VER			"B"
// #define	NP2SYSP_CREDIT		""					// 要るなら・・・

// NP2依存ポート
// port:07edh	np2 value comm
// port:07efh	np2 string comm

// 基本的に STRINGでやり取りする
// ポート 7efh に 'NP2' と出力で "NP2"が返ってきたら NP2である

// verA
//		out->str: 'ver'				in->str:	ver番号 A～
//		out->str: 'poweroff'		NP2を終了

// verB
//		out->str: 'cpu'				in->str:	CPU型番
//		out->str: 'clock'			in->str:	動作クロック数


// ----

typedef struct {
const char	*key;
	void	(*func)(const void *arg1, long arg2);
const void	*arg1;
	long	arg2;
} SYSPCMD;

static const char str_80286[] = "80286";
static const char str_v30[] = "V30";
static const char str_pentium[] = "PENTIUM";
static const char str_mhz[] = "%uMHz";


static void cmd_outstr(const void *arg1, long arg2) {

	milstr_ncpy(np2sysp.outstr, (char *)arg1, sizeof(np2sysp.outstr));
	np2sysp.outpos = 0;
	(void)arg2;
}

static void cmd_poweroff(const void *arg1, long arg2) {

	taskmng_exit();
	(void)arg1;
	(void)arg2;
}

static void cmd_cpu(const void *arg1, long arg2) {

	// CPUを返す
#if 1											// 80286 or V30
	if (!(CPU_TYPE & CPUTYPE_V30)) {
		cmd_outstr(str_80286, 0);
	}
	else {
		cmd_outstr(str_v30, 0);
	}
#else
	// 386機以降の場合 V30モードはエミュレーションだから固定(?)
	cmd_outstr(str_pentium, 0);
#endif
	(void)arg1;
	(void)arg2;
}

static void cmd_clock(const void *arg1, long arg2) {

	SPRINTF(np2sysp.outstr, str_mhz, (pc.realclock + 500000) / 1000000);
	np2sysp.outpos = 0;
	(void)arg1;
	(void)arg2;
}

static void cmd_multiple(const void *arg1, long arg2) {

	SPRINTF(np2sysp.outstr, str_u, pc.multiple);
	np2sysp.outpos = 0;
	(void)arg1;
	(void)arg2;
}


// ---- SXSI BIOS

typedef struct {
	BYTE	r_ax[2];
	BYTE	r_bx[2];
	BYTE	r_cx[2];
	BYTE	r_dx[2];
	BYTE	r_bp[2];
	BYTE	r_es[2];
} B1BREG;

static void cmd_sxsibios(const void *arg1, long arg2) {

	UINT16	org_ax, org_cx, org_dx, org_bx, org_bp, org_es;
	UINT32	org_esbase;
	B1BREG	r;
	REG16	tmp;
	UINT8	ret;
	REG8	flag;

	org_ax = CPU_AX;
	org_cx = CPU_CX;
	org_dx = CPU_DX;
	org_bx = CPU_BX;
	org_bp = CPU_BP;
	org_es = CPU_ES;
	org_esbase = ES_BASE;

	i286_memstr_read(CPU_SS, CPU_SP, &r, sizeof(r));
	CPU_AX = LOADINTELWORD(r.r_ax);
	CPU_BX = LOADINTELWORD(r.r_bx);
	CPU_CX = LOADINTELWORD(r.r_cx);
	CPU_DX = LOADINTELWORD(r.r_dx);
	CPU_BP = LOADINTELWORD(r.r_bp);
	tmp = LOADINTELWORD(r.r_es);
	CPU_ES = tmp;
	ES_BASE = tmp << 4;
	switch(CPU_AX & 0xf0) {
		case 0x00:
		case 0x20:
			ret = sxsi_operate(HDDTYPE_SASI);
			break;

		case 0x80:
		case 0xa0:
			ret = sxsi_operate(HDDTYPE_SCSI);
			break;

		default:
			ret = 0x40;
			break;
	}
	r.r_ax[0] = CPU_AL;
	r.r_ax[1] = ret;
	STOREINTELWORD(r.r_bx, CPU_BX);
	STOREINTELWORD(r.r_cx, CPU_CX);
	STOREINTELWORD(r.r_dx, CPU_DX);
	i286_memstr_write(CPU_SS, CPU_SP, &r, 8);
	flag = i286_membyte_read(CPU_SS, CPU_SP + 0x16) & 0xfe;
	if (ret >= 0x20) {
		flag += 1;
	}
	i286_membyte_write(CPU_SS, CPU_SP + 0x16, flag);

	CPU_AX = org_ax;
	CPU_CX = org_cx;
	CPU_DX = org_dx;
	CPU_BX = org_bx;
	CPU_BP = org_bp;
	CPU_ES = org_es;
	ES_BASE = org_esbase;

	(void)arg1;
	(void)arg2;
}


#if defined(SUPPORT_HOSTDRV)
// ---- hostdrv

enum {
	OPEN_HOSTDRV	= 0,
	CLOSE_HOSTDRV	= 1,
	INTR_HOSTDRV	= 2
};

static void cmd_hostdrv(const void *arg1, long arg2) {

	switch(arg2) {
		case OPEN_HOSTDRV:
			if (hostdrv_mount() == SUCCESS) {
				cmd_outstr("ok", 0);
			}
			else {
				cmd_outstr("ng", 0);
			}
			break;

		case CLOSE_HOSTDRV:
			hostdrv_unmount();
			break;

		case INTR_HOSTDRV:
			hostdrv_intr();
			break;
	}
	(void)arg1;
}
#endif


// ----

static const char str_np2[] = "NP2";
static const char str_ver[] = "ver";
static const char str_poweroff[] = "poweroff";
static const char str_credit[] = "credit";
static const char str_cpu[] = "cpu";
static const char str_clock[] = "clock";
static const char str_multiple[] = "multiple";
static const char str_sxsibios[] = "sxsibios";
static const char str_hdrvcheck[] = "check_hostdrv";
static const char str_hdrvopen[] = "open_hostdrv";
static const char str_hdrvclose[] = "close_hostdrv";
static const char str_hdrvintr[] = "intr_hostdrv";


#if defined(NP2SYSP_VER)
static const char str_syspver[] = NP2SYSP_VER;
#else
#define	str_syspver		str_null
#endif

#if defined(NP2SYSP_CREDIT)
static const char str_syspcredit[] = NP2SYSP_CREDIT;
#else
#define	str_syspcredit	str_null
#endif


static const SYSPCMD np2spcmd[] = {
			{str_np2,		cmd_outstr,		str_np2,		0},
			{str_ver,		cmd_outstr,		str_syspver,	0},

// version:A
			{str_poweroff,	cmd_poweroff,	NULL,			0},

// version:B
			{str_credit,	cmd_outstr,		str_syspcredit,	0},
			{str_cpu,		cmd_cpu,		NULL,			0},
			{str_clock,		cmd_clock,		NULL,			0},
			{str_multiple,	cmd_multiple,	NULL,			0},


// extension
			{str_sxsibios,	cmd_sxsibios,	NULL,			0},
#if defined(SUPPORT_HOSTDRV)
			{str_hdrvcheck,	cmd_outstr,		"supported",	0},
			{str_hdrvopen,	cmd_hostdrv,	NULL,			OPEN_HOSTDRV},
			{str_hdrvclose,	cmd_hostdrv,	NULL,			CLOSE_HOSTDRV},
			{str_hdrvintr,	cmd_hostdrv,	NULL,			INTR_HOSTDRV},
#endif
};


static BOOL np2syspcmp(const char *p) {

	int		len;
	int		pos;

	len = strlen(p);
	if (!len) {
		return(TRUE);
	}
	pos = np2sysp.strpos;
	while(len--) {
		if (p[len] != np2sysp.substr[pos]) {
			return(TRUE);
		}
		pos--;
		pos &= NP2SYSP_MASK;
	}
	return(FALSE);
}

static void IOOUTCALL np2sysp_o7ed(UINT port, REG8 dat) {

	np2sysp.outval = (dat << 24) + (np2sysp.outval >> 8);
	(void)port;
}

static void IOOUTCALL np2sysp_o7ef(UINT port, REG8 dat) {

const SYSPCMD	*cmd;
const SYSPCMD	*cmdterm;

	np2sysp.substr[np2sysp.strpos] = (char)dat;
	cmd = np2spcmd;
	cmdterm = cmd + (sizeof(np2spcmd) / sizeof(SYSPCMD));
	while(cmd < cmdterm) {
		if (!np2syspcmp(cmd->key)) {
			cmd->func(cmd->arg1, cmd->arg2);
			break;
		}
		cmd++;
	}
	np2sysp.strpos++;
	np2sysp.strpos &= NP2SYSP_MASK;
	(void)port;
}

static REG8 IOINPCALL np2sysp_i7ed(UINT port) {

	REG8	ret;

	ret = (REG8)(np2sysp.inpval & 0xff);
	np2sysp.inpval = (ret << 24) + (np2sysp.inpval >> 8);
	(void)port;
	return(ret);
}

static REG8 IOINPCALL np2sysp_i7ef(UINT port) {

	REG8	ret;

	ret = (UINT8)np2sysp.outstr[np2sysp.outpos];
	if (ret) {
		np2sysp.outpos++;
		np2sysp.outpos &= NP2SYSP_MASK;
	}
	(void)port;
	return(ret);
}


// ---- I/F

void np2sysp_reset(void) {

	ZeroMemory(&np2sysp, sizeof(np2sysp));
}

void np2sysp_bind(void) {

	iocore_attachout(0x07ef, np2sysp_o7ed);
	iocore_attachout(0x07ef, np2sysp_o7ef);
	iocore_attachinp(0x07ef, np2sysp_i7ed);
	iocore_attachinp(0x07ef, np2sysp_i7ef);
}

