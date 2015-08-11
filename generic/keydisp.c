/**
 * @file	keydisp.c
 * @brief	Implementation of the key display
 */

#include "compiler.h"

#if defined(SUPPORT_KEYDISP)

#include "pccore.h"
#include "iocore.h"
#include "sound.h"
#include "fmboard.h"
#include "keydisp.h"

typedef struct {
	UINT8	k[KEYDISP_NOTEMAX];
	UINT8	r[KEYDISP_NOTEMAX];
	UINT	remain;
	UINT8	flag;
	UINT8	padding[3];
} KDCHANNEL;

typedef struct {
	UINT8	ch;
	UINT8	key;
} KDDELAYE;

typedef struct {
	UINT	pos;
	UINT	rem;
	UINT8	warm;
	UINT8	warmbase;
} KDDELAY;

typedef struct {
	UINT16	fnum[4];
	UINT8	lastnote[4];
	UINT8	flag;
	UINT8	extflag;
} KDFMCTRL;

typedef struct {
	UINT16	fto[4];
	UINT8	lastnote[4];
	UINT8	flag;
	UINT8	mix;
	UINT8	padding[2];
} KDPSGCTRL;

typedef struct {
	UINT8		mode;
	UINT8		dispflag;
	UINT8		framepast;
	UINT8		keymax;
	UINT8		fmmax;
	UINT8		psgmax;
	UINT8		fmpos[KEYDISP_FMCHMAX];
	UINT8		psgpos[KEYDISP_PSGMAX];
	const UINT8	*pfmreg[KEYDISP_FMCHMAX];
	KDDELAY		delay;
	KDCHANNEL	ch[KEYDISP_CHMAX];
	KDFMCTRL	fmctl[KEYDISP_FMCHMAX];
	KDPSGCTRL	psgctl[KEYDISP_PSGMAX];
	KDDELAYE	delaye[KEYDISP_DELAYEVENTS];
} KEYDISP;

static	KEYDISP		s_keydisp;

/**
 * @brief The table of the notes
 */
struct TagNotePattern
{
	UINT16 nPosX;			/*!< X-Coorinate */
	UINT8 nType;			/*!< type */
	const UINT8 *lpImage;	/*!< image */
};
typedef struct TagNotePattern NOTEPATTERN;		/*!< The define of the note's pattern */

/**
 * @brief const data
 */
struct KeyDispConstData
{
	UINT8 pal8[KEYDISP_PALS];			/*!< 8npp palettes */
	UINT16 pal16[2][KEYDISP_LEVEL];		/*!< 16bpp palettes */
	RGB32 pal32[2][KEYDISP_LEVEL];		/*!< 32bpp palettes */
	NOTEPATTERN pattern[128];			/*!< pattern */
};

/*! const data */
static struct KeyDispConstData s_constData;

#include "keydisp.res"


// ---- event

static void keyon(KEYDISP *keydisp, UINT ch, UINT8 note)
{
	UINT		i;
	KDCHANNEL	*kdch;

	note &= 0x7f;
	kdch = keydisp->ch + ch;
	for (i = 0; i < kdch->remain; i++)
	{
		if (kdch->k[i] == note)
		{
			/* ƒqƒbƒg‚µ‚½ */
			for (; i < (kdch->remain - 1); i++)
			{
				kdch->k[i] = kdch->k[i + 1];
				kdch->r[i] = kdch->r[i + 1];
			}
			kdch->k[i] = note;
			kdch->r[i] = 0x80 | (KEYDISP_LEVEL - 1);
			kdch->flag |= 1;
			return;
		}
	}
	if (i < KEYDISP_NOTEMAX)
	{
		kdch->k[i] = note;
		kdch->r[i] = 0x80 | (KEYDISP_LEVEL - 1);
		kdch->flag |= 1;
		kdch->remain++;
	}
}

