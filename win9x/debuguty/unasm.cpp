#include	"compiler.h"
#include	"unasm.h"


static const char str_al[] = "al";
static const char str_cl[] = "cl";
static const char str_dl[] = "dl";
static const char str_bl[] = "bl";
static const char str_ah[] = "ah";
static const char str_ch[] = "ch";
static const char str_dh[] = "dh";
static const char str_bh[] = "bh";

static const char str_ax[] = "ax";
static const char str_cx[] = "cx";
static const char str_dx[] = "dx";
static const char str_bx[] = "bx";
static const char str_sp[] = "sp";
static const char str_bp[] = "bp";
static const char str_si[] = "si";
static const char str_di[] = "di";

static const char str_cs[] = "cs";
static const char str_ds[] = "ds";
static const char str_es[] = "es";
static const char str_ss[] = "ss";

static const char str_bxsi[] = "bx+si";
static const char str_bxdi[] = "bx+di";
static const char str_bpsi[] = "bp+si";
static const char str_bpdi[] = "bp+di";

static const char *reg8[8] = {	str_al, str_cl, str_dl, str_bl,
								str_ah, str_ch, str_dh, str_bh};
static const char *reg16[8] = {	str_ax, str_cx, str_dx, str_bx,
								str_sp, str_bp, str_si, str_di};
static const char *seg16[4] = {	str_es, str_cs, str_ss, str_ds};

static const char *lea16[8] = {	str_bxsi, str_bxdi, str_bpsi, str_bpdi,
								str_si, str_di, str_bp, str_bx};

enum {
	OPERAND_NONE = 0,					// オペランド無し
	OPERAND_NONE_FIX,

	OPERAND_EA,
	OPERAND_EA_R8,
	OPERAND_EA_R16,
	OPERAND_EA8_IMM8,
	OPERAND_EA16_IMM16,
	OPERAND_EA_SEG,

	OPERAND_R8_EA,
	OPERAND_R16_EA,
	OPERAND_R16_EA_D8,
	OPERAND_R16_EA_D16,
	OPERAND_SEG_EA,

	OPERAND_AL_MEM,
	OPERAND_AX_MEM,
	OPERAND_MEM_AL,
	OPERAND_MEM_AX,

	OPERAND_AL_IMM8,					// アキュームレータ, 即値 8bit
	OPERAND_AX_IMM16,					// アキュームレータ, 即値16bit
	OPERAND_R_IMM8,						// reg, 即値 8bit
	OPERAND_R_IMM16,					// reg, 即値16bit
	OPERAND_IMM8,						// 即値 8bit
	OPERAND_IMM8x,						// 即値 8bit
	OPERAND_IMM16,						// 即値16bit
	OPERAND_IMM_SHORT,					// 即値short
	OPERAND_IMM_NEAR,					// 即値near
	OPERAND_IMM_FAR,					// 即値far
	MNEMONIC_REG,						// レジスタ
	MNEMONIC_AX_REG,					// ax, レジスタ
	MNEMONIC_SEG,						// セグメント

	OPERAND_IMM8_AL,					// 即値 8bit
	OPERAND_IMM16_AL,					// 即値16bit

	MNEMONIC_DX_AL,
	MNEMONIC_DX_AX,
	MNEMONIC_AL_DX,
	MNEMONIC_AX_DX,

	OPERAND_IMM16_IMM8,
	OPERAND_IMM_10,						// 10
	OPERAND_3,

	MNEMONIC_0F,
	MNEMONIC_63,
	MNEMONIC_64,
	MNEMONIC_65,
	MNEMONIC_66,
	MNEMONIC_67,
	MNEMONIC_ESC,
	MNEMONIC_80,
	MNEMONIC_81,
	MNEMONIC_82,
	MNEMONIC_83,
	MNEMONIC_C0,
	MNEMONIC_C1,
	MNEMONIC_D0,
	MNEMONIC_D1,
	MNEMONIC_D2,
	MNEMONIC_D3,
	MNEMONIC_F6,
	MNEMONIC_F7,
	MNEMONIC_FE,
	MNEMONIC_FF,
};

typedef struct {
	char	mnemonic[7];
	BYTE	type;
} MENMONIC_TBL;

