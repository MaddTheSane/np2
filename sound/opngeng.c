#include	"compiler.h"
#include	"sound.h"
#include	"opngen.h"


extern	SINT32	env_curve[];				// ver0.27
extern	SINT32	envtable[];
extern	SINT32	sintable[];					// ver0.27

// opn work
extern	UINT	playchannels;
extern	SINT32	feedback2;
extern	SINT32	feedback3;
extern	SINT32	feedback4;
extern	SINT32	outdl;
extern	SINT32	outdc;
extern	SINT32	outdr;
extern	SINT32	calc1024;
extern	SINT32	calcremain;
extern	SINT32	fmvolforasm;
extern	SINT32	fmvolforc;


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

	feedback2 = 0;
	feedback3 = 0;
	feedback4 = 0;

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
			feedback2 = feedback3 = feedback4 = opout;
		}
		else {
			*ch->connect1 += opout;
		}
	}
	/* SLOT 2 */
	envout = calcrateenvlope(ch->slot + 1);
	if (envout > 0) {
		*ch->connect2 += SLOTOUT(ch->slot[1], envout, feedback2);
	}
	/* SLOT 3 */
	envout = calcrateenvlope(ch->slot + 2);
	if (envout > 0) {
		*ch->connect3 += SLOTOUT(ch->slot[2], envout, feedback3);
	}
	/* SLOT 4 */
	envout = calcrateenvlope(ch->slot + 3);
	if (envout > 0) {
		*ch->connect4 += SLOTOUT(ch->slot[3], envout, feedback4);
	}
}

void SOUNDCALL opngen_getpcm(void *hdl, SINT32 *pcm, UINT count) {

	OPNCH	*opn;
	UINT	i;
	SINT32	samp_l;
	SINT32	samp_r;

	opn = opnch;
	while(count--) {
		samp_l = outdl * (calcremain * -1);
		samp_r = outdr * (calcremain * -1);
		calcremain += FMDIV_ENT;
		while(1) {
			outdc = 0;
			outdl = 0;
			outdr = 0;
			for (i=0; i<playchannels; i++) {
				calcratechannel(opn + i);
			}
			outdl += outdc;
			outdr += outdc;
			outdl >>= FMVOL_SFTBIT;
			outdr >>= FMVOL_SFTBIT;
			if (calcremain > calc1024) {
				samp_l += outdl * calc1024;
				samp_r += outdr * calc1024;
				calcremain -= calc1024;
			}
			else {
				break;
			}
		}
		samp_l += outdl * calcremain;
		samp_l >>= 8;
		samp_l *= fmvolforc;
		samp_l >>= (OPM_OUTSB + FMDIV_BITS + 1 + 6 - FMVOL_SFTBIT - 8);
		pcm[0] += samp_l;
		samp_r += outdr * calcremain;
		samp_r >>= FMDIV_BITS;
		samp_r *= fmvolforc;
		samp_r >>= (OPM_OUTSB + FMDIV_BITS + 1 + 6 - FMVOL_SFTBIT - 8);
		pcm[1] += samp_r;
		calcremain -= calc1024;
		pcm += 2;
	}
	(void)hdl;
}

