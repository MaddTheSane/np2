
#define I286FN	static void
#define I286_P	static void
#define I286EXT	void

typedef void (*I286OP)(void);
typedef UINT32 (*CALCEA)(void);
typedef UINT16 (*CALCLEA)(void);
typedef UINT16 (*GETLEA)(void);

extern	DWORD	EA_FIX;
extern	BYTE	*reg8_b53[256];
extern	BYTE	*reg8_b20[256];
extern	UINT16	*reg16_b53[256];
extern	UINT16	*reg16_b20[256];
extern	BYTE	szpcflag[0x200];
extern	BYTE	szpflag_w[0x10000];
extern	CALCEA	c_calc_ea_dst[];
extern	CALCLEA	c_calc_lea[];
extern	GETLEA	c_get_ea[];

extern void CPUCALL i286_intnum(DWORD vect, WORD IP);


extern const I286OP i286op[];
extern const I286OP i286op_repe[];
extern const I286OP i286op_repne[];


extern const CALCEA i286c_ea_dst_tbl[];
extern const CALCLEA i286c_lea_tbl[];
extern const GETLEA i286c_ea_tbl[];


#define	I286_0F	static void CPUCALL
typedef void (CPUCALL * I286OP_0F)(DWORD op);

I286EXT i286c_cts(void);


#define	I286_8X	static void CPUCALL
typedef void (CPUCALL * I286OP8XREG8)(BYTE *p);
typedef void (CPUCALL * I286OP8XEXT8)(DWORD madr);
typedef void (CPUCALL * I286OP8XREG16)(WORD *p, DWORD src);
typedef void (CPUCALL * I286OP8XEXT16)(DWORD madr, DWORD src);

extern const I286OP8XREG8 c_op8xreg8_table[];
extern const I286OP8XEXT8 c_op8xext8_table[];
extern const I286OP8XREG16 c_op8xreg16_table[];
extern const I286OP8XEXT16 c_op8xext16_table[];


#define	I286_SFT static void CPUCALL
typedef void (CPUCALL * I286OPSFTR8)(BYTE *p);
typedef void (CPUCALL * I286OPSFTE8)(DWORD madr);
typedef void (CPUCALL * I286OPSFTR16)(WORD *p);
typedef void (CPUCALL * I286OPSFTE16)(DWORD madr);
typedef void (CPUCALL * I286OPSFTR8CL)(BYTE *p, BYTE cl);
typedef void (CPUCALL * I286OPSFTE8CL)(DWORD madr, BYTE cl);
typedef void (CPUCALL * I286OPSFTR16CL)(WORD *p, BYTE cl);
typedef void (CPUCALL * I286OPSFTE16CL)(DWORD madr, BYTE cl);

extern const I286OPSFTR8 sft_r8_table[];
extern const I286OPSFTE8 sft_e8_table[];
extern const I286OPSFTR16 sft_r16_table[];
extern const I286OPSFTE16 sft_e16_table[];
extern const I286OPSFTR8CL sft_r8cl_table[];
extern const I286OPSFTE8CL sft_e8cl_table[];
extern const I286OPSFTR16CL sft_r16cl_table[];
extern const I286OPSFTE16CL sft_e16cl_table[];


#define	I286_F6 static void CPUCALL
typedef void (CPUCALL * I286OPF6)(DWORD madr);

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

