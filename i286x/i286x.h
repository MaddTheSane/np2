
#define I286 __declspec(naked) static void
#define I286EXT __declspec(naked) void

#define		I286IRQCHECKTERM								\
				__asm {	xor		eax, eax				}	\
				__asm { cmp		I286_REMCLOCK, eax		}	\
				__asm {	jle		short nonremainclr		}	\
				__asm { xchg	I286_REMCLOCK, eax		}	\
				__asm {	sub		I286_BASECLOCK, eax		}	\
		nonremainclr:										\
				__asm {	ret								}


#define		I286PREFIX(proc)								\
				__asm {	bts		i286core.s.prefix, 0	}	\
				__asm {	jc		fixed					}	\
				__asm {	mov		REPPOSBAK, esi			}	\
				__asm {	push	offset removeprefix		}	\
			fixed:											\
				GET_NEXTPRE1								\
				__asm {	movzx	eax, bl					}	\
				__asm {	jmp		(proc)[eax*4]			}


extern void __fastcall i286x_localint(void);
extern void removeprefix(void);

extern void (*i286op[])(void);
extern void (*i286op_repne[])(void);
extern void (*i286op_repe[])(void);

extern void (*v30op[])(void);
extern void (*v30op_repne[])(void);
extern void (*v30op_repe[])(void);

void i286xadr_init(void);