static void keyoff(KEYDISP *keydisp, UINT ch, UINT8 note)
{
	UINT		i;
	KDCHANNEL	*kdch;

	note &= 0x7f;
	kdch = keydisp->ch + ch;
	for (i = 0; i < kdch->remain; i++)
	{
		if (kdch->k[i] == note)
		{
			/* ƒqƒbƒg‚µ‚½ */
			kdch->r[i] = 0x80 | (KEYDISP_LEVEL - 2);
			kdch->flag |= 1;
			break;
		}
	}
}

static void chkeyoff(KEYDISP *keydisp, UINT ch)
{
	UINT		i;
	KDCHANNEL	*kdch;

	kdch = keydisp->ch + ch;
	for (i = 0; i < kdch->remain; i++)
	{
		if ((kdch->r[i] & (~0x80)) >= (KEYDISP_LEVEL - 1))
		{
			kdch->r[i] = 0x80 | (KEYDISP_LEVEL - 2);
			kdch->flag |= 1;
		}
	}
}

static void keyalloff(KEYDISP *keydisp)
{
	UINT i;

	for (i = 0; i < KEYDISP_CHMAX; i++)
	{
		chkeyoff(keydisp, i);
	}
}

static void keyallreload(KEYDISP *keydisp)
{
	UINT i;

	for (i = 0; i < KEYDISP_CHMAX; i++)
	{
		keydisp->ch[i].flag = 2;
	}
}

static void keyallclear(KEYDISP *keydisp)
{
	memset(keydisp->ch, 0, sizeof(keydisp->ch));
	keyallreload(keydisp);
}


// ---- delay event

static void delayreset(KEYDISP *keydisp)
{
	keydisp->delay.warm = keydisp->delay.warmbase;
	keydisp->delay.pos = 0;
	keydisp->delay.rem = 0;
	memset(keydisp->delaye, 0, sizeof(keydisp->delaye));
	keyalloff(keydisp);
}

static void delayexecevent(KEYDISP *keydisp, UINT8 framepast)
{
	KDDELAYE	*ebase;
	UINT		pos;
	UINT		rem;
	KDDELAYE	*ev;

	ebase = keydisp->delaye;
	pos = keydisp->delay.pos;
	rem = keydisp->delay.rem;
	while ((keydisp->delay.warm) && (framepast))
	{
		keydisp->delay.warm--;
		framepast--;
		if (rem >= KEYDISP_DELAYEVENTS)
		{
			ev = ebase + pos;
			rem--;
			if (ev->ch == 0xff)
			{
				keydisp->delay.warm++;
			}
			else if (ev->key & 0x80)
			{
				keyon(keydisp, ev->ch, ev->key);
				rem--;
			}
			else
			{
				keyoff(keydisp, ev->ch, ev->key);
			}
			pos = (pos + 1) & (KEYDISP_DELAYEVENTS - 1);
		}
		ebase[(pos + rem) & (KEYDISP_DELAYEVENTS - 1)].ch = 0xff;
		rem++;
	}
	while (framepast)
	{
		framepast--;
		while (rem)
		{
			rem--;
			ev = ebase + pos;
			if (ev->ch == 0xff)
			{
				pos = (pos + 1) & (KEYDISP_DELAYEVENTS - 1);
				break;
			}
			if (ev->key & 0x80)
			{
				keyon(keydisp, ev->ch, ev->key);
			}
			else
			{
				keyoff(keydisp, ev->ch, ev->key);
			}
			pos = (pos + 1) & (KEYDISP_DELAYEVENTS - 1);
		}
		ebase[(pos + rem) & (KEYDISP_DELAYEVENTS - 1)].ch = 0xff;
		rem++;
	}
	keydisp->delay.pos = pos;
	keydisp->delay.rem = rem;
}