static const MENMONIC_TBL mnemonic[256] = {
			{"add",		OPERAND_EA_R8},			// 00:	add		EA, REG8
			{"add",		OPERAND_EA_R16},		// 01:	add		EA, REG16
			{"add",		OPERAND_R8_EA},			// 02:	add		REG8, EA
			{"add",		OPERAND_R16_EA},		// 03:	add		REG16, EA
			{"add",		OPERAND_AL_IMM8},		// 04:	add		al, DATA8
			{"add",		OPERAND_AX_IMM16},		// 05:	add		ax, DATA16
			{"push",	MNEMONIC_SEG},			// 06:	push	es
			{"pop",		MNEMONIC_SEG},			// 07:	pop		es
			{"or",		OPERAND_EA_R8},			// 08:	or		EA, REGF8
			{"or",		OPERAND_EA_R16},		// 09:	or		EA, REG16
			{"or",		OPERAND_R8_EA},			// 0A:	or		REG8, EA
			{"or",		OPERAND_R16_EA},		// 0B:	or		REG16, EA
			{"or",		OPERAND_AL_IMM8},		// 0C:	or		al, DATA8
			{"or",		OPERAND_AX_IMM16},		// 0D:	or		ax, DATA16
			{"push",	MNEMONIC_SEG},			// 0E:	push	cs
			{"cts",		MNEMONIC_0F},			// 0F:	i286 upper opcode

			{"adc",		OPERAND_EA_R8},			// 10:	adc		EA, REG8
			{"adc",		OPERAND_EA_R16},		// 11:	adc		EA, REG16
			{"adc",		OPERAND_R8_EA},			// 12:	adc		REG8, EA
			{"adc",		OPERAND_R16_EA},		// 13:	adc		REG16, EA
			{"adc",		OPERAND_AL_IMM8},		// 14:	adc		al, DATA8
			{"adc",		OPERAND_AX_IMM16},		// 15:	adc		ax, DATA16
			{"push",	MNEMONIC_SEG},			// 16:	push	ss
			{"pop",		MNEMONIC_SEG},			// 17:	pop		ss
			{"sbb",		OPERAND_EA_R8},			// 18:	sbb		EA, REGF8
			{"sbb",		OPERAND_EA_R16},		// 19:	sbb		EA, REG16
			{"sbb",		OPERAND_R8_EA},			// 1A:	sbb		REG8, EA
			{"sbb",		OPERAND_R16_EA},		// 1B:	sbb		REG16, EA
			{"sbb",		OPERAND_AL_IMM8},		// 1C:	sbb		al, DATA8
			{"sbb",		OPERAND_AX_IMM16},		// 1D:	sbb		ax, DATA16
			{"push",	MNEMONIC_SEG},			// 1E:	push	ds
			{"pop",		MNEMONIC_SEG},			// 1F:	pop		ds

			{"and",		OPERAND_EA_R8},			// 20:	and		EA, REG8
			{"and",		OPERAND_EA_R16},		// 21:	and		EA, REG16
			{"and",		OPERAND_R8_EA},			// 22:	and		REG8, EA
			{"and",		OPERAND_R16_EA},		// 23:	and		REG16, EA
			{"and",		OPERAND_AL_IMM8},		// 24:	and		al, DATA8
			{"and",		OPERAND_AX_IMM16},		// 25:	and		ax, DATA16
			{"es:",		OPERAND_NONE},			// 26:	es:
			{"daa",		OPERAND_NONE_FIX},		// 27:	daa
			{"sub",		OPERAND_EA_R8},			// 28:	sub		EA, REGF8
			{"sub",		OPERAND_EA_R16},		// 29:	sub		EA, REG16
			{"sub",		OPERAND_R8_EA},			// 2A:	sub		REG8, EA
			{"sub",		OPERAND_R16_EA},		// 2B:	sub		REG16, EA
			{"sub",		OPERAND_AL_IMM8},		// 2C:	sub		al, DATA8
			{"sub",		OPERAND_AX_IMM16},		// 2D:	sub		ax, DATA16
			{"cs:",		OPERAND_NONE},			// 2E:	cs:
			{"das",		OPERAND_NONE_FIX},		// 2F:	das

			{"xor",		OPERAND_EA_R8},			// 30:	xor		EA, REGF8
			{"xor",		OPERAND_EA_R16},		// 31:	xor		EA, REG16
			{"xor",		OPERAND_R8_EA},			// 32:	xor		REG8, EA
			{"xor",		OPERAND_R16_EA},		// 33:	xor		REG16, EA
			{"xor",		OPERAND_AL_IMM8},		// 34:	xor		al, DATA8
			{"xor",		OPERAND_AX_IMM16},		// 35:	xor		ax, DATA16
			{"ss:",		OPERAND_NONE},			// 36:	ss:
			{"aaa",		OPERAND_NONE_FIX},		// 37:	aaa
			{"cmp",		OPERAND_EA_R8},			// 38:	cmp		EA, REGF8
			{"cmp",		OPERAND_EA_R16},		// 39:	cmp		EA, REG16
			{"cmp",		OPERAND_R8_EA},			// 3A:	cmp		REG8, EA
			{"cmp",		OPERAND_R16_EA},		// 3B:	cmp		REG16, EA
			{"cmp",		OPERAND_AL_IMM8},		// 3C:	cmp		al, DATA8
			{"cmp",		OPERAND_AX_IMM16},		// 3D:	cmp		ax, DATA16
			{"ds:",		OPERAND_NONE},			// 3E:	ds:
			{"aas",		OPERAND_NONE_FIX},		// 3F:	aas

			{"inc",		MNEMONIC_REG},			// 40:	inc		ax
			{"inc",		MNEMONIC_REG},			// 41:	inc		cx
			{"inc",		MNEMONIC_REG},			// 42:	inc		dx
			{"inc",		MNEMONIC_REG},			// 43:	inc		bx
			{"inc",		MNEMONIC_REG},			// 44:	inc		sp
			{"inc",		MNEMONIC_REG},			// 45:	inc		bp
			{"inc",		MNEMONIC_REG},			// 46:	inc		si
			{"inc",		MNEMONIC_REG},			// 47:	inc		di
			{"dec",		MNEMONIC_REG},			// 48:	dec		ax
			{"dec",		MNEMONIC_REG},			// 49:	dec		cx
			{"dec",		MNEMONIC_REG},			// 4A:	dec		dx
			{"dec",		MNEMONIC_REG},			// 4B:	dec		bx
			{"dec",		MNEMONIC_REG},			// 4C:	dec		sp
			{"dec",		MNEMONIC_REG},			// 4D:	dec		bp
			{"dec",		MNEMONIC_REG},			// 4E:	dec		si
			{"dec",		MNEMONIC_REG},			// 4F:	dec		di

			{"push",	MNEMONIC_REG},			// 50:	push	ax
			{"push",	MNEMONIC_REG},			// 51:	push	cx
			{"push",	MNEMONIC_REG},			// 52:	push	dx
			{"push",	MNEMONIC_REG},			// 53:	push	bx
			{"push",	MNEMONIC_REG},			// 54:	push	sp
			{"push",	MNEMONIC_REG},			// 55:	push	bp
			{"push",	MNEMONIC_REG},			// 56:	push	si
			{"push",	MNEMONIC_REG},			// 57:	push	di
			{"pop",		MNEMONIC_REG},			// 58:	pop		ax
			{"pop",		MNEMONIC_REG},			// 59:	pop		cx
			{"pop",		MNEMONIC_REG},			// 5A:	pop		dx
			{"pop",		MNEMONIC_REG},			// 5B:	pop		bx
			{"pop",		MNEMONIC_REG},			// 5C:	pop		sp
			{"pop",		MNEMONIC_REG},			// 5D:	pop		bp
			{"pop",		MNEMONIC_REG},			// 5E:	pop		si
			{"pop",		MNEMONIC_REG},			// 5F:	pop		di

			{"pusha",	OPERAND_NONE},			// 60:	pusha
			{"popa",	OPERAND_NONE},			// 61:	popa
			{"bound",	OPERAND_R16_EA},		// 62:	bound
			{"db",		MNEMONIC_63},			// 63:	arpl
			{"db",		MNEMONIC_64},			// 64:	reserved
			{"db",		MNEMONIC_65},			// 65:	reserved
			{"db",		MNEMONIC_66},			// 66:	reserved
			{"db",		MNEMONIC_67},			// 67:	reserved
			{"push",	OPERAND_IMM16},			// 68:	push	DATA16
			{"imul",	OPERAND_R16_EA_D16},	// 69:	imul	REG, EA, DATA16
			{"push",	OPERAND_IMM8},			// 6A:	push	DATA8
			{"imul",	OPERAND_R16_EA_D8},		// 6B:	imul	REG, EA, DATA8
			{"insb",	OPERAND_NONE},			// 6C:	insb
			{"insw",	OPERAND_NONE},			// 6D:	insw
			{"outsb",	OPERAND_NONE},			// 6E:	outsb
			{"outsw",	OPERAND_NONE},			// 6F:	outsw

			{"jo",		OPERAND_IMM_SHORT},		// 70:	jo short
			{"jno",		OPERAND_IMM_SHORT},		// 71:	jno short
			{"jb",		OPERAND_IMM_SHORT},		// 72:	jnae/jb/jc short
			{"jnb",		OPERAND_IMM_SHORT},		// 73:	jae/jnb/jnc short
			{"jz",		OPERAND_IMM_SHORT},		// 74:	je/jz short
			{"jnz",		OPERAND_IMM_SHORT},		// 75:	jne/jnz short
			{"jbe",		OPERAND_IMM_SHORT},		// 76:	jna/jbe short
			{"ja",		OPERAND_IMM_SHORT},		// 77:	ja/jnbe short
			{"js",		OPERAND_IMM_SHORT},		// 78:	js short
			{"jns",		OPERAND_IMM_SHORT},		// 79:	jns short
			{"jp",		OPERAND_IMM_SHORT},		// 7A:	jp/jpe short
			{"jpo",		OPERAND_IMM_SHORT},		// 7B:	jnp/jpo short
			{"jl",		OPERAND_IMM_SHORT},		// 7C:	jl/jnge short
			{"jge",		OPERAND_IMM_SHORT},		// 7D:	jnl/jge short
			{"jle",		OPERAND_IMM_SHORT},		// 7E:	jle/jng short
			{"jg",		OPERAND_IMM_SHORT},		// 7F:	jg/jnle short

			{"?",		MNEMONIC_80},			// 80:	op		EA8, DATA8
			{"?",		MNEMONIC_81},			// 81:	op		EA16, DATA16
			{"?",		MNEMONIC_80},			// 82:	op		EA8, DATA8
			{"?",		MNEMONIC_83},			// 83:	op		EA16, DATA8
			{"test",	OPERAND_EA_R8},			// 84:	test	EA, REG8
			{"test",	OPERAND_EA_R16},		// 85:	test	EA, REG16
			{"xchg",	OPERAND_R8_EA},			// 86:	xchg	EA, REG8
			{"xchg",	OPERAND_R16_EA},		// 87:	xchg	EA, REG16
			{"mov",		OPERAND_EA_R8},			// 88:	mov		EA, REG8
			{"mov",		OPERAND_EA_R16},		// 89:	mov		EA, REG16
			{"mov",		OPERAND_R8_EA},			// 8A:	mov		REG8, EA
			{"mov",		OPERAND_R16_EA},		// 8B:	add		REG16, EA
			{"mov",		OPERAND_EA_SEG},		// 8C:	mov		EA, segreg
			{"lea",		OPERAND_R16_EA},		// 8D:	lea		REG16, EA
			{"mov",		OPERAND_SEG_EA},		// 8E:	mov		segrem, EA
			{"pop",		OPERAND_EA},			// 8F:	pop		EA

			{"nop",		OPERAND_NONE},			// 90:	xchg	ax, ax
			{"xchg",	MNEMONIC_AX_REG},		// 91:	xchg	ax, cx
			{"xchg",	MNEMONIC_AX_REG},		// 92:	xchg	ax, dx
			{"xchg",	MNEMONIC_AX_REG},		// 93:	xchg	ax, bx
			{"xchg",	MNEMONIC_AX_REG},		// 94:	xchg	ax, sp
			{"xchg",	MNEMONIC_AX_REG},		// 95:	xchg	ax, bp
			{"xchg",	MNEMONIC_AX_REG},		// 96:	xchg	ax, si
			{"xchg",	MNEMONIC_AX_REG},		// 97:	xchg	ax, di
			{"cbw",		OPERAND_NONE},			// 98:	cbw
			{"cwd",		OPERAND_NONE},			// 99:	cwd
			{"call",	OPERAND_IMM_FAR},		// 9A:	call far
			{"wait",	OPERAND_NONE},			// 9B:	wait
			{"pushf",	OPERAND_NONE},			// 9C:	pushf
			{"popf",	OPERAND_NONE},			// 9D:	popf
			{"sahf",	OPERAND_NONE},			// 9E:	sahf
			{"lahf",	OPERAND_NONE},			// 9F:	lahf

			{"mov",		OPERAND_AL_MEM},		// A0:	mov		al, m8
			{"mov",		OPERAND_AX_MEM},		// A1:	mov		ax, m16
			{"mov",		OPERAND_MEM_AL},		// A2:	mov		m8, al
			{"mov",		OPERAND_MEM_AX},		// A3:	mov		m16, ax
			{"movsb",	OPERAND_NONE_FIX},		// A4:	movsb
			{"movsw",	OPERAND_NONE_FIX},		// A5:	movsw
			{"cmpsb",	OPERAND_NONE_FIX},		// A6:	cmpsb
			{"cmpsw",	OPERAND_NONE_FIX},		// A7:	cmpsw
			{"test",	OPERAND_AL_IMM8},		// A8:	test	al, DATA8
			{"test",	OPERAND_AX_IMM16},		// A9:	test	ax, DATA16
			{"stosb",	OPERAND_NONE_FIX},		// AA:	stosw
			{"stosw",	OPERAND_NONE_FIX},		// AB:	stosw
			{"lodsb",	OPERAND_NONE_FIX},		// AC:	lodsb
			{"lodsw",	OPERAND_NONE_FIX},		// AD:	lodsw
			{"scasb",	OPERAND_NONE_FIX},		// AE:	scasb
			{"scasw",	OPERAND_NONE_FIX},		// AF:	scasw

			{"mov",		OPERAND_R_IMM8},		// B0:	mov		al, imm8
			{"mov",		OPERAND_R_IMM8},		// B1:	mov		cl, imm8
			{"mov",		OPERAND_R_IMM8},		// B2:	mov		dl, imm8
			{"mov",		OPERAND_R_IMM8},		// B3:	mov		bl, imm8
			{"mov",		OPERAND_R_IMM8},		// B4:	mov		ah, imm8
			{"mov",		OPERAND_R_IMM8},		// B5:	mov		ch, imm8
			{"mov",		OPERAND_R_IMM8},		// B6:	mov		dh, imm8
			{"mov",		OPERAND_R_IMM8},		// B7:	mov		bh, imm8
			{"mov",		OPERAND_R_IMM16},		// B8:	mov		ax, imm16
			{"mov",		OPERAND_R_IMM16},		// B9:	mov		cx, imm16
			{"mov",		OPERAND_R_IMM16},		// BA:	mov		dx, imm16
			{"mov",		OPERAND_R_IMM16},		// BB:	mov		bx, imm16
			{"mov",		OPERAND_R_IMM16},		// BC:	mov		sp, imm16
			{"mov",		OPERAND_R_IMM16},		// BD:	mov		bp, imm16
			{"mov",		OPERAND_R_IMM16},		// BE:	mov		si, imm16
			{"mov",		OPERAND_R_IMM16},		// BF:	mov		di, imm16

			{"?",		MNEMONIC_C0},			// C0:	shift	EA8, DATA8
			{"?",		MNEMONIC_C1},			// C1:	shift	EA16, DATA8
			{"ret",		OPERAND_IMM16},			// C2:	ret near DATA16
			{"ret",		OPERAND_NONE},			// C3:	ret near
			{"les",		OPERAND_R16_EA},		// C4:	les		REG16, EA
			{"lds",		OPERAND_R16_EA},		// C5:	lds		REG16, EA
			{"mov",		OPERAND_EA8_IMM8},		// C6:	mov		EA8, DATA8
			{"mov",		OPERAND_EA16_IMM16},	// C7:	mov		EA16, DATA16
			{"enter",	OPERAND_IMM16_IMM8},	// C8:	enter	DATA16, DATA8
			{"leave",	OPERAND_NONE},			// C9:	leave
			{"retf",	OPERAND_IMM16},			// CA:	ret far	DATA16
			{"retf",	OPERAND_NONE},			// CB:	ret far
			{"int",		OPERAND_3},				// CC:	int		3
			{"int",		OPERAND_IMM8x},			// CD:	int		DATA8
			{"into",	OPERAND_NONE},			// CE:	into
			{"iret",	OPERAND_NONE},			// CF:	iret

			{"?",		MNEMONIC_D0},			// D0:	shift EA8, 1
			{"?",		MNEMONIC_D1},			// D1:	shift EA16, 1
			{"?",		MNEMONIC_D2},			// D2:	shift EA8, cl
			{"?",		MNEMONIC_D3},			// D3:	shift EA16, cl
			{"aam",		OPERAND_IMM_10},		// D4:	AAM
			{"aad",		OPERAND_IMM_10},		// D5:	AAD
			{"setalc",	OPERAND_NONE},			// D6:	setalc (80286)
			{"xlat",	OPERAND_NONE},			// D7:	xlat
			{"esc",		MNEMONIC_ESC},			// D8:	esc
			{"esc",		MNEMONIC_ESC},			// D9:	esc
			{"esc",		MNEMONIC_ESC},			// DA:	esc
			{"esc",		MNEMONIC_ESC},			// DB:	esc
			{"esc",		MNEMONIC_ESC},			// DC:	esc
			{"esc",		MNEMONIC_ESC},			// DD:	esc
			{"esc",		MNEMONIC_ESC},			// DE:	esc
			{"esc",		MNEMONIC_ESC},			// DF:	esc

			{"loopnz",	OPERAND_IMM_SHORT},		// E0:	loopnz
			{"loopz",	OPERAND_IMM_SHORT},		// E1:	loopz
			{"loop",	OPERAND_IMM_SHORT},		// E2:	loop
			{"jcxz",	OPERAND_IMM_SHORT},		// E3:	jcxz
			{"in",		OPERAND_AL_IMM8},		// E4:	in		al, DATA8
			{"in",		OPERAND_AX_IMM16},		// E5:	in		ax, DATA8
			{"out",		OPERAND_IMM8_AL},		// E6:	out		DATA8, al
			{"out",		OPERAND_IMM16_AL},		// E7:	out		DATA8, ax
			{"call",	OPERAND_IMM_NEAR},		// E8:	call near
			{"jmp",		OPERAND_IMM_NEAR},		// E9:	jmp near
			{"jmp",		OPERAND_IMM_FAR},		// EA:	jmp far
			{"jmp",		OPERAND_IMM_SHORT},		// EB:	jmp short
			{"in",		MNEMONIC_AL_DX},		// EC:	in		al, dx
			{"in",		MNEMONIC_AX_DX},		// ED:	in		ax, dx
			{"out",		MNEMONIC_DX_AL},		// EE:	out		dx, al
			{"out",		MNEMONIC_DX_AX},		// EF:	out		dx, ax

			{"lock",	OPERAND_NONE},			// F0:	lock
			{"lock",	OPERAND_NONE},			// F1:	lock
			{"repnz",	OPERAND_NONE},			// F2:	repne
			{"repz",	OPERAND_NONE},			// F3:	repe
			{"hlt",		OPERAND_NONE},			// F4:	hlt
			{"cmc",		OPERAND_NONE},			// F5:	cmc
			{"?",		MNEMONIC_F6},			// F6:	
			{"?",		MNEMONIC_F7},			// F7:	
			{"clc",		OPERAND_NONE},			// F8:	clc
			{"stc",		OPERAND_NONE},			// F9:	stc
			{"cli",		OPERAND_NONE},			// FA:	cli
			{"sti",		OPERAND_NONE},			// FB:	sti
			{"cld",		OPERAND_NONE},			// FC:	cld
			{"std",		OPERAND_NONE},			// FD:	std
			{"?",		MNEMONIC_FE},			// FE:	
			{"?",		MNEMONIC_FF}};			// FF:	

