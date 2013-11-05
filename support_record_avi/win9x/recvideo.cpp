#include "compiler.h"
#include <vfw.h>
#include "recvideo.h"
#include "pccore.h"
#include "iocore.h"
#include "scrndraw.h"
#include "dispsync.h"
#include "palettes.h"
#include "dosio.h"

#pragma comment(lib, "vfw32.lib")

#define	LENG			10
#define	VIDEO_WIDTH		640
#define	VIDEO_HEIGHT	400
#define	VIDEO_FPS		30

struct tagRecVideo
{
	COMPVARS cv;
	BOOL bDirty;
	int nStep;
	int nNumber;
	UINT8 *pWork8;
	UINT8 *pWork24;
	struct
	{
		PAVIFILE pAvi;
		PAVISTREAM pStm;
		PAVISTREAM pStmTmp;
		UINT uFrame;
		DWORD dwSize;
	} f;
	BITMAPINFOHEADER bmih;
	TCHAR szPath[MAX_PATH];
};

typedef struct tagRecVideo		RECVIDEO;
typedef struct tagRecVideo		*PRECVIDEO;


static BOOL s_bEnable;
static RECVIDEO s_rec;

static const BITMAPINFOHEADER s_bmih =
		{
			sizeof(BITMAPINFOHEADER),
			VIDEO_WIDTH, VIDEO_HEIGHT, 1, 24, BI_RGB,
			VIDEO_WIDTH * VIDEO_HEIGHT * 3, 0, 0, 0, 0
		};


// ----

static BRESULT openFile(PRECVIDEO pRec)
{
	AVICOMPRESSOPTIONS opt;
	AVISTREAMINFO si =
		{
			streamtypeVIDEO,
			comptypeDIB,
			0,0,0,0,
			1, VIDEO_FPS,
			0, (DWORD)-1, 0, 0, (DWORD)-1, 0,
			{0, 0, VIDEO_WIDTH, VIDEO_HEIGHT}, 0, 0, _T("Video #1")};

	si.fccHandler = pRec->cv.fccHandler;
	opt.fccType = streamtypeVIDEO;
	opt.fccHandler = pRec->cv.fccHandler;
	opt.dwKeyFrameEvery = pRec->cv.lKey;
	opt.dwQuality = pRec->cv.lQ;
	opt.dwBytesPerSecond = pRec->cv.lDataRate;
	opt.dwFlags = (pRec->cv.lDataRate > 0) ? AVICOMPRESSF_DATARATE : 0;
	opt.dwFlags |= (pRec->cv.lKey > 0) ? AVICOMPRESSF_KEYFRAMES : 0;
	opt.lpFormat = NULL;
	opt.cbFormat = 0;
	opt.lpParms = pRec->cv.lpState;
	opt.cbParms = pRec->cv.cbState;
	opt.dwInterleaveEvery = 0;

	TCHAR szPath[MAX_PATH];
	file_cpyname(szPath, pRec->szPath, NELEMENTS(szPath));

	TCHAR szExt[16];
	wsprintf(szExt, _T("_%04d.avi"), pRec->nNumber);
	file_catname(szPath, szExt, NELEMENTS(szPath));

	if (AVIFileOpen(&pRec->f.pAvi, szPath,
					OF_CREATE | OF_WRITE | OF_SHARE_DENY_NONE,NULL) != 0)
	{
		return FAILURE;
	}
	if (AVIFileCreateStream(pRec->f.pAvi, &pRec->f.pStm, &si) != 0)
	{
		return FAILURE;
	}
	if (AVIMakeCompressedStream(&pRec->f.pStmTmp, pRec->f.pStm, &opt, NULL)
															!= AVIERR_OK)
	{
		return FAILURE;
	}
	if (AVIStreamSetFormat(pRec->f.pStmTmp, 0,
							&pRec->bmih, sizeof(pRec->bmih)) != 0)
	{
		return FAILURE;
	}

	s_bEnable = TRUE;

	pRec->bDirty = TRUE;
	pRec->nNumber++;

	return SUCCESS;
}

