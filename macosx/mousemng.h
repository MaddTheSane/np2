
#if defined(NP2GCC)

#define	MOUSE_MASK		7

#define	M_RES		0x00
#define	M_XOR		0x40
#define	M_SET		0x80


#define	MOUSE_OFF		(M_RES | 0)
#define MOUSE_ON		(M_SET | 0)
#define	MOUSE_XOR		(M_XOR | 0)
#define	MOUSE_CONT		(M_RES | 1)
#define	MOUSE_STOP		(M_SET | 1)
#define	MOUSE_CONT_M	(M_RES | 2)
#define	MOUSE_STOP_M	(M_SET | 2)


#define	MOUSE_LEFTDOWN	0
#define	MOUSE_LEFTUP	1
#define	MOUSE_RIGHTDOWN	2
#define	MOUSE_RIGHTUP	3


#ifdef __cplusplus
extern "C" {
#endif

BYTE mousemng_getstat(short *x, short *y, int clear);

#ifdef __cplusplus
}
#endif

BYTE mouse_flag(void);
void mouse_running(BYTE flg);
void mouse_callback(HIPoint delta);
BYTE mouse_btn(BYTE btn);

#else
#ifdef __cplusplus
extern "C" {
#endif

BYTE mousemng_getstat(SINT16 *x, SINT16 *y, int clear);

#ifdef __cplusplus
}
#endif

#endif