static const char opcd80[8][4] = {	"add",	"or",	"adc",	"sbb",
									"and",	"sub",	"xor",	"cmp"};
static const char opcdc0[8][4] = {	"rol",	"ror",	"rcl",	"rcr",
									"shl",	"shr",	"shl",	"sar"};
static const char opcdf6[8][8] = {	"test",	"test",	"not",	"neg",
									"mul",	"imul",	"div",	"idiv"};
static const char opcdfe[8][8] = {	"inc",	"dec",	"call",	"call",
									"jmp",	"jmp",	"push",	"pop"};


// ----

static void cat_cnm(char *str) {

	strcat(str, ",");
}

static void cat_hex8x(UNASM_T *unasm_t, BYTE value) {

	char	buf[16];

	wsprintf(buf, "%02x", value);
	strcat(unasm_t->operand, buf);
}

static void cat_hex8(UNASM_T *unasm_t, BYTE value) {

	char	buf[16];

	wsprintf(buf, "%02x", value);
	strcat(unasm_t->operand, buf);
	if ((value >= 0x20) && (value < 0x7f)) {
		wsprintf(unasm_t->extend, ";'%c'", value);
	}
}

static void cat_hexs8x(UNASM_T *unasm_t, BYTE value) {

	char	buf[16];

	if (value & 0x80) {
		wsprintf(buf, "-%02x", 0x100-value);
	}
	else {
		wsprintf(buf, "+%02x", value);
	}
	strcat(unasm_t->operand, buf);
}

