
#define I286FN	static void
#define I286EXT	void

typedef void (*I286OP)(void);
typedef UINT32 (*CALCEA)(void);
typedef UINT16 (*CALCLEA)(void);
typedef UINT16 (*GETLEA)(UINT32 *seg);

extern	BYTE	szpcflag[0x200];

extern void CPUCALL i286_intnum(UINT vect, UINT16 IP);

#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
extern void i286cea_initialize(void);
#endif

extern const I286OP i286op[];
extern const I286OP i286op_repe[];
extern const I286OP i286op_repne[];

#define	I286_0F	static void CPUCALL
typedef void (CPUCALL * I286OP_0F)(UINT op);

I286EXT i286c_cts(void);


#define	I286_8X	static void CPUCALL
typedef void (CPUCALL * I286OP8XREG8)(BYTE *p);
typedef void (CPUCALL * I286OP8XEXT8)(UINT32 madr);
typedef void (CPUCALL * I286OP8XREG16)(UINT16 *p, UINT32 src);
typedef void (CPUCALL * I286OP8XEXT16)(UINT32 madr, UINT32 src);

extern const I286OP8XREG8 c_op8xreg8_table[];
extern const I286OP8XEXT8 c_op8xext8_table[];
extern const I286OP8XREG16 c_op8xreg16_table[];
extern const I286OP8XEXT16 c_op8xext16_table[];


#define	I286_SFT static void CPUCALL
typedef void (CPUCALL * I286OPSFTR8)(BYTE *p);
typedef void (CPUCALL * I286OPSFTE8)(UINT32 madr);
typedef void (CPUCALL * I286OPSFTR16)(UINT16 *p);
typedef void (CPUCALL * I286OPSFTE16)(UINT32 madr);
typedef void (CPUCALL * I286OPSFTR8CL)(BYTE *p, BYTE cl);
typedef void (CPUCALL * I286OPSFTE8CL)(UINT32 madr, BYTE cl);
typedef void (CPUCALL * I286OPSFTR16CL)(UINT16 *p, BYTE cl);
typedef void (CPUCALL * I286OPSFTE16CL)(UINT32 madr, BYTE cl);

extern const I286OPSFTR8 sft_r8_table[];
extern const I286OPSFTE8 sft_e8_table[];
extern const I286OPSFTR16 sft_r16_table[];
extern const I286OPSFTE16 sft_e16_table[];
extern const I286OPSFTR8CL sft_r8cl_table[];
extern const I286OPSFTE8CL sft_e8cl_table[];
extern const I286OPSFTR16CL sft_r16cl_table[];
extern const I286OPSFTE16CL sft_e16cl_table[];


#define	I286_F6 static void CPUCALL
typedef void (CPUCALL * I286OPF6)(UINT op);

extern const I286OPF6 c_ope0xf6_table[];
extern const I286OPF6 c_ope0xf7_table[];


extern const I286OPF6 c_ope0xfe_table[];
extern const I286OPF6 c_ope0xff_table[];


extern I286EXT i286c_rep_insb(void);
extern I286EXT i286c_rep_insw(void);
extern I286EXT i286c_rep_outsb(void);
extern I286EXT i286c_rep_outsw(void);
extern I286EXT i286c_rep_movsb(void);
extern I286EXT i286c_rep_movsw(void);
extern I286EXT i286c_rep_lodsb(void);
extern I286EXT i286c_rep_lodsw(void);
extern I286EXT i286c_rep_stosb(void);
extern I286EXT i286c_rep_stosw(void);
extern I286EXT i286c_repe_cmpsb(void);
extern I286EXT i286c_repne_cmpsb(void);
extern I286EXT i286c_repe_cmpsw(void);
extern I286EXT i286c_repne_cmpsw(void);
extern I286EXT i286c_repe_scasb(void);
extern I286EXT i286c_repne_scasb(void);
extern I286EXT i286c_repe_scasw(void);
extern I286EXT i286c_repne_scasw(void);