static void closeFile(PRECVIDEO pRec)
{
	s_bEnable = FALSE;

	if (pRec->f.pStmTmp)
	{
		AVIStreamRelease(pRec->f.pStmTmp);
	}
	if (pRec->f.pStm)
	{
		AVIStreamRelease(pRec->f.pStm);
	}
	if (pRec->f.pAvi)
	{
		AVIFileRelease(pRec->f.pAvi);
	}
	ZeroMemory(&pRec->f, sizeof(pRec->f));
}

static BRESULT open(PRECVIDEO pRec, LPCTSTR lpcszFilename)
{
	ZeroMemory(pRec, sizeof(*pRec));

	pRec->bmih = s_bmih;

	pRec->cv.cbSize = sizeof(pRec->cv);
	pRec->cv.dwFlags = ICMF_COMPVARS_VALID;
	pRec->cv.fccHandler = comptypeDIB;
	pRec->cv.lQ = ICQUALITY_DEFAULT;
	if (!ICCompressorChoose(NULL,
							ICMF_CHOOSE_DATARATE | ICMF_CHOOSE_KEYFRAME,
							&pRec->bmih, NULL, &pRec->cv, NULL))
	{
		return FAILURE;
	}

	pRec->pWork8 = (UINT8 *)(malloc(SURFACE_WIDTH * SURFACE_HEIGHT));
	if (!pRec->pWork8)
	{
		return FAILURE;
	}
	ZeroMemory(pRec->pWork8, SURFACE_WIDTH * SURFACE_HEIGHT);

	pRec->pWork24 = (UINT8 *)(malloc(VIDEO_WIDTH * VIDEO_HEIGHT * 3));
	if (!pRec->pWork24)
	{
		return FAILURE;
	}
	ZeroMemory(pRec->pWork24, VIDEO_WIDTH * VIDEO_HEIGHT * 3);

	file_cpyname(pRec->szPath, lpcszFilename, NELEMENTS(pRec->szPath));
	file_cutext(pRec->szPath);

	return openFile(pRec);
}

static void close(PRECVIDEO pRec)
{
	closeFile(pRec);

	ICCompressorFree(&pRec->cv);

	if (pRec->pWork8)
	{
		free(pRec->pWork8);
	}
	if (pRec->pWork24)
	{
		free(pRec->pWork24);
	}

	ZeroMemory(pRec, sizeof(*pRec));
}

static void write(PRECVIDEO pRec)
{
	UINT8	*pBuffer;
	UINT	uBufferSize;
	DWORD	dwFlags;

	while(pRec->nStep >= 0)
	{
		pBuffer = pRec->pWork24;
		uBufferSize = 0;
		dwFlags = 0;
		if (pRec->bDirty)
		{
			uBufferSize = VIDEO_WIDTH * VIDEO_HEIGHT * 3;
			dwFlags = AVIIF_KEYFRAME;
		}
		LONG lSize = 0;
		if (AVIStreamWrite(pRec->f.pStmTmp, pRec->f.uFrame, 1,
								pBuffer, uBufferSize, dwFlags,
								NULL, &lSize) != 0)
		{
			break;
		}
		pRec->bDirty = FALSE;
		pRec->nStep -= 21052600 / (8 * VIDEO_FPS);
		pRec->f.uFrame++;
		pRec->f.dwSize += lSize;

		if (pRec->f.dwSize >= 1024 * 1024 * 1024)
		{
			closeFile(pRec);
			openFile(pRec);
		}
	}
	pRec->nStep += 106 * 440;
}



// ----

static void screenmix1(UINT8 *dest, const UINT8 *src1, const UINT8 *src2)
{
	int		i;

	for (i=0; i<(SURFACE_WIDTH * SURFACE_HEIGHT); i++)
	{
		dest[i] = src1[i] + src2[i] + NP2PAL_GRPH;
	}
}