#if 0
static void cat_hexs8(UNASM_T *unasm_t, BYTE value) {

	char	buf[16];

	if (value & 0x80) {
		wsprintf(buf, "-%02x", 0x100-value);
	}
	else {
		wsprintf(buf, "+%02x", value);
	}
	strcat(unasm_t->operand, buf);
	if ((value >= 0x20) && (value < 0x7f)) {
		wsprintf(unasm_t->extend, ";'%c'", value);
	}
}
#endif

static void cat_hex16(char *str, WORD value) {

	char	buf[16];

	wsprintf(buf, "%04x", value);
	strcat(str, buf);
}

static void cat_mem16(char *str, WORD value, PREFIX_T *fix) {

	char	buf[16];

	if (fix->seg) {
		strcat(str, seg16[(fix->seg & 0x18) >> 3]);
		strcat(str, ":");
		fix->seg = 0;
	}
	wsprintf(buf, "[%04x]", value);
	strcat(str, buf);
}

// ------------------

static int cat_ea(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	int		ret;

	ret = 0;
	if (fix->seg) {
		strcat(unasm_t->operand, seg16[(fix->seg & 0x18) >> 3]);
		strcat(unasm_t->operand, ":");
		fix->seg = 0;
	}
	strcat(unasm_t->operand, "[");
	if ((bincode[1] & 0xc7) != 0x06) {
		strcat(unasm_t->operand, lea16[bincode[1] & 7]);
		switch(bincode[1] & 0xc0) {
			case 0x40:
				cat_hexs8x(unasm_t, bincode[2]);
				ret = 1;
				break;
			case 0x80:
				strcat(unasm_t->operand, "+");
				cat_hex16(unasm_t->operand, *(WORD *)(bincode + 2));
				ret = 2;
				break;
		}
	}
	else {
		cat_hex16(unasm_t->operand, *(WORD *)(bincode + 2));
		ret = 2;
	}
	strcat(unasm_t->operand, "]");
	return(ret);
}

