
typedef struct {
	UINT16	timera;
	BYTE	timerb;
	BYTE	status;
	BYTE	reg;
	BYTE	intr;
	BYTE	irq;
	BYTE	intdisabel;
} _FMTIMER, *FMTIMER;


#ifdef __cplusplus
extern "C" {
#endif

void fmport_a(NEVENTITEM item);
void fmport_b(NEVENTITEM item);

void fmtimer_reset(BYTE irq);
void fmtimer_setreg(BYTE reg, BYTE value);

#ifdef __cplusplus
}
#endif