static void delaysetevent(KEYDISP *keydisp, UINT8 ch, UINT8 key)
{
	KDDELAYE	*e;

	e = keydisp->delaye;
	if (keydisp->delay.rem < KEYDISP_DELAYEVENTS)
	{
		e += (keydisp->delay.pos + keydisp->delay.rem) & (KEYDISP_DELAYEVENTS - 1);
		keydisp->delay.rem++;
		e->ch = ch;
		e->key = key;
	}
	else
	{
		e += keydisp->delay.pos;
		keydisp->delay.pos = (keydisp->delay.pos + 1) & (KEYDISP_DELAYEVENTS - 1);
		if (e->ch == 0xff)
		{
			keydisp->delay.warm++;
		}
		else if (e->key & 0x80)
		{
			keyon(keydisp, e->ch, e->key);
		}
		else
		{
			keyoff(keydisp, e->ch, e->key);
		}
		e->ch = ch;
		e->key = key;
	}
}


// ---- FM

static UINT8 getfmnote(UINT16 fnum)
{
	UINT8	ret;
	int		i;

	ret = (fnum >> 11) & 7;
	ret *= 12;
	ret += 24;
	fnum &= 0x7ff;

	while (fnum < FNUM_MIN)
	{
		if (!ret)
		{
			return 0;
		}
		ret -= 12;
		fnum <<= 1;
	}
	while (fnum > FNUM_MAX)
	{
		fnum >>= 1;
		ret += 12;
	}
	for (i = 0; fnum >= fnumtbl[i]; i++)
	{
		ret++;
	}
	return min(ret, 127);
}

static void fmkeyoff(KEYDISP *keydisp, UINT8 ch, KDFMCTRL *k)
{
	delaysetevent(keydisp, keydisp->fmpos[ch], k->lastnote[0]);
}

static void fmkeyon(KEYDISP *keydisp, UINT8 ch, KDFMCTRL *k)
{
	const UINT8 *pReg;

	fmkeyoff(keydisp, ch, k);
	pReg = keydisp->pfmreg[ch];
	if (pReg)
	{
		pReg = pReg + 0xa0;
		k->fnum[0] = ((pReg[4] & 0x3f) << 8) + pReg[0];
		k->lastnote[0] = getfmnote(k->fnum[0]);
		delaysetevent(keydisp, keydisp->fmpos[ch], (UINT8)(k->lastnote[0] | 0x80));
	}
}

static void fmkeyreset(KEYDISP *keydisp)
{
	memset(keydisp->fmctl, 0, sizeof(keydisp->fmctl));
}

void keydisp_fmkeyon(UINT8 ch, UINT8 value)
{
	KDFMCTRL	*k;

	if (s_keydisp.mode != KEYDISP_MODEFM)
	{
		return;
	}
	if (ch < s_keydisp.fmmax)
	{
		k = s_keydisp.fmctl + ch;
		value &= 0xf0;
		if (k->flag != value)
		{
			if (value)
			{
				fmkeyon(&s_keydisp, ch, k);
			}
			else
			{
				fmkeyoff(&s_keydisp, ch, k);
			}
			k->flag = value;
		}
	}
}

static void fmkeysync(KEYDISP *keydisp)
{
	UINT8		ch;
	KDFMCTRL	*k;
	const UINT8 *pReg;
	UINT16		fnum;

	for (ch = 0, k = keydisp->fmctl; ch < keydisp->fmmax; ch++, k++)
	{
		if (k->flag)
		{
			pReg = keydisp->pfmreg[ch];
			if (pReg)
			{
				pReg = pReg + 0xa0;
				fnum = ((pReg[4] & 0x3f) << 8) + pReg[0];
				if (k->fnum[0] != fnum)
				{
					UINT8 n;
					k->fnum[0] = fnum;
					n = getfmnote(fnum);
					if (k->lastnote[0] != n)
					{
						fmkeyoff(keydisp, ch, k);
					}
					k->lastnote[0] = n;
					delaysetevent(keydisp, keydisp->fmpos[ch], (UINT8)(k->lastnote[0] | 0x80));
				}
			}
		}
	}
}