// ----

static int cat_ea8(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	if (bincode[1] < 0xc0) {
		return(cat_ea(bincode, fix, unasm_t));
	}
	strcat(unasm_t->operand, reg8[bincode[1] & 0x07]);
	return(0);
}

static int cat_ea8b(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	if (bincode[1] < 0xc0) {
		strcat(unasm_t->operand, "byte ptr ");
		return(cat_ea(bincode, fix, unasm_t));
	}
	strcat(unasm_t->operand, reg8[bincode[1] & 0x07]);
	return(0);
}

static int cat_ea_r8(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	int		ret;

	ret = cat_ea8(bincode, fix, unasm_t);
	cat_cnm(unasm_t->operand);
	strcat(unasm_t->operand, reg8[(bincode[1] & 0x38) >> 3]);
	return(ret+1);
}

static int cat_r8_ea(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	strcat(unasm_t->operand, reg8[(bincode[1] & 0x38) >> 3]);
	cat_cnm(unasm_t->operand);
	return(cat_ea8(bincode, fix, unasm_t) + 1);
}

// ----

static int cat_ea16(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	if (bincode[1] < 0xc0) {
		return(cat_ea(bincode, fix, unasm_t));
	}
	strcat(unasm_t->operand, reg16[bincode[1] & 0x07]);
	return(0);
}

