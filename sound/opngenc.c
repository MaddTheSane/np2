#include	"compiler.h"
#include	<math.h>
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"keydisp.h"


#define	OPM_ARRATE		 399128L
#define	OPM_DRRATE		5514396L

#define	EG_STEP	(96.0 / EVC_ENT)					// dB step
#define	SC(db)	(SINT32)((db) * ((3.0 / EG_STEP) * (1 << ENV_BITS))) + EC_DECAY
#define	D2(v)	(((double)(6 << KF_BITS) * log((double)(v)) / log(2.0)) + 0.5)
#define	FMASMSHIFT	(32 - 6 - (OPM_OUTSB + 1 + FMDIV_BITS) + FMVOL_SFTBIT)
#define	FREQBASE4096	((double)OPNA_CLOCK / calcrate / 64)


	OPNCFG	opncfg;
	SINT32	env_curve[EVC_ENT*2 + 1];
	SINT32	envtable[EVC_ENT];
	SINT32	sintable[SIN_ENT];
#ifdef OPNGENX86
	char	envshift[EVC_ENT];
	char	sinshift[SIN_ENT];
#endif


static	SINT32	detunetable[8][32];
static	SINT32	attacktable[94];
static	SINT32	decaytable[94];

static const SINT32	decayleveltable[16] = {
		 			SC( 0),SC( 1),SC( 2),SC( 3),SC( 4),SC( 5),SC( 6),SC( 7),
		 			SC( 8),SC( 9),SC(10),SC(11),SC(12),SC(13),SC(14),SC(31)};
