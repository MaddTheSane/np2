
typedef struct {
	BYTE	mode[4];
	BYTE	flag[4];
	BYTE	intr[4];
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

void itimer_setflag(int ch, BYTE value);
BOOL itimer_setcount(int ch, BYTE value);
BYTE itimer_getcount(int ch);

void itimer_reset(void);
void itimer_bind(void);

#ifdef __cplusplus
}
#endif