static int cat_ea16w(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	if (bincode[1] < 0xc0) {
		strcat(unasm_t->operand, "word ptr ");
		return(cat_ea(bincode, fix, unasm_t));
	}
	strcat(unasm_t->operand, reg16[bincode[1] & 0x07]);
	return(0);
}

static int cat_ea_r16(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	int		ret;

	ret = cat_ea16(bincode, fix, unasm_t);
	cat_cnm(unasm_t->operand);
	strcat(unasm_t->operand, reg16[(bincode[1] & 0x38) >> 3]);
	return(ret+1);
}

static int cat_r16_ea(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	strcat(unasm_t->operand, reg16[(bincode[1] & 0x38) >> 3]);
	cat_cnm(unasm_t->operand);
	return(cat_ea16(bincode, fix, unasm_t) + 1);
}

static int cat_r16_eax(BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	strcat(unasm_t->operand, reg16[(bincode[1] & 0x38) >> 3]);
	if ((bincode[1] < 0xc0) || (((bincode[1] >> 3) ^ bincode[1]) & 7)) {
		cat_cnm(unasm_t->operand);
		return(cat_ea16(bincode, fix, unasm_t) + 1);
	}
	return(1);
}


// ----

void unasm_reset(PREFIX_T *fix) {

	if (fix) {
		fix->seg = 0;
	}
}