static const BYTE multipletable[] = {
			    	1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
static const SINT32 nulltable[] = {
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const BYTE kftable[16] = {0,0,0,0,0,0,0,1,2,3,3,3,3,3,3,3};
static const BYTE dttable[] = {
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
					2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8, 8,
					1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5,
					5, 6, 6, 7, 8, 8, 9,10,11,12,13,14,16,16,16,16,
					2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7,
					8, 8, 9,10,11,12,13,14,16,17,19,20,22,22,22,22};
static const int extendslot[4] = {2, 3, 1, 0};
static const int fmslot[4] = {0, 2, 1, 3};


void opngen_initialize(UINT rate) {

	int		i;
	int		j;
	double	pom;
	double	freq;
	UINT32	calcrate;

	calcrate = rate * 55466 / 44100;

	opncfg.calc1024 = FMDIV_ENT * 44100 / 55466;

	for (i=0; i<EVC_ENT; i++) {
#ifdef OPNGENX86
		char sft;
		sft = ENVTBL_BIT;
		while(sft < (ENVTBL_BIT + 8)) {
			pom = (double)(1 << sft) / pow(10.0, EG_STEP*(EVC_ENT-i)/20.0);
			envtable[i] = (long)pom;
			envshift[i] = sft - TL_BITS;
			if (envtable[i] >= (1 << (ENVTBL_BIT - 1))) {
				break;
			}
			sft++;
		}
#else
		pom = (double)(1 << ENVTBL_BIT) / pow(10.0, EG_STEP*(EVC_ENT-i)/20.0);
		envtable[i] = (long)pom;
#endif
	}
	for (i=0; i<SIN_ENT; i++) {
#ifdef OPNGENX86
		char sft;
		sft = SINTBL_BIT;
		while(sft < (SINTBL_BIT + 8)) {
			pom = (double)(1 << sft) * sin(2*PI*i/SIN_ENT);
			sintable[i] = (long)pom;
			sinshift[i] = sft;
			if (sintable[i] >= (1 << (SINTBL_BIT - 1))) {
				break;
			}
			if (sintable[i] <= -1 * (1 << (SINTBL_BIT - 1))) {
				break;
			}
			sft++;
		}
#else
		pom = (double)((1 << SINTBL_BIT) - 1) * sin(2*PI*i/SIN_ENT);
		sintable[i] = (long)pom;							// ver0.27
#endif
	}
	for (i=0; i<EVC_ENT; i++) {
		pom = pow(((double)(EVC_ENT-1-i)/EVC_ENT), 8) * EVC_ENT;
		env_curve[i] = (long)pom;							// ver0.27
		env_curve[EVC_ENT + i] = i;
	}
	env_curve[EVC_ENT*2] = EVC_ENT;

//	opmbaserate = (1L << FREQ_BITS) / (rate * x / 44100) * 55466;
//	�ł����� x == 55466������c

//	������ FREQ_BITS >= 16������
	if (rate == 44100) {
		opncfg.ratebit = 0 + (FREQ_BITS - 16);
	}
	else if (rate == 22050) {
		opncfg.ratebit = 1 + (FREQ_BITS - 16);
	}
	else {
		opncfg.ratebit = 2 + (FREQ_BITS - 16);
	}

	for (i=0; i<4; i++) {
		for (j=0; j<32; j++) {
#if (FREQ_BITS >= 21)
			freq = FREQBASE4096 * dttable[i*32 + j] *
											(1 << (FREQ_BITS-21));
#else
			freq = FREQBASE4096 * dttable[i*32 + j] /
											(1 << (21-FREQ_BITS));
#endif
			detunetable[i][j]   = (long)freq;
			detunetable[i+4][j] = (long)-freq;
		}
	}
	for (i=0; i<4; i++) {
		attacktable[i] = decaytable[i] = 0;
	}
	for (i=4; i<64; i++) {
		freq = (double)(EVC_ENT << ENV_BITS) * FREQBASE4096;
		if (i < 8) {							// �Y��Ă܂��B
			freq *= 1.0 + (i & 2) * 0.25;
		}
		else if (i < 60) {
			freq *= 1.0 + (i & 3) * 0.25;
		}
		freq *= (double)(1 << ((i >> 2) - 1));
		attacktable[i] = (long)(freq / OPM_ARRATE);
		decaytable[i] = (long)(freq / OPM_DRRATE);
	}
	attacktable[62] = EC_DECAY - 1;
	attacktable[63] = EC_DECAY - 1;
	for (i=64; i<94; i++) {
		attacktable[i] = attacktable[63];
		decaytable[i] = decaytable[63];
	}
}

void opngen_setvol(UINT vol) {

	opncfg.fmvol = vol * 5 / 4;
#if defined(OPNGENX86)
	opncfg.fmvol <<= FMASMSHIFT;
#endif
}

void opngen_setVR(BYTE channel, BYTE value) {

	if ((channel & 3) && (value)) {
		opncfg.vr_en = TRUE;
		opncfg.vr_l = (channel & 1)?value:0;
		opncfg.vr_r = (channel & 2)?value:0;
	}
	else {
		opncfg.vr_en = FALSE;
	}
}


// ----

static void set_algorithm(OPNCH *ch) {

	SINT32	*outd;

	outd = &opngen.outdc;
	if (ch->stereo) {
		switch(ch->pan & 0xc0) {
			case 0x80:
				outd = &opngen.outdl;
				break;

			case 0x40:
				outd = &opngen.outdr;
				break;
		}
	}
	switch(ch->algorithm) {
		case 0:
			ch->connect1 = &opngen.feedback2;
			ch->connect2 = &opngen.feedback3;
			ch->connect3 = &opngen.feedback4;
			break;

		case 1:
			ch->connect1 = &opngen.feedback3;
			ch->connect2 = &opngen.feedback3;
			ch->connect3 = &opngen.feedback4;
			break;

		case 2:
			ch->connect1 = &opngen.feedback4;
			ch->connect2 = &opngen.feedback3;
			ch->connect3 = &opngen.feedback4;
			break;

		case 3:
			ch->connect1 = &opngen.feedback2;
			ch->connect2 = &opngen.feedback4;
			ch->connect3 = &opngen.feedback4;
			break;

		case 4:
			ch->connect1 = &opngen.feedback2;
			ch->connect2 = outd;
			ch->connect3 = &opngen.feedback4;
			break;

		case 5:
			ch->connect1 = 0;
			ch->connect2 = outd;
			ch->connect3 = outd;
			break;

		case 6:
			ch->connect1 = &opngen.feedback2;
			ch->connect2 = outd;
			ch->connect3 = outd;
			break;

		case 7:
		default:
			ch->connect1 = outd;
			ch->connect2 = outd;
			ch->connect3 = outd;
	}
	ch->connect4 = outd;
}

static void set_dt1_mul(OPNSLOT *slot, BYTE value) {

	slot->multiple = (SINT32)multipletable[value & 0x0f];
	slot->detune1 = detunetable[(value >> 4) & 7];
}

static void set_tl(OPNSLOT *slot, BYTE value) {

	slot->totallevel = ((~value) & 0x007f) << (EVC_BITS - 7);
}

static void set_ks_ar(OPNSLOT *slot, BYTE value) {

	slot->keyscale = (BYTE)(((~value)>>6)&3);
	value &= 0x1f;
	slot->attack = (value)?(attacktable + (value << 1)):nulltable;
	slot->env_inc_attack = slot->attack[slot->envraito];
	if (slot->env_mode == EM_ATTACK) {
		slot->env_inc = slot->env_inc_attack;
	}
}

static void set_d1r(OPNSLOT *slot, BYTE value) {

	value &= 0x1f;
	slot->decay1 = (value)?(decaytable + (value << 1)):nulltable;
	slot->env_inc_decay1 = slot->decay1[slot->envraito];
	if (slot->env_mode == EM_DECAY1) {
		slot->env_inc = slot->env_inc_decay1;
	}
}

static void set_dt2_d2r(OPNSLOT *slot, BYTE value) {

	value &= 0x1f;
	slot->decay2 = (value)?(decaytable + (value << 1)):nulltable;
	if (slot->ssgeg1) {
		slot->env_inc_decay2 = 0;
	}
	else {
		slot->env_inc_decay2 = slot->decay2[slot->envraito];
	}
	if (slot->env_mode == EM_DECAY2) {
		slot->env_inc = slot->env_inc_decay2;
	}
}

static void set_d1l_rr(OPNSLOT *slot, BYTE value) {

	slot->decaylevel = decayleveltable[(value >> 4)];
	slot->release = decaytable + ((value & 0x0f) << 2) + 2;
	slot->env_inc_release = slot->release[slot->envraito];
	if (slot->env_mode == EM_RELEASE) {
		slot->env_inc = slot->env_inc_release;
		if (value == 0xff) {
			slot->env_mode = EM_OFF;
			slot->env_cnt = EC_OFF;
			slot->env_end = EC_OFF+1;
			slot->env_inc = 0;
		}
	}
}

static void set_ssgeg(OPNSLOT *slot, BYTE value) {

	value &= 0xf;
	if ((value == 0xb) || (value == 0xd)) {
		slot->ssgeg1 = 1;
		slot->env_inc_decay2 = 0;
	}
	else {
		slot->ssgeg1 = 0;
		slot->env_inc_decay2 = slot->decay2[slot->envraito];
	}
	if (slot->env_mode == EM_DECAY2) {
		slot->env_inc = slot->env_inc_decay2;
	}
}

static void channleupdate(OPNCH *ch) {

	int		i;
	UINT32	fc = ch->keynote[0];						// ver0.27
	BYTE	kc = ch->kcode[0];
	BYTE	evr;
	OPNSLOT	*slot;
	int		s;

	slot = ch->slot;
	if (!(ch->extop)) {
		for (i=0; i<4; i++, slot++) {
			slot->freq_inc = (fc + slot->detune1[kc]) * slot->multiple;
			evr = (BYTE)(kc >> slot->keyscale);
			if (slot->envraito != evr) {
				slot->envraito = evr;
				slot->env_inc_attack = slot->attack[evr];
				slot->env_inc_decay1 = slot->decay1[evr];
				slot->env_inc_decay2 = slot->decay2[evr];
				slot->env_inc_release = slot->release[evr];
			}
		}
	}
	else {
		for (i=0; i<4; i++, slot++) {
			s = extendslot[i];
			slot->freq_inc = (ch->keynote[s] + slot->detune1[ch->kcode[s]])
														* slot->multiple;
			evr = (BYTE)(ch->kcode[s] >> slot->keyscale);
			if (slot->envraito != evr) {
				slot->envraito = evr;
				slot->env_inc_attack = slot->attack[evr];
				slot->env_inc_decay1 = slot->decay1[evr];
				slot->env_inc_decay2 = slot->decay2[evr];
				slot->env_inc_release = slot->release[evr];
			}
		}
	}
}


// ----

void opngen_reset(void) {

	OPNCH	*ch;
	UINT	i;
	OPNSLOT	*slot;
	UINT	j;

	ZeroMemory(&opngen, sizeof(opngen));
	ZeroMemory(opnch, sizeof(opnch));
	opngen.playchannels = 3;

	ch = opnch;
	for (i=0; i<OPNCH_MAX; i++) {
		ch->keynote[0] = 0;
		slot = ch->slot;
		for (j=0; j<4; j++) {
			slot->env_mode = EM_OFF;
			slot->env_cnt = EC_OFF;
			slot->env_end = EC_OFF + 1;
			slot->env_inc = 0;
			slot->detune1 = detunetable[0];
			slot->attack = nulltable;
			slot->decay1 = nulltable;
			slot->decay2 = nulltable;
			slot->release = decaytable;
			slot++;
		}
		ch++;
	}
	for (i=0x30; i<0xc0; i++) {
		opngen_setreg((BYTE)i, 0xff, 0);
		opngen_setreg((BYTE)i, 0xff, 3);
		opngen_setreg((BYTE)i, 0xff, 6);
		opngen_setreg((BYTE)i, 0xff, 9);
	}
}

void opngen_setcfg(BYTE maxch, UINT flag) {

	OPNCH	*ch;
	UINT	i;

	opngen.playchannels = maxch;
	ch = opnch;
	if ((flag & OPN_CHMASK) == OPN_STEREO) {
		for (i=0; i<OPNCH_MAX; i++) {
			if (flag & (1 << i)) {
				ch->stereo = TRUE;
				set_algorithm(ch);
			}
			ch++;
		}
	}
	else {
		for (i=0; i<OPNCH_MAX; i++) {
			if (flag & (1 << i)) {
				ch->stereo = FALSE;
				set_algorithm(ch);
			}
			ch++;
		}
	}
}

void opngen_setextch(UINT chnum, BYTE data) {

	OPNCH	*ch;

	ch = opnch;
	ch[chnum].extop = data;
}

void opngen_setreg(BYTE chbase, BYTE reg, BYTE value) {

	UINT	chpos;
	OPNCH	*ch;
	OPNSLOT	*slot;
	UINT	fn;
	BYTE	blk;

	chpos = reg & 3;
	if (chpos == 3) {
		return;
	}
	sound_sync();
	ch = opnch + chbase + chpos;
	if (reg < 0xa0) {
		slot = ch->slot + fmslot[(reg >> 2) & 3];
		switch(reg & 0xf0) {
			case 0x30:					// DT1 MUL
				set_dt1_mul(slot, value);
				channleupdate(ch);
				break;

			case 0x40:					// TL
				set_tl(slot, value);
				break;

			case 0x50:					// KS AR
				set_ks_ar(slot, value);
				channleupdate(ch);
				break;

			case 0x60:					// D1R
				set_d1r(slot, value);
				break;

			case 0x70:					// DT2 D2R
				set_dt2_d2r(slot, value);
				channleupdate(ch);
				break;

			case 0x80:					// D1L RR
				set_d1l_rr(slot, value);
				break;

			case 0x90:
				set_ssgeg(slot, value);
				channleupdate(ch);
				break;
		}
	}
	else {
		switch(reg & 0xfc) {
			case 0xa0:
				blk = ch->keyfunc[0] >> 3;
				fn = ((ch->keyfunc[0] & 7) << 8) + value;
				ch->kcode[0] = (blk << 2) | kftable[fn >> 7];
//				ch->keynote[0] = fn * opmbaserate / (1L << (22-blk));
				ch->keynote[0] = (fn << (opncfg.ratebit + blk)) >> 6;
				channleupdate(ch);
				break;

			case 0xa4:
				ch->keyfunc[0] = value & 0x3f;
				break;

			case 0xa8:
				ch = opnch + chbase + 2;
				blk = ch->keyfunc[chpos+1] >> 3;
				fn = ((ch->keyfunc[chpos+1] & 7) << 8) + value;
				ch->kcode[chpos+1] = (blk << 2) | kftable[fn >> 7];
//				ch->keynote[chpos+1] = fn * opmbaserate / (1L << (22-blk));
				ch->keynote[chpos+1] = (fn << (opncfg.ratebit + blk)) >> 6;
				channleupdate(ch);
				break;

			case 0xac:
				ch = opnch + chbase + 2;
				ch->keyfunc[chpos+1] = value & 0x3f;
				break;

			case 0xb0:
				ch->algorithm = (BYTE)(value & 7);
				value = (value >> 3) & 7;
				if (value) {
					ch->feedback = 8 - value;
				}
				else {
					ch->feedback = 0;
				}
				set_algorithm(ch);
				break;

			case 0xb4:
				ch->pan = (BYTE)(value & 0xc0);
				set_algorithm(ch);
				break;
		}
	}
}

void opngen_keyon(UINT chnum, BYTE value) {

	OPNCH	*ch;
	OPNSLOT	*slot;
	BYTE	bit;
	UINT	i;

	sound_sync();
	opngen.keyreg[chnum] = value;
	ch = opnch + chnum;
	slot = ch->slot;
	bit = 0x10;
	for (i=0; i<4; i++) {
		if (value & bit) {							// keyon
			if (slot->env_mode <= EM_RELEASE) {
				slot->freq_cnt = 0;
				if (i == OPNSLOT1) {
					ch->op1fb = 0;
				}
				slot->env_mode = EM_ATTACK;
				slot->env_inc = slot->env_inc_attack;
				slot->env_cnt = EC_ATTACK;
				slot->env_end = EC_DECAY;
			}
		}
		else {										// keyoff
			if (slot->env_mode > EM_RELEASE) {
				slot->env_mode = EM_RELEASE;
				if (!(slot->env_cnt & EC_DECAY)) {
					slot->env_cnt = (env_curve[slot->env_cnt >> ENV_BITS]
													<< ENV_BITS) + EC_DECAY;
				}
				slot->env_end = EC_OFF;
				slot->env_inc = slot->env_inc_release;
			}
		}
		slot++;
		bit <<= 1;
	}
	keydisp_fmkeyon((BYTE)chnum, value);
}

