#include	"compiler.h"
#include	<dsound.h>
#include	"parts.h"
#include	"wavefile.h"
#include	"np2.h"
#include	"soundmng.h"
#include	"sound.h"
#if defined(VERMOUTH_LIB)
#include	"vermouth.h"
#endif


#if 1
#define	DSBUFFERDESC_SIZE	20			// DirectX3 Structsize
#else
#define	DSBUFFERDESC_SIZE	sizeof(DSBUFFERDESC)
#endif

#ifndef DSBVOLUME_MAX
#define	DSBVOLUME_MAX		0
#endif
#ifndef DSBVOLUME_MIN
#define	DSBVOLUME_MIN		(-10000)
#endif


static	LPDIRECTSOUND		pDSound;
static	LPDIRECTSOUNDBUFFER	pDSData3;
static	UINT				dsstreambytes;
static	BYTE				dsstreamevent;
static	BYTE				mute;

#if defined(VERMOUTH_LIB)
		MIDIMOD		vermouth_module = NULL;
#endif


// ---- directsound

static BOOL dsoundcreate(void) {

	// DirectSound�̏�����
	if (FAILED(DirectSoundCreate(0, &pDSound, 0))) {
		goto dscre_err;
	}
	if (FAILED(pDSound->SetCooperativeLevel(hWndMain, DSSCL_PRIORITY))) {
		if (FAILED(pDSound->SetCooperativeLevel(hWndMain, DSSCL_NORMAL))) {
			goto dscre_err;
		}
	}
	return(SUCCESS);

dscre_err:
	RELEASE(pDSound);
	return(FAILURE);
}

// ---- stream

UINT soundmng_create(UINT rate, UINT ms) {

	UINT			samples;
	DSBUFFERDESC	dsbdesc;
	PCMWAVEFORMAT	pcmwf;
#if defined(VERMOUTH_LIB)
	UINT			num;
#endif

	if ((pDSound == NULL) ||
		(rate != 11025) && (rate != 22050) && (rate != 44100)) {
		goto stcre_err1;
	}
	if (ms < 50) {
		ms = 50;
	}
	else if (ms > 1000) {
		ms = 1000;
	}

	// �L�[�{�[�h�\���̃f�B���C�ݒ�
//	keydispr_delayinit((BYTE)((ms * 10 + 563) / 564));

	samples = (rate * ms) / 2000;
	samples = (samples + 3) & (~3);
	dsstreambytes = samples * 2 * sizeof(SINT16);

	ZeroMemory(&pcmwf, sizeof(PCMWAVEFORMAT));
	pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
	pcmwf.wf.nChannels = 2;
	pcmwf.wf.nSamplesPerSec = rate;
	pcmwf.wBitsPerSample = 16;
	pcmwf.wf.nBlockAlign = 2 * sizeof(SINT16);
	pcmwf.wf.nAvgBytesPerSec = rate * 2 * sizeof(SINT16);

	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = DSBUFFERDESC_SIZE;
	dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
						DSBCAPS_CTRLFREQUENCY |
						DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;
	dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;
	dsbdesc.dwBufferBytes = dsstreambytes * 2;
	if (FAILED(pDSound->CreateSoundBuffer(&dsbdesc, &pDSData3, NULL))) {
		goto stcre_err2;
	}

#if defined(VERMOUTH_LIB)
	vermouth_module = midimod_create(rate);
	for (num=0; num<128; num++) {
		midimod_loadprogram(vermouth_module, num);
		midimod_loadrhythm(vermouth_module, num);
	}
#endif
	dsstreamevent = (BYTE)-1;
	soundmng_reset();
	return(samples);

stcre_err2:
	RELEASE(pDSData3);

stcre_err1:
	return(0);
}

void soundmng_reset(void) {

	LPBYTE	blockptr1;
	LPBYTE	blockptr2;
	DWORD	blocksize1;
	DWORD	blocksize2;

	if ((pDSData3) &&
		(SUCCEEDED(pDSData3->Lock(0, dsstreambytes * 2,
							(LPVOID *)&blockptr1, &blocksize1,
							(LPVOID *)&blockptr2, &blocksize2, 0)))) {
		ZeroMemory(blockptr1, blocksize1);
		if ((blockptr2 != NULL) && (blocksize2 != 0)) {
			ZeroMemory(blockptr2, blocksize2);
		}
		pDSData3->Unlock(blockptr1, blocksize1, blockptr2, blocksize2);
		pDSData3->SetCurrentPosition(0);
		dsstreamevent = (BYTE)-1;
	}
}

void soundmng_destroy(void) {

	if (pDSData3) {
#if defined(VERMOUTH_LIB)
		midimod_destroy(vermouth_module);
		vermouth_module = NULL;
#endif
		pDSData3->Stop();
		pDSData3->Release();
		pDSData3 = NULL;
	}
}

static void streamenable(BOOL play) {

	if (pDSData3) {
		if (play) {
			pDSData3->Play(0, 0, DSBPLAY_LOOPING);
		}
		else {
			pDSData3->Stop();
		}
	}
}

void soundmng_play(void) {

	if (!mute) {
		streamenable(TRUE);
	}
}

void soundmng_stop(void) {

	if (!mute) {
		streamenable(FALSE);
	}
}

static void streamwrite(DWORD pos) {

const SINT32	*pcm;
	HRESULT		hr;
	LPBYTE		blockptr1;
	LPBYTE		blockptr2;
	DWORD		blocksize1;
	DWORD		blocksize2;

	pcm = sound_pcmlock();
	if ((hr = pDSData3->Lock(pos, dsstreambytes,
								(LPVOID *)&blockptr1, &blocksize1,
								(LPVOID *)&blockptr2, &blocksize2, 0))
													== DSERR_BUFFERLOST) {
		pDSData3->Restore();
		hr = pDSData3->Lock(pos, dsstreambytes,
								(LPVOID *)&blockptr1, &blocksize1,
								(LPVOID *)&blockptr2, &blocksize2, 0);
	}
	if (SUCCEEDED(hr)) {
		if (pcm) {
			satuation_s16((SINT16 *)blockptr1, pcm, blocksize1);
		}
		else {
			ZeroMemory(blockptr1, blocksize1);
		}
		pDSData3->Unlock(blockptr1, blocksize1, blockptr2, blocksize2);
	}
	sound_pcmunlock(pcm);
}

void soundmng_sync(void) {

	DWORD	pos;
	DWORD	wpos;

	if (pDSData3 != NULL) {
		if (pDSData3->GetCurrentPosition(&pos, &wpos) == DS_OK) {
			if (pos >= dsstreambytes) {
				if (dsstreamevent != 0) {
					dsstreamevent = 0;
					streamwrite(0);
				}
			}
			else {
				if (dsstreamevent != 1) {
					dsstreamevent = 1;
					streamwrite(dsstreambytes);
				}
			}
		}
	}
}

void soundmng_setreverse(BOOL reverse) {
}


// ----

BOOL soundmng_initialize(void) {

	if (dsoundcreate() != SUCCESS) {
		goto smcre_err;
	}
	return(SUCCESS);

smcre_err:
	soundmng_destroy();
	return(FAILURE);
}

void soundmng_deinitialize(void) {

	soundmng_destroy();
	RELEASE(pDSound);
}


// ----

void soundmng_enable(void) {

	if (mute) {
		mute = 0;
		soundmng_reset();
		streamenable(TRUE);
	}
}

void soundmng_disable(void) {

	if (!mute) {
		mute = 1;
		streamenable(FALSE);
	}
}