int unasm(WORD adrs, BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t) {

	int		ret;
	int		pos;
	int		rem;

	ret = 0;
	if ((bincode) && (fix)) {
		BYTE	c;
		UNASM_T	una;

		rem = 6;
		do {
			c = *bincode;
			ret++;
			if ((c & 0xe7) == 0x26) {
				fix->seg = c;
			}
			else {
				break;
			}
			bincode++;
		} while(--rem);

		una.mnemonic = mnemonic[c].mnemonic;
		una.operand[0] = 0;
		una.extend[0] = 0;

		switch(mnemonic[c].type) {
			case OPERAND_NONE:
				break;

			case OPERAND_NONE_FIX:
				if (fix->seg) {
					strcat(una.operand, seg16[(fix->seg & 0x18) >> 3]);
					strcat(una.operand, ":");
					fix->seg = 0;
				}
				break;

			case OPERAND_EA:
				ret += cat_ea16(bincode, fix, &una);
				ret++;
				break;

			case OPERAND_EA_R8:
				ret += cat_ea_r8(bincode, fix, &una);
				break;

			case OPERAND_EA_R16:
				ret += cat_ea_r16(bincode, fix, &una);
				break;

			case OPERAND_EA8_IMM8:
				pos = cat_ea8b(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				cat_hex8(&una, bincode[pos + 1]);
				ret += pos + 1;
				break;

			case OPERAND_EA16_IMM16:
				pos = cat_ea16w(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				cat_hex16(una.operand, *(WORD *)(bincode + pos + 1));
				ret += pos + 2;
				break;

			case OPERAND_EA_SEG:
				ret += cat_ea16(bincode, fix, &una);
				cat_cnm(una.operand);
				strcat(una.operand, seg16[(bincode[1] & 0x18) >> 3]);
				ret++;
				break;

			case OPERAND_R8_EA:
				ret += cat_r8_ea(bincode, fix, &una);
				break;

			case OPERAND_R16_EA:
				ret += cat_r16_ea(bincode, fix, &una);
				break;

			case OPERAND_R16_EA_D8:
				pos = cat_r16_eax(bincode, fix, &una);
				cat_cnm(una.operand);
				cat_hex8(&una, bincode[pos + 1]);
				ret += pos + 1;
				break;

			case OPERAND_R16_EA_D16:
				pos = cat_r16_eax(bincode, fix, &una);
				cat_cnm(una.operand);
				cat_hex16(una.operand, *(WORD *)(bincode + pos + 1));
				ret += pos + 2;
				break;

			case OPERAND_SEG_EA:
				strcpy(una.operand, seg16[(bincode[1] & 0x18) >> 3]);
				cat_cnm(una.operand);
				ret += cat_ea16(bincode, fix, &una);
				ret++;
				break;

			case OPERAND_AL_MEM:
				strcpy(una.operand, str_al);
				cat_cnm(una.operand);
				cat_mem16(una.operand, *(WORD *)(bincode + 1), fix);
				ret += 2;
				break;

			case OPERAND_AX_MEM:
				strcpy(una.operand, str_ax);
				cat_cnm(una.operand);
				cat_mem16(una.operand, *(WORD *)(bincode + 1), fix);
				ret += 2;
				break;

			case OPERAND_MEM_AL:
				cat_mem16(una.operand, *(WORD *)(bincode + 1), fix);
				cat_cnm(una.operand);
				strcat(una.operand, str_al);
				ret += 2;
				break;

			case OPERAND_MEM_AX:
				cat_mem16(una.operand, *(WORD *)(bincode + 1), fix);
				cat_cnm(una.operand);
				strcat(una.operand, str_ax);
				ret += 2;
				break;

			case OPERAND_AL_IMM8:
				strcpy(una.operand, str_al);
				cat_cnm(una.operand);
				cat_hex8(&una, bincode[1]);
				ret++;
				break;

			case OPERAND_AX_IMM16:
				strcpy(una.operand, str_ax);
				cat_cnm(una.operand);
				cat_hex16(una.operand, *(WORD *)(bincode + 1));
				ret += 2;
				break;

			case OPERAND_R_IMM8:
				strcpy(una.operand, reg8[c & 7]);
				cat_cnm(una.operand);
				cat_hex8(&una, *(bincode + 1));
				ret++;
				break;

			case OPERAND_R_IMM16:
				strcpy(una.operand, reg16[c & 7]);
				cat_cnm(una.operand);
				cat_hex16(una.operand, *(WORD *)(bincode + 1));
				ret += 2;
				break;

			case OPERAND_IMM8:
				cat_hex8(&una, bincode[1]);
				ret++;
				break;

			case OPERAND_IMM8x:
				cat_hex8x(&una, bincode[1]);
				ret++;
				break;

			case OPERAND_IMM16:
				cat_hex16(una.operand, *(WORD *)(bincode + 1));
				ret += 2;
				break;

			case OPERAND_IMM_SHORT:
				cat_hex16(una.operand,
							adrs + ret + 1 + (int)((char)bincode[1]));
				ret++;
				break;

			case OPERAND_IMM_NEAR:
				cat_hex16(una.operand,
							adrs + ret + 2 + (int)(*(short *)(bincode + 1)));
				ret += 2;
				break;

			case OPERAND_IMM_FAR:
				cat_hex16(una.operand, *(WORD *)(bincode + 3));
				strcat(una.operand, ":");
				cat_hex16(una.operand, *(WORD *)(bincode + 1));
				ret += 4;
				break;

			case MNEMONIC_REG:
				strcpy(una.operand, reg16[c & 7]);
				break;

			case MNEMONIC_AX_REG:
				strcpy(una.operand, str_ax);
				cat_cnm(una.operand);
				strcat(una.operand, reg16[c & 7]);
				break;

			case MNEMONIC_SEG:
				strcpy(una.operand, seg16[(c & 0x18) >> 3]);
				break;

			case OPERAND_IMM8_AL:
				cat_hex8(&una, bincode[1]);
				cat_cnm(una.operand);
				strcat(una.operand, str_al);
				ret++;
				break;

			case OPERAND_IMM16_AL:
				cat_hex16(una.operand, *(WORD *)(bincode + 1));
				cat_cnm(una.operand);
				strcat(una.operand, str_ax);
				ret += 2;
				break;

			case MNEMONIC_DX_AL:
				strcpy(una.operand, "dx,al");
				break;

			case MNEMONIC_DX_AX:
				strcpy(una.operand, "dx,ax");
				break;

			case MNEMONIC_AL_DX:
				strcpy(una.operand, "al,dx");
				break;

			case MNEMONIC_AX_DX:
				strcpy(una.operand, "ax,dx");
				break;

			case OPERAND_IMM16_IMM8:
				cat_hex16(una.operand, *(WORD *)(bincode + 1));
				cat_cnm(una.operand);
				cat_hex8(&una, bincode[3]);
				ret += 3;
				break;

			case OPERAND_IMM_10:
				if (bincode[1] != 10) {
					cat_hex8x(&una, bincode[1]);
				}
				ret++;
				break;

			case OPERAND_3:
				strcpy(una.operand, "03");
				break;

			case MNEMONIC_80:
				una.mnemonic = opcd80[(bincode[1] & 0x38) >> 3];
				pos = cat_ea8b(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				cat_hex8(&una, bincode[pos+1]);
				ret += pos + 1;
				break;

			case MNEMONIC_81:
				una.mnemonic = opcd80[(bincode[1] & 0x38) >> 3];
				pos = cat_ea16w(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				cat_hex16(una.operand, *(WORD *)(bincode + pos + 1));
				ret += pos + 2;
				break;

			case MNEMONIC_83:
				una.mnemonic = opcd80[(bincode[1] & 0x38) >> 3];
				pos = cat_ea16w(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				cat_hexs8x(&una, bincode[pos + 1]);
				ret += pos + 1;
				break;

			case MNEMONIC_C0:
				una.mnemonic = opcdc0[(bincode[1] & 0x38) >> 3];
				pos = cat_ea8b(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				cat_hex8(&una, bincode[pos + 1]);
				ret += pos + 1;
				break;

			case MNEMONIC_C1:
				una.mnemonic = opcdc0[(bincode[1] & 0x38) >> 3];
				pos = cat_ea16w(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				cat_hex8(&una, bincode[pos + 1]);
				ret += pos + 1;
				break;

			case MNEMONIC_D0:
				una.mnemonic = opcdc0[(bincode[1] & 0x38) >> 3];
				ret += cat_ea8b(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				strcat(una.operand, "1");
				break;

			case MNEMONIC_D1:
				una.mnemonic = opcdc0[(bincode[1] & 0x38) >> 3];
				ret += cat_ea16w(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				strcat(una.operand, "1");
				break;

			case MNEMONIC_D2:
				una.mnemonic = opcdc0[(bincode[1] & 0x38) >> 3];
				ret += cat_ea8b(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				strcat(una.operand, str_cl);
				break;

			case MNEMONIC_D3:
				una.mnemonic = opcdc0[(bincode[1] & 0x38) >> 3];
				ret += cat_ea16w(bincode, fix, &una) + 1;
				cat_cnm(una.operand);
				strcat(una.operand, str_cl);
				break;

			case MNEMONIC_F6:
				una.mnemonic = opcdf6[(bincode[1] & 0x38) >> 3];
				pos = cat_ea8b(bincode, fix, &una) + 1;
				ret += pos;
				switch(bincode[1] & 0x38) {
					case 0x00:
					case 0x08:
						cat_cnm(una.operand);
						cat_hex8(&una, bincode[pos + 1]);
						ret++;
						break;
				}
				break;

			case MNEMONIC_F7:
				una.mnemonic = opcdf6[(bincode[1] & 0x38) >> 3];
				pos = cat_ea16w(bincode, fix, &una) + 1;
				ret += pos;
				switch(bincode[1] & 0x38) {
					case 0x00:
					case 0x08:
						cat_cnm(una.operand);
						cat_hex16(una.operand, *(WORD *)(bincode + pos + 1));
						ret += 2;
						break;
				}
				break;

			case MNEMONIC_FE:
				una.mnemonic = opcdfe[(bincode[1] & 0x38) >> 3];
				switch(bincode[1] & 0x38) {
					case 0x00:		// inc
					case 0x08:		// dec
						ret += cat_ea8b(bincode, fix, &una) + 1;
						break;
				}
				break;

			case MNEMONIC_FF:
				una.mnemonic = opcdfe[(bincode[1] & 0x38) >> 3];
				switch(bincode[1] & 0x38) {
					case 0x00:		// inc
					case 0x08:		// dec
						ret += cat_ea16w(bincode, fix, &una) + 1;
						break;

					case 0x18:		// call far
					case 0x28:		// jmp far
						strcat(una.operand, "far ");
					default:
						ret += cat_ea16(bincode, fix, &una) + 1;
						break;
				}
				break;

			case MNEMONIC_0F:
				cat_hex8x(&una, bincode[1]);
				ret++;
				break;

			case MNEMONIC_ESC:
			case MNEMONIC_63:
			case MNEMONIC_64:
			case MNEMONIC_65:
			case MNEMONIC_66:
			case MNEMONIC_67:
				cat_hex8x(&una, c);
				break;

			default:
				ret = 0;
				break;
		}
		if (unasm_t) {
			CopyMemory(unasm_t, &una, sizeof(una));
		}
//		unasm_reset(fix);
	}
	return(ret);
}
