#include	"compiler.h"
#include	"sound.h"
#include	"psggen.h"


extern	PSGGENCFG	psggencfg;


void SOUNDCALL psggen_getpcm(PSGGEN psg, SINT32 *pcm, UINT count) {

	SINT32	noisevol;
	BYTE	mixer;
	UINT	noisetbl = 0;
	PSGTONE	*tone;
	PSGTONE	*toneterm;
	SINT32	samp;
	UINT	psgvol;
	SINT32	vol;
	UINT	i;
	UINT	noise;

	while(count--) {
		noisevol = 0;
		if (psg->envcnt) {
			psg->envcnt--;
			if (psg->envcnt == 0) {
				psg->envvolcnt--;
				if (psg->envvolcnt < 0) {
					if (psg->envmode & PSGENV_ONESHOT) {
						psg->envvol = (psg->envmode & PSGENV_LASTON)?15:0;
					}
					else {
						psg->envvolcnt = 15;
						if (!(psg->envmode & PSGENV_ONECYCLE)) {
							psg->envmode ^= PSGENV_INC;
						}
						psg->envcnt = psg->envmax;
						psg->envvol = (psg->envvolcnt ^ psg->envmode) & 0x0f;
					}
				}
				else {
					psg->envcnt = psg->envmax;
					psg->envvol = (psg->envvolcnt ^ psg->envmode) & 0x0f;
				}
			}
		}
		mixer = psg->mixer;
		if (mixer & 0x38) {
			for (i=0; i<(1 << PSGADDEDBIT); i++) {
				SINT32 countbak;
				countbak = psg->noise.count;
				psg->noise.count -= psg->noise.freq;
				if (psg->noise.count > countbak) {
					psg->noise.base = rand() & (1 << (1 << PSGADDEDBIT));
				}
				noisetbl += psg->noise.base;
				noisetbl >>= 1;
			}
		}
		tone = psg->tone;
		toneterm = tone + 3;
		samp = 0;
		do {
			psgvol = (*(tone->pvol)) & 15;
			if (psgvol) {
				vol = psggencfg.volume[psgvol];
				switch(mixer & 9) {
					case 0:							// no mix
						if (tone->puchi) {
							tone->puchi--;
							samp += vol << PSGADDEDBIT;
						}
						break;

					case 1:							// tone only
						for (i=0; i<(1 << PSGADDEDBIT); i++) {
							tone->count += tone->freq;
							samp += vol * ((tone->count>=0)?1:-1);
						}
						break;

					case 8:							// noise only
						noise = noisetbl;
						for (i=0; i<(1 << PSGADDEDBIT); i++) {
							samp += vol * ((noise & 1)?1:-1);
							noise >>= 1;
						}
						break;

					case 9:
						noise = noisetbl;
						for (i=0; i<(1 << PSGADDEDBIT); i++) {
							tone->count += tone->freq;
							if ((tone->count >= 0) || (noise & 1)) {
								samp += vol;
							}
							else {
								samp -= vol;
							}
							noise >>= 1;
						}
						break;
				}
			}
			mixer >>= 1;
		} while(++tone < toneterm);
		pcm[0] += samp;
		pcm[1] += samp;
		pcm += 2;
	}
}

