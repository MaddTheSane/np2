#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"


extern	OPNCFG	opncfg;

extern	SINT32	env_curve[];				// ver0.27
extern	SINT32	envtable[];
extern	SINT32	sintable[];					// ver0.27


#define SLOTOUT(s, e, c)													\
		((sintable[(((s).freq_cnt + (c)) >> (FREQ_BITS - SIN_BITS)) &		\
			(SIN_ENT-1)] * envtable[(e)]) >> (ENVTBL_BIT+SINTBL_BIT-TL_BITS))

static SINT32 calcrateenvlope(OPNSLOT *slot) {

	/* calcrate phage generator */
	slot->freq_cnt += slot->freq_inc;
	/* calcrate envelope generator */
	slot->env_cnt += slot->env_inc;
	if (slot->env_cnt >= slot->env_end) {
		switch(slot->env_mode) {
			case EM_ATTACK:						// DECAY1 start
				slot->env_mode = EM_DECAY1;
				slot->env_cnt = EC_DECAY;
				slot->env_end = slot->decaylevel;
				slot->env_inc = slot->env_inc_decay1;
				break;

			case EM_DECAY1:						// DECAY2 start
				slot->env_mode = EM_DECAY2;
				slot->env_cnt = slot->decaylevel;
				slot->env_end = EC_OFF;
				slot->env_inc = slot->env_inc_decay2;
				break;

			case EM_RELEASE:					// OFF timing
				slot->env_mode = EM_OFF;

			case EM_DECAY2: 					// DECAY end
				slot->env_cnt = EC_OFF;
				slot->env_end = EC_OFF + 1;
				slot->env_inc = 0;
				break;
		}
	}
	return(slot->totallevel - env_curve[slot->env_cnt >> ENV_BITS]);
}

static void calcratechannel(OPNCH *ch) {

	SINT32	envout;
	SINT32	opout;

	opngen.feedback2 = 0;
	opngen.feedback3 = 0;
	opngen.feedback4 = 0;

	/* SLOT 1 */
	envout = calcrateenvlope(ch->slot + 0);
	if (envout > 0) {
		if (ch->feedback) {
			/* with self feed back */
			opout = ch->op1fb;
			ch->op1fb = SLOTOUT(ch->slot[0], envout,
											(ch->op1fb >> ch->feedback));
			opout = (opout + ch->op1fb) / 2;
		}
		else {
			/* without self feed back */
			opout = SLOTOUT(ch->slot[0], envout, 0);
		}
		/* output slot1 */
		if (!ch->connect1) {
			opngen.feedback2 = opngen.feedback3 = opngen.feedback4 = opout;
		}
		else {
			*ch->connect1 += opout;
		}
	}
	/* SLOT 2 */
	envout = calcrateenvlope(ch->slot + 1);
	if (envout > 0) {
		*ch->connect2 += SLOTOUT(ch->slot[1], envout, opngen.feedback2);
	}
	/* SLOT 3 */
	envout = calcrateenvlope(ch->slot + 2);
	if (envout > 0) {
		*ch->connect3 += SLOTOUT(ch->slot[2], envout, opngen.feedback3);
	}
	/* SLOT 4 */
	envout = calcrateenvlope(ch->slot + 3);
	if (envout > 0) {
		*ch->connect4 += SLOTOUT(ch->slot[3], envout, opngen.feedback4);
	}
}

void SOUNDCALL opngen_getpcm(void *hdl, SINT32 *pcm, UINT count) {

	OPNCH	*fm;
	UINT	i;
	SINT32	samp_l;
	SINT32	samp_r;

	fm = opnch;
	while(count--) {
		samp_l = opngen.outdl * (opngen.calcremain * -1);
		samp_r = opngen.outdr * (opngen.calcremain * -1);
		opngen.calcremain += FMDIV_ENT;
		while(1) {
			opngen.outdc = 0;
			opngen.outdl = 0;
			opngen.outdr = 0;
			for (i=0; i<opngen.playchannels; i++) {
				calcratechannel(fm + i);
			}
			opngen.outdl += opngen.outdc;
			opngen.outdr += opngen.outdc;
			opngen.outdl >>= FMVOL_SFTBIT;
			opngen.outdr >>= FMVOL_SFTBIT;
			if (opngen.calcremain > opncfg.calc1024) {
				samp_l += opngen.outdl * opncfg.calc1024;
				samp_r += opngen.outdr * opncfg.calc1024;
				opngen.calcremain -= opncfg.calc1024;
			}
			else {
				break;
			}
		}
		samp_l += opngen.outdl * opngen.calcremain;
		samp_l >>= 8;
		samp_l *= opncfg.fmvol;
		samp_l >>= (OPM_OUTSB + FMDIV_BITS + 1 + 6 - FMVOL_SFTBIT - 8);
		pcm[0] += samp_l;
		samp_r += opngen.outdr * opngen.calcremain;
		samp_r >>= FMDIV_BITS;
		samp_r *= opncfg.fmvol;
		samp_r >>= (OPM_OUTSB + FMDIV_BITS + 1 + 6 - FMVOL_SFTBIT - 8);
		pcm[1] += samp_r;
		opngen.calcremain -= opncfg.calc1024;
		pcm += 2;
	}
	(void)hdl;
}

