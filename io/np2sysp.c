#include	"compiler.h"
#include	"strres.h"
#include	"taskmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sxsibios.h"
#if defined(SUPPORT_HOSTDRV)
#include	"hostdrv.h"
#endif


#define		NP2SYSP_VER			"C"
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


void np2sysp_outstr(const void *arg1, long arg2) {

	milstr_ncpy(np2sysp.outstr, (char *)arg1, sizeof(np2sysp.outstr));
	np2sysp.outpos = 0;
	(void)arg2;
}

static void np2sysp_poweroff(const void *arg1, long arg2) {

	taskmng_exit();
	(void)arg1;
	(void)arg2;
}

static void np2sysp_cpu(const void *arg1, long arg2) {

	// CPUを返す
#if 1											// 80286 or V30
	if (!(CPU_TYPE & CPUTYPE_V30)) {
		np2sysp_outstr(str_80286, 0);
	}
	else {
		np2sysp_outstr(str_v30, 0);
	}
#else
	// 386機以降の場合 V30モードはエミュレーションだから固定(?)
	np2sysp_outstr(str_pentium, 0);
#endif
	(void)arg1;
	(void)arg2;
}

static void np2sysp_clock(const void *arg1, long arg2) {

	SPRINTF(np2sysp.outstr, str_mhz, (pccore.realclock + 500000) / 1000000);
	np2sysp.outpos = 0;
	(void)arg1;
	(void)arg2;
}

static void np2sysp_multiple(const void *arg1, long arg2) {

	SPRINTF(np2sysp.outstr, str_u, pccore.multiple);
	np2sysp.outpos = 0;
	(void)arg1;
	(void)arg2;
}

static void np2sysp_hwreset(const void *arg1, long arg2) {

	hardwarereset = TRUE;
	(void)arg1;
	(void)arg2;
}


// ----

static const char str_np2[] = "NP2";
static const char str_ver[] = "ver";
static const char str_poweroff[] = "poweroff";
static const char str_credit[] = "credit";
static const char str_cpu[] = "cpu";
static const char str_clock[] = "clock";
static const char str_multiple[] = "multiple";
static const char str_hwreset[] = "hardwarereset";
static const char str_sasibios[] = "sasibios";
static const char str_scsibios[] = "scsibios";
static const char str_scsidev[] = "scsi_dev";
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
			{str_np2,		np2sysp_outstr,		str_np2,		0},
			{str_ver,		np2sysp_outstr,		str_syspver,	0},

// version:A
			{str_poweroff,	np2sysp_poweroff,	NULL,			0},

// version:B
			{str_credit,	np2sysp_outstr,		str_syspcredit,	0},
			{str_cpu,		np2sysp_cpu,		NULL,			0},
			{str_clock,		np2sysp_clock,		NULL,			0},
			{str_multiple,	np2sysp_multiple,	NULL,			0},

// version:C
			{str_hwreset,	np2sysp_hwreset,	NULL,			0},

// extension
#if defined(SUPPORT_SASI)
			{str_sasibios,	np2sysp_sasi,		NULL,			0},
#endif
#if defined(SUPPORT_SCSI)
			{str_scsibios,	np2sysp_scsi,		NULL,			0},
			{str_scsidev,	np2sysp_scsidev,	NULL,			0},
#endif

#if defined(SUPPORT_HOSTDRV)
			{str_hdrvcheck,	np2sysp_outstr,		"0.74",			0},
			{str_hdrvopen,	hostdrv_mount,		NULL,			0},
			{str_hdrvclose,	hostdrv_unmount,	NULL,			0},
			{str_hdrvintr,	hostdrv_intr,		NULL,			0},
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