// ---- PSG

static const void *psgtbl[3] = {&g_psg1, &g_psg2, &g_psg3};

static UINT8 getpsgnote(UINT16 tone)
{
	UINT8	ret;
	int		i;

	ret = 60;
	tone &= 0xfff;

	while (tone < FTO_MIN)
	{
		tone <<= 1;
		ret += 12;
		if (ret > 127)
		{
			return 127;
		}
	}
	while (tone > FTO_MAX)
	{
		if (!ret)
		{
			return 0;
		}
		tone >>= 1;
		ret -= 12;
	}
	for (i = 0; tone < ftotbl[i]; i++)
	{
		ret++;
	}
	return min(ret, 127);
}

static void psgmix(KEYDISP *keydisp, UINT8 ch, PSGGEN psg)
{
	KDPSGCTRL	*k;

	k = keydisp->psgctl + ch;
	if ((k->mix ^ psg->reg.mixer) & 7)
	{
		UINT8 i, bit, pos;
		k->mix = psg->reg.mixer;
		pos = keydisp->psgpos[ch];
		for (i = 0, bit = 1; i < 3; i++, pos++, bit <<= 1)
		{
			if (k->flag & bit)
			{
				k->flag ^= bit;
				delaysetevent(keydisp, pos, k->lastnote[i]);
			}
			else if ((!(k->mix & bit)) && (psg->reg.vol[i] & 0x1f))
			{
				k->flag |= bit;
				k->fto[i] = LOADINTELWORD(psg->reg.tune[i]) & 0xfff;
				k->lastnote[i] = getpsgnote(k->fto[i]);
				delaysetevent(keydisp, pos, (UINT8)(k->lastnote[i] | 0x80));
			}
		}
	}
}

static void psgvol(KEYDISP *keydisp, UINT8 ch, PSGGEN psg, UINT8 i)
{
	KDPSGCTRL	*k;
	UINT8		bit;
	UINT8		pos;
	UINT16		tune;

	k = keydisp->psgctl + ch;
	bit = (1 << i);
	pos = keydisp->psgpos[ch] + i;
	if (psg->reg.vol[i] & 0x1f)
	{
		if (!((k->mix | k->flag) & bit))
		{
			k->flag |= bit;
			tune = LOADINTELWORD(psg->reg.tune[i]);
			tune &= 0xfff;
			k->fto[i] = tune;
			k->lastnote[i] = getpsgnote(tune);
			delaysetevent(keydisp, pos, (UINT8)(k->lastnote[i] | 0x80));
		}
	}
	else if (k->flag & bit)
	{
		k->flag ^= bit;
		delaysetevent(keydisp, pos, k->lastnote[i]);
	}
}

static void psgkeyreset(KEYDISP *keydisp)
{
	memset(keydisp->psgctl, 0, sizeof(keydisp->psgctl));
}

void keydisp_psgmix(void *psg)
{
	UINT8	c;

	if (s_keydisp.mode != KEYDISP_MODEFM)
	{
		return;
	}
	for (c = 0; c < s_keydisp.psgmax; c++)
	{
		if (psgtbl[c] == psg)
		{
			psgmix(&s_keydisp, c, (PSGGEN)psg);
			break;
		}
	}
}

void keydisp_psgvol(void *psg, UINT8 ch)
{
	UINT8	c;

	if (s_keydisp.mode != KEYDISP_MODEFM)
	{
		return;
	}
	for (c = 0; c < s_keydisp.psgmax; c++)
	{
		if (psgtbl[c] == psg)
		{
			psgvol(&s_keydisp, c, (PSGGEN)psg, ch);
			break;
		}
	}
}

