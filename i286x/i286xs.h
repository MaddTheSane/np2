extern void (*op8xreg8_xtable[])(void);
extern void (*op8xext8_xtable[])(void);
extern void (*op8xreg16_xtable[])(void);
extern void (*op8xext16_xtable[])(void);
extern void (*op8xext16_atable[])(void);

extern void (*sftreg8_xtable[])(void);
extern void (*sftmem8_xtable[])(void);
extern void (*sftext8_xtable[])(void);
extern void (*sftreg16_xtable[])(void);
extern void (*sftmem16_xtable[])(void);
extern void (*sftext16_xtable[])(void);

extern void (*sftreg8cl_xtable[])(void);
extern void (*sftext8cl_xtable[])(void);
extern void (*sftreg16cl_xtable[])(void);
extern void (*sftext16cl_xtable[])(void);

extern void (*ope0xf6_xtable[])(void);
extern void (*ope0xf7_xtable[])(void);
extern void (*ope0xfe_xtable[])(void);
extern void (*ope0xff_xtable[])(void);

#if 0
extern void (*sftreg8v30_table[])(void);
extern void (*sftext8v30_table[])(void);
extern void (*sftreg16v30_table[])(void);
extern void (*sftext16v30_table[])(void);
#endif
