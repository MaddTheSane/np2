// palette
//	 00		text palette
//	+0A		skipline palette
//	+10		grph palette
//	+10		text palette
//	+09		black + text palette
//	+80		end...

enum {
	NP2PALS_TXT		= 10,
	NP2PALS_GRPH	= 16,

	NP2PAL_TEXT		= 0,
	NP2PAL_SKIP		= (NP2PALS_TXT),
	NP2PAL_GRPH		= (NP2PAL_SKIP + NP2PALS_GRPH),
	NP2PAL_TEXT2	= (NP2PAL_GRPH + NP2PALS_GRPH),
	NP2PAL_TEXT3	= (NP2PAL_TEXT2 + (8 * NP2PALS_GRPH)),

						// 8bit color‚Å‚Í TEXT3‚ÍŽg‚í‚È‚¢”¤
	NP2PAL_TOTAL	= NP2PAL_TEXT3
};

#define	PALEVENTMAX		1024

typedef struct {
	SINT32	clock;
	UINT16	color;
	BYTE	value;
	BYTE	reserve;
} PAL1EVENT;

typedef struct {
	UINT16		anabit;
	UINT16		degbit;
	RGB32		pal[16];
	UINT		events;
	PAL1EVENT	event[PALEVENTMAX];
} PALEVENT;


#ifdef __cplusplus
extern "C" {
#endif

extern	RGB32		np2_pal32[256];
#if defined(SUPPORT_16BPP)
extern	UINT16		np2_pal16[256];
#endif
extern	PALEVENT	palevent;
extern	BYTE		pal_monotable[16];

void pal_makegrad(RGB32 *pal, int pals, UINT32 bg, UINT32 fg);

void pal_initlcdtable(void);
void pal_makelcdpal(void);
void pal_makeskiptable(void);
void pal_change(BYTE textpalset);

void pal_eventclear(void);

void pal_makeanalog(RGB32 *pal, UINT16 bit);
void pal_makeanalog_lcd(RGB32 *pal, UINT16 bit);

#ifdef __cplusplus
}
#endif

