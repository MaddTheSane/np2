#include	"compiler.h"
#include	<dsound.h>
#include	"parts.h"
#include	"wavefile.h"
#include	"np2.h"
#include	"soundmng.h"
#include	"extromio.h"
#include	"sound.h"
#if defined(VERMOUTH_LIB)
#include	"vermouth.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
void __fastcall satuation_s16mmx(SINT16 *dst, const SINT32 *src, UINT size);
#ifdef __cplusplus
}
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
static	LPDIRECTSOUNDBUFFER pDSwave3[SOUND_MAXPCM];
static	UINT				mute;
static	void				(PARTSCALL *fnmix)(SINT16 *dst,
												const SINT32 *src, UINT size);

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

	if ((pDSound == NULL) ||
		(rate != 11025) && (rate != 22050) && (rate != 44100)) {
		goto stcre_err1;
	}
	if (ms < 40) {
		ms = 40;
	}
	else if (ms > 1000) {
		ms = 1000;
	}

	// �L�[�{�[�h�\���̃f�B���C�ݒ�
//	keydispr_delayinit((BYTE)((ms * 10 + 563) / 564));

	samples = (rate * ms) / 2000;
	samples = (samples + 3) & (~3);
	dsstreambytes = samples * 2 * sizeof(SINT16);
	soundmng_setreverse(FALSE);

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
	midimod_loadall(vermouth_module);
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
			(*fnmix)((SINT16 *)blockptr1, pcm, blocksize1);
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

	if (!reverse) {
		if (mmxflag) {
			fnmix = satuation_s16;
		}
		else {
			fnmix = satuation_s16mmx;
		}
	}
	else {
		fnmix = satuation_s16x;
	}
}


// ---- pcm

static void pcmcreate(void) {

	UINT	i;

	for (i=0; i<SOUND_MAXPCM; i++) {
		pDSwave3[i] = NULL;
	}
}

static void pcmdestroy(void) {

	UINT				i;
	LPDIRECTSOUNDBUFFER	dsbuf;

	for (i=0; i<SOUND_MAXPCM; i++) {
		dsbuf = pDSwave3[i];
		pDSwave3[i] = NULL;
		if (dsbuf) {
			dsbuf->Stop();
			dsbuf->Release();
		}
	}
}

static void pcmstop(void) {

	UINT	i;

	for (i=0; i<SOUND_MAXPCM; i++) {
		if (pDSwave3[i]) {
			pDSwave3[i]->Stop();
		}
	}
}