static void psgkeysync(KEYDISP *keydisp)
{
	UINT8		ch;
	KDPSGCTRL	*k;
	UINT8		bit;
	UINT8		i;
	UINT8		pos;
	PSGGEN		psg;
	UINT16		tune;
	UINT8		n;

	for (ch = 0, k = keydisp->psgctl; ch < keydisp->psgmax; ch++, k++)
	{
		psg = (PSGGEN)psgtbl[ch];
		pos = keydisp->psgpos[ch];
		for (i = 0, bit = 1; i < 3; i++, pos++, bit <<= 1)
		{
			if (k->flag & bit)
			{
				tune = LOADINTELWORD(psg->reg.tune[i]);
				tune &= 0xfff;
				if (k->fto[i] != tune)
				{
					k->fto[i] = tune;
					n = getpsgnote(tune);
					if (k->lastnote[i] != n)
					{
						delaysetevent(keydisp, pos, k->lastnote[i]);
						k->lastnote[i] = n;
						delaysetevent(keydisp, pos, (UINT8)(n | 0x80));
					}
				}
			}
		}
	}
}


// ---- BOARD change...

static void setfmhdl(KEYDISP *keydisp, const OPN_T *pOpn, UINT nItems, UINT nBase)
{
	while (nItems--)
	{
		if ((keydisp->keymax < KEYDISP_CHMAX) && (keydisp->fmmax < KEYDISP_FMCHMAX))
		{
			keydisp->fmpos[keydisp->fmmax] = keydisp->keymax++;
			keydisp->pfmreg[keydisp->fmmax] = pOpn->reg + nBase;
			keydisp->fmmax++;
			nBase++;
			if ((nBase & 3) == 3)
			{
				nBase += 0x100 - 3;
			}
		}
	}
}

static void setpsghdl(KEYDISP *keydisp, UINT8 items)
{
	while (items--)
	{
		if ((keydisp->keymax <= (KEYDISP_CHMAX - 3)) && (keydisp->psgmax < KEYDISP_PSGMAX))
		{
			keydisp->psgpos[keydisp->psgmax++] = keydisp->keymax;
			keydisp->keymax += 3;
		}
	}
}

void keydisp_setfmboard(UINT b)
{
	s_keydisp.keymax = 0;
	s_keydisp.fmmax = 0;
	s_keydisp.psgmax = 0;

#if defined(SUPPORT_PX)
	if (b == 0x30)
	{
		setfmhdl(&s_keydisp, &g_opn, 12, 0);
		setfmhdl(&s_keydisp, &g_opn2, 12, 0);
		setpsghdl(&s_keydisp, 2);
		b = 0;
	}
	if (b == 0x50)
	{
		setfmhdl(&s_keydisp, &g_opn, 12, 0);
		setfmhdl(&s_keydisp, &g_opn2, 12, 0);
		setfmhdl(&s_keydisp, &g_opn3, 6, 0);
		setpsghdl(&s_keydisp, 3);
		b = 0;
	}

#endif	// defined(SUPPORT_PX)

	if (b & 0x02)
	{
		if (!(b & 0x04))
		{
			setfmhdl(&s_keydisp, &g_opn, 3, 0);
		}
		else
		{
			/* ‚Q–‡Žh‚µ‚ÌŽžƒŒƒWƒXƒ^ˆÚ“® */
			setfmhdl(&s_keydisp, &g_opn, 3, 0x200);
		}
		setpsghdl(&s_keydisp, 1);
	}
	if (b & 0x04)
	{
		setfmhdl(&s_keydisp, &g_opn, 6, 0);
		setpsghdl(&s_keydisp, 1);
	}
	if (b & 0x08)
	{
		setfmhdl(&s_keydisp, &g_opn, 6, 0);
		setpsghdl(&s_keydisp, 1);
	}
	if (b & 0x20)
	{
		setfmhdl(&s_keydisp, &g_opn, 6, 0);
		setpsghdl(&s_keydisp, 1);
	}
	if (b & 0x40)
	{
		setfmhdl(&s_keydisp, &g_opn, 12, 0);
		setpsghdl(&s_keydisp, 1);
	}
	if (b & 0x80)
	{
		setpsghdl(&s_keydisp, 3);
	}
	delayreset(&s_keydisp);
	fmkeyreset(&s_keydisp);
	psgkeyreset(&s_keydisp);

	if (s_keydisp.mode == KEYDISP_MODEFM)
	{
		s_keydisp.dispflag |= KEYDISP_FLAGSIZING;
	}
}


