#include	"compiler.h"
#include	"strres.h"
#include	"taskmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"sxsi.h"
// #include	"hostdrv.h"
// #include	"hostdir.h"


#define		NP2SYSP_VER			"B"
// #define	NP2SYSP_CREDIT		""					// 要るなら・・・

#define		OPEN_HOSTDRV		0
#define		INTR_HOSTDRV		1
#define		CLOSE_HOSTDRV		2

// NP2依存ポート
// port:07edh	reserved
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
	void	(*func)(const void *arg1, const void *arg2);
const void	*arg1;
const void	*arg2;
} SYSPCMD;

static const char str_80286[] = "80286";
static const char str_v30[] = "V30";
static const char str_pentium[] = "PENTIUM";
static const char str_mhz[] = "%uMHz";


static void np2sysp_outstr(const void *arg1, const void *arg2) {

	milstr_ncpy(np2sysp.outstr, (char *)arg1, sizeof(np2sysp.outstr));
	np2sysp.outpos = 0;
	(void)arg2;
}

static void np2sysp_pwroff(const void *arg1, const void *arg2) {

	taskmng_exit();
	(void)arg1;
	(void)arg2;
}

static void np2sysp_cpu(const void *arg1, const void *arg2) {

	// CPUを返す
#if 1											// 80286 or V30
	if (!(CPU_TYPE & CPUTYPE_V30)) {
		np2sysp_outstr(str_80286, NULL);
	}
	else {
		np2sysp_outstr(str_v30, NULL);
	}
#else
	// 386機以降の場合 V30モードはエミュレーションだから固定(?)
	np2sysp_outstr(str_pentium, NULL);
#endif
	(void)arg1;
	(void)arg2;
}

static void np2sysp_clock(const void *arg1, const void *arg2) {

	SPRINTF(np2sysp.outstr, str_mhz, (pc.realclock + 500000) / 1000000);
	np2sysp.outpos = 0;
	(void)arg1;
	(void)arg2;
}

static void np2sysp_mul(const void *arg1, const void *arg2) {

	SPRINTF(np2sysp.outstr, str_u, pc.multiple);
	np2sysp.outpos = 0;
	(void)arg1;
	(void)arg2;
}

typedef struct {
	UINT16	r_ax;
	UINT16	r_bx;
	UINT16	r_cx;
	UINT16	r_dx;
	UINT16	r_bp;
	UINT16	r_es;
} B1BREG;

static void np2sysp_sxsi(const void *arg1, const void *arg2) {

	B1BREG	org;
	UINT32	esbase_org;
	B1BREG	r;
	UINT8	ret;
	REG8	flag;

	org.r_ax = CPU_AX;
	org.r_cx = CPU_CX;
	org.r_dx = CPU_DX;
	org.r_bx = CPU_BX;
	org.r_bp = CPU_BP;
	org.r_es = CPU_ES;
	esbase_org = ES_BASE;

	i286_memstr_read(CPU_SS, CPU_SP, &r, sizeof(r));
	CPU_AX = r.r_ax;
	CPU_BX = r.r_bx;
	CPU_CX = r.r_cx;
	CPU_DX = r.r_dx;
	CPU_BP = r.r_bp;
	CPU_ES = r.r_es;
	ES_BASE = r.r_es << 4;
	switch(r.r_ax & 0xf0) {
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
	r.r_ax = CPU_AL + (ret << 8);
	r.r_bx = CPU_BX;
	r.r_cx = CPU_CX;
	r.r_dx = CPU_DX;
	i286_memstr_write(CPU_SS, CPU_SP, &r, 8);
	flag = i286_membyte_read(CPU_SS, CPU_SP + 0x16) & 0xfe;
	if (ret >= 0x20) {
		flag += 1;
	}
	i286_membyte_write(CPU_SS, CPU_SP + 0x16, flag);

	CPU_AX = org.r_ax;
	CPU_CX = org.r_cx;
	CPU_DX = org.r_dx;
	CPU_BX = org.r_bx;
	CPU_BP = org.r_bp;
	CPU_ES = org.r_es;
	ES_BASE = esbase_org;
}

#if 0
static void np2sysp_hostdrv(const void *arg1, const void *arg2) {

	switch((DWORD)arg1) {
		case OPEN_HOSTDRV:
			if(open_hostdrv())
				np2sysp_outstr("ok", NULL);
			else
				np2sysp_outstr("ng", NULL);
			break;

		case INTR_HOSTDRV:
			intr_hostdrv();
			break;

		case CLOSE_HOSTDRV:
			close_hostdrv();
			break;
	}
}
#endif


static const char str_np2[] = "NP2";
static const char str_ver[] = "ver";
static const char str_poweroff[] = "poweroff";
static const char str_credit[] = "credit";
static const char str_cpu[] = "cpu";
static const char str_clock[] = "clock";
static const char str_multiple[] = "multiple";
static const char str_sxsibios[] = "sxsibios";

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
			{str_np2,		np2sysp_outstr,	str_np2,		NULL},
			{str_ver,		np2sysp_outstr,	str_syspver,	NULL},

// version:A
			{str_poweroff,	np2sysp_pwroff,	NULL,			NULL},

// version:B
			{str_credit,	np2sysp_outstr,	str_syspcredit,	NULL},
			{str_cpu,		np2sysp_cpu,	NULL,			NULL},
			{str_clock,		np2sysp_clock,	NULL,			NULL},
			{str_multiple,	np2sysp_mul,	NULL,			NULL},

// version:C
			{str_sxsibios,	np2sysp_sxsi,	NULL,			NULL},

#if 0
// hostdrv
			{"check_hostdrv",	np2sysp_outstr,		"supported",
																NULL},
			{"open_hostdrv",	np2sysp_hostdrv,	(void *)OPEN_HOSTDRV,
																NULL},
			{"intr_hostdrv",	np2sysp_hostdrv,	(void *)INTR_HOSTDRV,
																NULL},
			{"close_hostdrv",	np2sysp_hostdrv,	(void *)CLOSE_HOSTDRV,
																NULL},

			{hostdir_check,		np2sysp_outstr,	hostdir_check,	NULL},
			{"hostdir_reset",	hostdir_reset,	NULL,			NULL},
			{"hostdir_band",	hostdir_band,	NULL,			NULL},
			{"hostdir_int",		hostdir_int,	NULL,			NULL},
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

	iocore_attachout(0x07ef, np2sysp_o7ef);
	iocore_attachinp(0x07ef, np2sysp_i7ef);
}