void soundmng_pcmload(UINT num, const char *filename, UINT type) {

	EXTROMH				erh;
	RIFF_HEADER			riff;
	BOOL				head;
	WAVE_HEADER			whead;
	UINT				size;
	WAVE_INFOS			info;
	PCMWAVEFORMAT		pcmwf;
	DSBUFFERDESC		dsbdesc;
	LPDIRECTSOUNDBUFFER dsbuf;
	HRESULT				hr;
	LPBYTE				blockptr1;
	LPBYTE				blockptr2;
	DWORD				blocksize1;
	DWORD				blocksize2;

	if ((pDSound == NULL) || (num >= SOUND_MAXPCM)) {
		goto smpl_err1;
	}
	erh = extromio_open(filename, type);
	if (erh == NULL) {
		goto smpl_err1;
	}
	if ((extromio_read(erh, &riff, sizeof(riff)) != sizeof(riff)) ||
		(riff.sig != WAVE_SIG('R','I','F','F')) ||
		(riff.fmt != WAVE_SIG('W','A','V','E'))) {
		goto smpl_err2;
	}

	head = FALSE;
	while(1) {
		if (extromio_read(erh, &whead, sizeof(whead)) != sizeof(whead)) {
			goto smpl_err2;
		}
		size = LOADINTELDWORD(whead.size);
		if (whead.sig == WAVE_SIG('f','m','t',' ')) {
			if (size >= sizeof(info)) {
				if (extromio_read(erh, &info, sizeof(info)) != sizeof(info)) {
					goto smpl_err2;
				}
				size -= sizeof(info);
				head = TRUE;
			}
		}
		else if (whead.sig == WAVE_SIG('d','a','t','a')) {
			break;
		}
		if (size) {
			extromio_seek(erh, size, ERSEEK_CUR);
		}
	}
	if (!head) {
		goto smpl_err2;
	}
	ZeroMemory(&pcmwf, sizeof(PCMWAVEFORMAT));
	pcmwf.wf.wFormatTag = LOADINTELWORD(info.format);
	if (pcmwf.wf.wFormatTag != 1) {
		goto smpl_err2;
	}
	pcmwf.wf.nChannels = LOADINTELWORD(info.channel);
	pcmwf.wf.nSamplesPerSec = LOADINTELDWORD(info.rate);
	pcmwf.wBitsPerSample = LOADINTELWORD(info.bit);
	pcmwf.wf.nBlockAlign = LOADINTELWORD(info.block);
	pcmwf.wf.nAvgBytesPerSec = LOADINTELDWORD(info.rps);

	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = DSBUFFERDESC_SIZE;
	dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
						DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC | 
						DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;
	dsbdesc.dwBufferBytes = size;
	dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;
	if (FAILED(pDSound->CreateSoundBuffer(&dsbdesc, &dsbuf, NULL))) {
		goto smpl_err2;
	}

	hr = dsbuf->Lock(0, size, (LPVOID *)&blockptr1, &blocksize1,
								(LPVOID *)&blockptr2, &blocksize2, 0);
	if (hr == DSERR_BUFFERLOST) {
		dsbuf->Restore();
		hr = dsbuf->Lock(0, size, (LPVOID *)&blockptr1, &blocksize1,
									(LPVOID *)&blockptr2, &blocksize2, 0);
	}
	if (SUCCEEDED(hr)) {
		extromio_read(erh, blockptr1, blocksize1);
		if ((blockptr2) && (blocksize2)) {
			extromio_read(erh, blockptr2, blocksize2);
		}
		dsbuf->Unlock(blockptr1, blocksize1, blockptr2, blocksize2);
		pDSwave3[num] = dsbuf;
	}
	else {
		dsbuf->Release();
	}

smpl_err2:
	extromio_close(erh);

smpl_err1:
	return;
}

void soundmng_pcmvolume(UINT num, int volume) {

	if ((num < SOUND_MAXPCM) && (pDSwave3[num])) {
		pDSwave3[num]->SetVolume((((DSBVOLUME_MAX - DSBVOLUME_MIN) * volume)
											/ 100) + DSBVOLUME_MIN);
	}
}

BOOL soundmng_pcmplay(UINT num, BOOL loop) {

	LPDIRECTSOUNDBUFFER	dsbuf;

	if ((!mute) && (num < SOUND_MAXPCM)) {
		dsbuf = pDSwave3[num];
		if (dsbuf) {
//			dsbuf->SetCurrentPosition(0);
			dsbuf->Play(0, 0, (loop)?DSBPLAY_LOOPING:0);
			return(SUCCESS);
		}
	}
	return(FAILURE);
}

void soundmng_pcmstop(UINT num) {

	LPDIRECTSOUNDBUFFER	dsbuf;

	if ((!mute) && (num < SOUND_MAXPCM)) {
		dsbuf = pDSwave3[num];
		if (dsbuf) {
			dsbuf->Stop();
		}
	}
}


// ----

BOOL soundmng_initialize(void) {

	if (dsoundcreate() != SUCCESS) {
		goto smcre_err;
	}
	pcmcreate();
	return(SUCCESS);

smcre_err:
	soundmng_destroy();
	return(FAILURE);
}

void soundmng_deinitialize(void) {

	pcmdestroy();
	soundmng_destroy();
	RELEASE(pDSound);
}


// ----

void soundmng_enable(UINT proc) {

	if (!(mute & (1 << proc))) {
		return;
	}
	mute &= ~(1 << proc);
	if (!mute) {
		soundmng_reset();
		streamenable(TRUE);
	}
}

void soundmng_disable(UINT proc) {

	if (!mute) {
		streamenable(FALSE);
		pcmstop();
	}
	mute |= 1 << proc;
}