// ---- MIDI

void keydisp_midi(const UINT8 *cmd)
{
	if (s_keydisp.mode != KEYDISP_MODEMIDI)
	{
		return;
	}
	switch (cmd[0] & 0xf0)
	{
		case 0x80:
			keyoff(&s_keydisp, cmd[0] & 0x0f, cmd[1]);
			break;

		case 0x90:
			if (cmd[2] & 0x7f)
			{
				keyon(&s_keydisp, cmd[0] & 0x0f, cmd[1]);
			}
			else
			{
				keyoff(&s_keydisp, cmd[0] & 0x0f, cmd[1]);
			}
			break;

		case 0xb0:
			if ((cmd[1] == 0x78) || (cmd[1] == 0x79) || (cmd[1] == 0x7b))
			{
				chkeyoff(&s_keydisp, cmd[0] & 0x0f);
			}
			break;

		case 0xfe:
			keyalloff(&s_keydisp);
			break;
	}
}


// ---- draw

static UINT getdispkeys(const KEYDISP *keydisp)
{
	UINT keys;

	switch (keydisp->mode)
	{
		case KEYDISP_MODEFM:
			keys = keydisp->keymax;
			break;

		case KEYDISP_MODEMIDI:
			keys = 16;
			break;

		default:
			keys = 0;
			break;
	}
	return min(keys, KEYDISP_CHMAX);
}

static void clearrect(CMNVRAM *vram, int x, int y, int cx, int cy)
{
	CMNPAL col;

	switch (vram->bpp)
	{
#if defined(SUPPORT_8BPP)
		case 8:
			col.pal8 = s_constData.pal8[KEYDISP_PALBG];
			break;
#endif
#if defined(SUPPORT_16BPP)
		case 16:
			col.pal16 = s_constData.pal16[1][0];
			break;
#endif
#if defined(SUPPORT_24BPP) || defined(SUPPORT_32BPP)
		case 24:
		case 32:
			col.pal32 = s_constData.pal32[1][0];
			break;
#endif
		default:
			return;
	}
	cmndraw_fill(vram, x, y, cx, cy, col);
}

static void drawkeybg(CMNVRAM *vram)
{
	CMNPAL	bg;
	CMNPAL	fg;
	int		i;

	switch (vram->bpp)
	{
#if defined(SUPPORT_8BPP)
		case 8:
			bg.pal8 = s_constData.pal8[KEYDISP_PALBG];
			fg.pal8 = s_constData.pal8[KEYDISP_PALFG];
			break;
#endif
#if defined(SUPPORT_16BPP)
		case 16:
			bg.pal16 = s_constData.pal16[1][0];
			fg.pal16 = s_constData.pal16[0][0];
			break;
#endif
#if defined(SUPPORT_24BPP) || defined(SUPPORT_32BPP)
		case 24:
		case 32:
			bg.pal32 = s_constData.pal32[1][0];
			fg.pal32 = s_constData.pal32[0][0];
			break;
#endif
		default:
			return;
	}
	for (i = 0; i < 10; i++)
	{
		cmndraw_setpat(vram, keybrd1, i * KEYDISP_KEYCX, 0, bg, fg);
	}
	cmndraw_setpat(vram, keybrd2, 10 * KEYDISP_KEYCX, 0, bg, fg);
}

