
enum {
	NP2FLAG_SUCCESS		= 0,
	NP2FLAG_DISKCHG		= 0x0001,
	NP2FLAG_VERCHG		= 0x0002,
	NP2FLAG_WARNING		= 0x0080,
	NP2FLAG_VERSION		= 0x0100,
	NP2FLAG_FAILURE		= -1
};


#ifdef __cplusplus
extern "C" {
#endif

int statsave_save(const char *filename);
int statsave_check(const char *filename, char *buf, int size);
int statsave_load(const char *filename);

#ifdef __cplusplus
}
#endif

