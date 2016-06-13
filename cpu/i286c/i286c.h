
#define INTR_FAST

// #define	I286C_TEST
#if defined(I286C_TEST)
#undef MEMOPTIMIZE
#endif


#define	I286_STAT		i286core.s.r

#define	I286_REG		i286core.s.r
#define	I286_SEGREG		i286core.s.r.w.es

#define	I286_AX			i286core.s.r.w.ax
#define	I286_BX			i286core.s.r.w.bx
#define	I286_CX			i286core.s.r.w.cx
#define	I286_DX			i286core.s.r.w.dx
#define	I286_SI			i286core.s.r.w.si
#define	I286_DI			i286core.s.r.w.di
#define	I286_BP			i286core.s.r.w.bp
#define	I286_SP			i286core.s.r.w.sp
#define	I286_CS			i286core.s.r.w.cs
#define	I286_DS			i286core.s.r.w.ds
#define	I286_ES			i286core.s.r.w.es
#define	I286_SS			i286core.s.r.w.ss
#define	I286_IP			i286core.s.r.w.ip

#define	SEG_BASE		i286core.s.es_base
#define	ES_BASE			i286core.s.es_base
#define	CS_BASE			i286core.s.cs_base
#define	SS_BASE			i286core.s.ss_base
#define	DS_BASE			i286core.s.ds_base
#define	SS_FIX			i286core.s.ss_fix
#define	DS_FIX			i286core.s.ds_fix

#define	I286_AL			i286core.s.r.b.al
#define	I286_BL			i286core.s.r.b.bl
#define	I286_CL			i286core.s.r.b.cl
#define	I286_DL			i286core.s.r.b.dl
#define	I286_AH			i286core.s.r.b.ah
#define	I286_BH			i286core.s.r.b.bh
#define	I286_CH			i286core.s.r.b.ch
#define	I286_DH			i286core.s.r.b.dh

#define	I286_FLAG		i286core.s.r.w.flag
#define	I286_FLAGL		i286core.s.r.b.flag_l
#define	I286_FLAGH		i286core.s.r.b.flag_h
#define	I286_TRAP		i286core.s.trap
#define	I286_OV			i286core.s.ovflag

#define	I286_GDTR		i286core.s.GDTR
#define	I286_IDTR		i286core.s.IDTR
#define	I286_LDTR		i286core.s.LDTR
#define	I286_LDTRC		i286core.s.LDTRC
#define	I286_TR			i286core.s.TR
#define	I286_TRC		i286core.s.TRC
#define	I286_MSW		i286core.s.MSW

#define	I286_REMCLOCK	i286core.s.remainclock
#define	I286_BASECLOCK	i286core.s.baseclock
#define	I286_CLOCK		i286core.s.clock
#define	I286_ADRSMASK	i286core.s.adrsmask

#define	I286_PREFIX		i286core.s.prefix

#define	I286_INPADRS	i286core.e.inport


#define I286FN	static void
#define I286EXT	void

typedef void (*I286OP)(I286CORE *cpu);

extern void CPUCALL i286c_intnum(UINT vect, REG16 IP);
extern UINT32 i286c_selector(UINT sel);

#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
extern void i286cea_initialize(void);
#endif

extern const I286OP i286op[];
extern const I286OP i286op_repe[];
extern const I286OP i286op_repne[];

#define	I286_0F	static void CPUCALL
typedef void (CPUCALL * I286OP_0F)(I286CORE *cpu, UINT op);

I286EXT i286c_cts(I286CORE *cpu);


#define	I286_8X	static void CPUCALL
typedef void (CPUCALL * I286OP8XREG8)(I286CORE *cpu, UINT8 *p);
typedef void (CPUCALL * I286OP8XEXT8)(I286CORE *cpu, UINT32 madr);
typedef void (CPUCALL * I286OP8XREG16)(I286CORE *cpu, UINT16 *p, UINT32 src);
typedef void (CPUCALL * I286OP8XEXT16)(I286CORE *cpu, UINT32 madr, UINT32 src);