static BOOL draw1key(CMNVRAM *vram, KDCHANNEL *kdch, UINT n)
{
	const NOTEPATTERN *pPattern;
	UINT		pal;
	CMNPAL		fg;

	pPattern = s_constData.pattern + (kdch->k[n] & 0x7f);
	pal = kdch->r[n] & 0x7f;
	switch (vram->bpp)
	{
#if defined(SUPPORT_8BPP)
		case 8:
			if (pal != (KEYDISP_LEVEL - 1))
			{
				fg.pal8 = s_constData.pal8[(pPattern->nType) ? KEYDISP_PALBG : KEYDISP_PALFG];
				cmndraw_setfg(vram, pPattern->lpImage, pPattern->nPosX, 0, fg);
				kdch->r[n] = 0;
				return TRUE;
			}
			fg.pal8 = s_constData.pal8[KEYDISP_PALHIT];
			break;
#endif
#if defined(SUPPORT_16BPP)
		case 16:
			fg.pal16 = s_constData.pal16[pPattern->nType][pal];
			break;
#endif
#if defined(SUPPORT_24BPP) || defined(SUPPORT_32BPP)
		case 24:
		case 32:
			fg.pal32 = s_constData.pal32[pPattern->nType][pal];
			break;
#endif
		default:
			return FALSE;
	}
	cmndraw_setfg(vram, pPattern->lpImage, pPattern->nPosX, 0, fg);
	return FALSE;
}

static BOOL draw1ch(CMNVRAM *vram, UINT8 framepast, KDCHANNEL *kdch)
{
	BOOL	draw;
	UINT	i;
	BOOL	coll;
	UINT8	nextf;
	UINT	j;

	draw = FALSE;
	if (kdch->flag & 2)
	{
		drawkeybg(vram);
		draw = TRUE;
	}
	if (kdch->flag)
	{
		coll = FALSE;
		nextf = 0;
		for (i = 0; i < kdch->remain; i++)
		{
			if ((kdch->r[i] & 0x80) || (kdch->flag & 2))
			{
				kdch->r[i] &= ~0x80;
				if (kdch->r[i] < (KEYDISP_LEVEL - 1))
				{
					if (kdch->r[i] > framepast)
					{
						kdch->r[i] -= framepast;
						kdch->r[i] |= 0x80;
						nextf = 1;
					}
					else
					{
						kdch->r[i] = 0;
						coll = TRUE;
					}
				}
				coll |= draw1key(vram, kdch, i);
				draw = TRUE;
			}
		}
		if (coll)
		{
			for (i = 0; i < kdch->remain; i++)
			{
				if (!kdch->r[i])
				{
					break;
				}
			}
			for (j = i; i < kdch->remain; i++)
			{
				if (kdch->r[i])
				{
					kdch->k[j] = kdch->k[i];
					kdch->r[j] = kdch->r[i];
					j++;
				}
			}
			kdch->remain = j;
		}
		kdch->flag = nextf;
	}
	return draw;
}


// ----

void keydisp_initialize(void)
{
	int		r;
	UINT16	x;
	int		i;

	r = 0;
	x = 0;
	do
	{
		for (i = 0; i < 12 && r < 128; i++, r++)
		{
			s_constData.pattern[r].nPosX = s_notepattern[i].nPosX + x;
			s_constData.pattern[r].nType = s_notepattern[i].nType;
			s_constData.pattern[r].lpImage = s_notepattern[i].lpImage;
		}
		x += 28;
	} while (r < 128);
	keyallclear(&s_keydisp);
}

