
#if defined(SUPPORT_WAVEMIX)

enum {
	WAVEMIX_SEEK	= 0,
	WAVEMIX_SEEK1,

	WAVEMIX_MAXTRACK
};

enum {
	WMFLAG_L		= 0x0001,
	WMFLAG_R		= 0x0002,
	WMFLAG_LOOP		= 0x0004
};


#ifdef __cplusplus
extern "C" {
#endif

void wavemix_initialize(UINT rate);
void wavemix_deinitialize(void);

void wavemix_play(UINT num, BOOL loop);
void wavemix_stop(UINT num);
void wavemix_bind(void);

#ifdef __cplusplus
}
#endif

#endif

