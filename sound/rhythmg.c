#include	"compiler.h"
#include	"sound.h"
#include	"rhythm.h"


void SOUNDCALL rhythm_getpcm(RHYTHM rhy, SINT32 *pcm, UINT count) {

	UINT		bit;
	RHYTHMCH	*r;
	UINT		rem;
const SINT16	*src;
	SINT32		*dst;
	SINT32		samp;

	if ((rhy->bitmap == 0) || (count == 0)) {
		return;
	}
	bit = 1;
	r = rhy->r;
	do {
		if (rhy->bitmap & bit) {
			rem = r->remain;
			if (rem > count) {
				rem = count;
				r->remain -= count;
			}
			else {
				rhy->bitmap ^= bit;
				r->remain = 0;
			}
			src = r->ptr;
			dst = pcm;
			do {
				samp = *src++;
				samp *= r->volume;
				samp /= (1 << 12);
				if (r->lr & 0x80) {
					dst[0] += samp;
				}
				if (r->lr & 0x40) {
					dst[1] += samp;
				}
				dst += 2;
			} while(--rem);
			r->ptr = src;
		}
		r++;
		bit <<= 1;
	} while(bit & 0x3f);
}

