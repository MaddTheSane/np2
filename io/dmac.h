
#ifndef DMACCALL
#define	DMACCALL
#endif


enum {
	DMA_CH00			= 0,
	DMA_CH01			= 1,
	DMA_2HD				= 2,
	DMA_2DD				= 3,

	DMAEXT_START		= 0,
	DMAEXT_END			= 1,
	DMAEXT_BREAK		= 2,

	DMA_INITSIGNALONLY	= 1
};

#if defined(BYTESEX_LITTLE)
enum {
	DMA16_LOW		= 0,
	DMA16_HIGH		= 1,
	DMA32_LOW		= 0,
	DMA32_HIGH		= 2
};
#elif defined(BYTESEX_BIG)
enum {
	DMA16_LOW		= 1,
	DMA16_HIGH		= 0,
	DMA32_LOW		= 2,
	DMA32_HIGH		= 0
};
#endif

typedef struct {
	union {
		BYTE	b[4];
		UINT16	w[2];
		UINT32	d;
	} adrs;
	union {
		BYTE	b[2];
		UINT16	w;
	} leng;
	union {
		BYTE	b[2];
		UINT16	w;
	} adrsorg;
	union {
		BYTE	b[2];
		UINT16	w;
	} lengorg;
	UINT16	action;
	void	(DMACCALL * outproc)(REG8 data);
	REG8	(DMACCALL * inproc)(void);
	REG8	(DMACCALL * extproc)(REG8 action);
	UINT8	mode;
	UINT8	sreq;
	UINT8	ready;
	UINT8	mask;
} _DMACH, *DMACH;

typedef struct {
	_DMACH	dmach[4];
	int		lh;
	UINT	work;
	UINT	working;
	UINT8	mask;
	UINT8	stat;
} _DMAC, *DMAC;


#ifdef __cplusplus
extern "C" {
#endif

void DMACCALL dma_dummyout(REG8 data);
REG8 DMACCALL dma_dummyin(void);
REG8 DMACCALL dma_dummyproc(REG8 func);

void dmac_reset(void);
void dmac_bind(void);

void dmac_check(void);

#ifdef __cplusplus
}
#endif