static void screenmix2(UINT8 *dest, const UINT8 *src1, const UINT8 *src2)
{
	int		x, y;

	for (y=0; y<(SURFACE_HEIGHT/2); y++)
	{
		for (x=0; x<SURFACE_WIDTH; x++)
		{
			dest[x] = src1[x] + src2[x] + NP2PAL_GRPH;
		}
		dest += SURFACE_WIDTH;
		src1 += SURFACE_WIDTH;
		src2 += SURFACE_WIDTH;
		for (x=0; x<SURFACE_WIDTH; x++)
		{
			dest[x] = (src1[x] >> 4) + NP2PAL_TEXT;
		}
		dest += SURFACE_WIDTH;
		src1 += SURFACE_WIDTH;
		src2 += SURFACE_WIDTH;
	}
}

static void screenmix3(UINT8 *dest, const UINT8 *src1, const UINT8 *src2)
{
	UINT8	c;
	int		x, y;

	for (y=0; y<(SURFACE_HEIGHT/2); y++)
	{
		// dest == src1, dest == src2 ‚ÌŽž‚ª‚ ‚é‚Ì‚Åc
		for (x=0; x<SURFACE_WIDTH; x++)
		{
			c = (src1[x + SURFACE_WIDTH]) >> 4;
			if (!c)
			{
				c = src2[x] + NP2PAL_SKIP;
			}
			dest[x + SURFACE_WIDTH] = c;
			dest[x] = src1[x] + src2[x] + NP2PAL_GRPH;
		}
		dest += SURFACE_WIDTH * 2;
		src1 += SURFACE_WIDTH * 2;
		src2 += SURFACE_WIDTH * 2;
	}
}

static void screenmix(PRECVIDEO pRec)
{
	void	(*fnMix)(UINT8 *dest, const UINT8 *src1, const UINT8 *src2);
	UINT8	*p;
	UINT8	*q;
	int		nWidth;
	int		nHeight;
	int		y;
	UINT8	*r;
	int		x;
	RGB32	*pPal;

	ZeroMemory(pRec->pWork8, SURFACE_WIDTH * SURFACE_HEIGHT);
	fnMix = NULL;

	if (!(gdc.mode1 & 0x10))
	{
		fnMix = screenmix1;
	}
	else if (!np2cfg.skipline)
	{
		fnMix = screenmix2;
	}
	else
	{
		fnMix = screenmix3;
	}
	p = pRec->pWork8;
	q = pRec->pWork8;
	if (gdc.mode1 & 0x80)
	{
		if (gdcs.textdisp & 0x80)
		{
			p = np2_tram;
		}
		if (gdcs.grphdisp & 0x80)
		{
			q = np2_vram[gdcs.disp];
		}
	}
	(*fnMix)(pRec->pWork8, p, q);

	nWidth = dsync.scrnxmax;
	nHeight = dsync.scrnymax;

	nWidth = min(nWidth, VIDEO_WIDTH);
	nHeight = min(nHeight, VIDEO_HEIGHT);
	p = pRec->pWork8;
	q = pRec->pWork24 + (VIDEO_WIDTH * nHeight * 3);
	for (y=0; y<nHeight; y++)
	{
		q -= VIDEO_WIDTH * 3;
		r = q;
		for (x=0; x<nWidth; x++)
		{
			pPal = np2_pal32 + p[x];
			r[0] = pPal->p.b;
			r[1] = pPal->p.g;
			r[2] = pPal->p.r;
			r += 3;
		}
		p += SURFACE_WIDTH;
	}
	pRec->bDirty = TRUE;
}



// ----

void recvideo_initialize(void)
{
	AVIFileInit();
}

void recvideo_deinitialize(void)
{
	close(&s_rec);
	AVIFileExit();
}

void recvideo_open(LPCTSTR lpcszFilename)
{
	open(&s_rec, lpcszFilename);
}

void recvideo_close(void)
{
	close(&s_rec);
}

void recvideo_write(void)
{
	if (s_bEnable)
	{
		write(&s_rec);
	}
}

void recvideo_update(void)
{
	if (s_bEnable)
	{
		screenmix(&s_rec);
	}
}

