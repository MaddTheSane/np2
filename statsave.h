
#if 1		// ÇøÇÂÇ¡Ç∆ÇæÇØÇÊÅH
enum {
	NP2FLAG_SUCCESS		= 0,
	NP2FLAG_DISKCHG		= 0x0001,
	NP2FLAG_VERCHG		= 0x0002,
	NP2FLAG_WARNING		= 0x0080,
	NP2FLAG_VERSION		= 0x0100,
	NP2FLAG_FAILURE		= -1
};
#endif

//	Å´Ç…ïœçXÅ`

enum {
	STATFLAG_SUCCESS	= 0,
	STATFLAG_DISKCHG	= 0x0001,
	STATFLAG_VERCHG		= 0x0002,
	STATFLAG_WARNING	= 0x0080,
	STATFLAG_VERSION	= 0x0100,
	STATFLAG_FAILURE	= -1
};

typedef struct {
	char		index[10];
	UINT16		ver;
	UINT32		size;
} STFLAGHDR;

typedef struct {
	STFLAGHDR	hdr;
	UINT		pos;
	char		*err;
	int			errlen;
} _STFLAGH, *STFLAGH;

typedef struct {
	char	index[10];
	UINT16	ver;
	UINT16	type;
	void	*arg1;
	UINT	arg2;
} SFENTRY;

#ifdef __cplusplus
extern "C" {
#endif

int statflag_read(STFLAGH sfh, void *ptr, UINT size);
int statflag_write(STFLAGH sfh, const void *ptr, UINT size);
void statflag_seterr(STFLAGH sfh, const char *str);

int statsave_save(const char *filename);
int statsave_check(const char *filename, char *buf, int size);
int statsave_load(const char *filename);

#ifdef __cplusplus
}
#endif

