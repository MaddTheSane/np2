
typedef struct {
	UINT8	mode[4];
	UINT8	flag[4];
	UINT8	intr[4];
#if 0
	UINT8	stat[4];
#endif
	UINT16	value[4];
	UINT16	latch[4];
} _PIT, *PIT;


#ifdef __cplusplus
extern "C" {
#endif

void systimer_noint(NEVENTITEM item);
void systimer(NEVENTITEM item);
void beeponeshot(NEVENTITEM item);
void rs232ctimer(NEVENTITEM item);

void pit_setflag(int ch, REG8 value);
BOOL pit_setcount(int ch, REG8 value);
REG8 pit_getcount(int ch);

void itimer_reset(void);
void itimer_bind(void);

#ifdef __cplusplus
}
#endif

