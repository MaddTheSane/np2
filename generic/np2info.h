
struct _np2infoex;
typedef struct	_np2infoex	NP2INFOEX;

struct _np2infoex {
	char	cr[4];
	BOOL	(*ext)(char *dst, const char *key, int maxlen, NP2INFOEX *ex);
};


#ifdef __cplusplus
extern "C" {
#endif

void np2info(char *dst, const char *src, int maxlen, const NP2INFOEX *ex);

#ifdef __cplusplus
}
#endif