void keydisp_setpal(CMNPALFN *palfn)
{
	UINT i;
	RGB32 pal32[KEYDISP_PALS];

	if (palfn == NULL)
	{
		return;
	}
	if (palfn->get8)
	{
		for (i = 0; i < KEYDISP_PALS; i++)
		{
			s_constData.pal8[i] = (*palfn->get8)(palfn, i);
		}
	}
	if (palfn->get32)
	{
		for (i = 0; i < KEYDISP_PALS; i++)
		{
			pal32[i].d = (*palfn->get32)(palfn, i);
			cmndraw_makegrad(s_constData.pal32[0], KEYDISP_LEVEL, pal32[KEYDISP_PALFG], pal32[KEYDISP_PALHIT]);
			cmndraw_makegrad(s_constData.pal32[1], KEYDISP_LEVEL, pal32[KEYDISP_PALBG], pal32[KEYDISP_PALHIT]);
		}
		if (palfn->cnv16)
		{
			for (i = 0; i < KEYDISP_LEVEL; i++)
			{
				s_constData.pal16[0][i] = (*palfn->cnv16)(palfn, s_constData.pal32[0][i]);
				s_constData.pal16[1][i] = (*palfn->cnv16)(palfn, s_constData.pal32[1][i]);
			}
		}
	}
	s_keydisp.dispflag |= KEYDISP_FLAGREDRAW;
}

void keydisp_setmode(UINT8 mode)
{
	if (s_keydisp.mode != mode)
	{
		s_keydisp.mode = mode;
		s_keydisp.dispflag |= KEYDISP_FLAGREDRAW | KEYDISP_FLAGSIZING;
		keyallclear(&s_keydisp);
		if (mode == KEYDISP_MODEFM)
		{
			delayreset(&s_keydisp);
			fmkeyreset(&s_keydisp);
			psgkeyreset(&s_keydisp);
		}
	}
	else
	{
		keyalloff(&s_keydisp);
	}
}

void keydisp_setdelay(UINT8 frames)
{
	s_keydisp.delay.warmbase = frames;
	delayreset(&s_keydisp);
}

UINT8 keydisp_process(UINT8 framepast)
{
	UINT	keys;
	UINT	i;

	if (framepast)
	{
		if (s_keydisp.mode == KEYDISP_MODEFM)
		{
			fmkeysync(&s_keydisp);
			psgkeysync(&s_keydisp);
			delayexecevent(&s_keydisp, framepast);
		}
		s_keydisp.framepast += framepast;
	}

	keys = getdispkeys(&s_keydisp);
	for (i = 0; i < keys; i++)
	{
		if (s_keydisp.ch[i].flag)
		{
			s_keydisp.dispflag |= KEYDISP_FLAGDRAW;
			break;
		}
	}
	return s_keydisp.dispflag;
}

void keydisp_getsize(int *width, int *height)
{
	if (width)
	{
		*width = KEYDISP_WIDTH;
	}
	if (height)
	{
		*height = (getdispkeys(&s_keydisp) * KEYDISP_KEYCY) + 1;
	}
	s_keydisp.dispflag &= ~KEYDISP_FLAGSIZING;
}

BOOL keydisp_paint(CMNVRAM *vram, BOOL redraw)
{
	BOOL		draw;
	UINT		keys;
	UINT		i;
	KDCHANNEL	*p;

	draw = FALSE;
	if ((vram == NULL) || (vram->width < KEYDISP_WIDTH) || (vram->height <= 0))
	{
		goto kdpnt_exit;
	}
	if (s_keydisp.dispflag & KEYDISP_FLAGREDRAW)
	{
		redraw = TRUE;
	}
	if (redraw)
	{
		keyallreload(&s_keydisp);
		clearrect(vram, 0, 0, KEYDISP_WIDTH, 1);
		clearrect(vram, 0, 0, 1, vram->height);
		draw = TRUE;
	}
	vram->ptr += vram->xalign + vram->yalign;		// ptr (1, 1)
	keys = (vram->height - 1) / KEYDISP_KEYCY;
	keys = min(keys, getdispkeys(&s_keydisp));
	for (i = 0, p = s_keydisp.ch; i < keys; i++, p++)
	{
		draw |= draw1ch(vram, s_keydisp.framepast, p);
		vram->ptr += KEYDISP_KEYCY * vram->yalign;
	}
	s_keydisp.dispflag &= ~(KEYDISP_FLAGDRAW | KEYDISP_FLAGREDRAW);
	s_keydisp.framepast = 0;

kdpnt_exit:
	return draw;
}
#endif
