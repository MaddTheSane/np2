
// Ç†Ç∆Ç≈ GETSNDÇ…èÊÇËä∑Ç¶ÇÈÇ©Ç‡Åc

#if defined(BYTESEX_BIG)
#define	WAVE_SIG(a, b, c, d)	\
				(UINT32)((d) + ((c) << 8) + ((b) << 16) + ((a) << 24))
#else
#define	WAVE_SIG(a, b, c, d)	\
				(UINT32)((a) + ((b) << 8) + ((c) << 16) + ((d) << 24))
#endif

typedef struct {
	UINT32	sig;
	BYTE	size[4];
	UINT32	fmt;
} RIFF_HEADER;

typedef struct {
	UINT	sig;
	BYTE	size[4];
} WAVE_HEADER;

typedef struct {
	BYTE	format[2];
	BYTE	channel[2];
	BYTE	rate[4];
	BYTE	rps[4];
	BYTE	block[2];
	BYTE	bit[2];
} WAVE_INFOS;


// ---- write

typedef struct {
	long		fh;
	UINT		rate;
	UINT		bits;
	UINT		ch;
	UINT		size;

	BYTE		*ptr;
	UINT		remain;
	BYTE		buf[4096];
} _WAVEWR, *WAVEWR;


#ifdef __cplusplus
extern "C" {
#endif

WAVEWR wavewr_open(const char *filename, UINT rate, UINT bits, UINT ch);
UINT wavewr_write(WAVEWR hdl, const void *buf, UINT size);
void wavewr_close(WAVEWR hdl);

#ifdef __cplusplus
}
#endif