extern const I286OP8XREG8 c_op8xreg8_table[];
extern const I286OP8XEXT8 c_op8xext8_table[];
extern const I286OP8XREG16 c_op8xreg16_table[];
extern const I286OP8XEXT16 c_op8xext16_table[];


#define	I286_SFT static void CPUCALL
typedef void (CPUCALL * I286OPSFTR8)(I286CORE *cpu, UINT8 *p);
typedef void (CPUCALL * I286OPSFTE8)(I286CORE *cpu, UINT32 madr);
typedef void (CPUCALL * I286OPSFTR16)(I286CORE *cpu, UINT16 *p);
typedef void (CPUCALL * I286OPSFTE16)(I286CORE *cpu, UINT32 madr);
typedef void (CPUCALL * I286OPSFTR8CL)(I286CORE *cpu, UINT8 *p, REG8 cl);
typedef void (CPUCALL * I286OPSFTE8CL)(I286CORE *cpu, UINT32 madr, REG8 cl);
typedef void (CPUCALL * I286OPSFTR16CL)(I286CORE *cpu, UINT16 *p, REG8 cl);
typedef void (CPUCALL * I286OPSFTE16CL)(I286CORE *cpu, UINT32 madr, REG8 cl);

extern const I286OPSFTR8 sft_r8_table[];
extern const I286OPSFTE8 sft_e8_table[];
extern const I286OPSFTR16 sft_r16_table[];
extern const I286OPSFTE16 sft_e16_table[];
extern const I286OPSFTR8CL sft_r8cl_table[];
extern const I286OPSFTE8CL sft_e8cl_table[];
extern const I286OPSFTR16CL sft_r16cl_table[];
extern const I286OPSFTE16CL sft_e16cl_table[];


#define	I286_F6 static void CPUCALL
typedef void (CPUCALL * I286OPF6)(I286CORE *cpu, UINT op);

extern const I286OPF6 c_ope0xf6_table[];
extern const I286OPF6 c_ope0xf7_table[];


extern const I286OPF6 c_ope0xfe_table[];
extern const I286OPF6 c_ope0xff_table[];


extern I286EXT i286c_rep_insb(I286CORE *cpu);
extern I286EXT i286c_rep_insw(I286CORE *cpu);
extern I286EXT i286c_rep_outsb(I286CORE *cpu);
extern I286EXT i286c_rep_outsw(I286CORE *cpu);
extern I286EXT i286c_rep_movsb(I286CORE *cpu);
extern I286EXT i286c_rep_movsw(I286CORE *cpu);
extern I286EXT i286c_rep_lodsb(I286CORE *cpu);
extern I286EXT i286c_rep_lodsw(I286CORE *cpu);
extern I286EXT i286c_rep_stosb(I286CORE *cpu);
extern I286EXT i286c_rep_stosw(I286CORE *cpu);
extern I286EXT i286c_repe_cmpsb(I286CORE *cpu);
extern I286EXT i286c_repne_cmpsb(I286CORE *cpu);
extern I286EXT i286c_repe_cmpsw(I286CORE *cpu);
extern I286EXT i286c_repne_cmpsw(I286CORE *cpu);
extern I286EXT i286c_repe_scasb(I286CORE *cpu);
extern I286EXT i286c_repne_scasb(I286CORE *cpu);
extern I286EXT i286c_repe_scasw(I286CORE *cpu);
extern I286EXT i286c_repne_scasw(I286CORE *cpu);


#define i286_memoryread(a)			memp_read8(a)
#define i286_memoryread_w(a)		memp_read16(a)
#define i286_memoryread_d(a)		memp_read32(a)
#define i286_memorywrite(a, v)		memp_write8(a, v)
#define i286_memorywrite_w(a, v)	memp_write16(a, v)
#define i286_memorywrite_d(a, v)	memp_write32(a, v)

