
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


#if 0
typedef long (*WFSEEK)(void *fh, long pos, int method);
typedef UINT (*WFREAD)(void *fh, void *buf, UINT size);

typedef struct {
	UINT	rate;
	UINT	channels;
	UINT	bit;

	void	*fh;
	WFREAD	read;
	WFSEEK	seek;
} _WAVHDL, *WAVHDL;

WAVHDL wavhdl_create(void *fh, WFSEEK seek, WFREAD read);
void wavhdl_destroy(WAVHDL hdl);
UINT wavhdl_getpcm(WAVHDL hdl, void *buf, UINT size);
#endif

