
#ifdef __cplusplus
extern "C" {
#endif

BOOL profile_enum(const char *filename, void *arg,
							BOOL (*proc)(void *arg, const char *para,
									const char *key, const char *data));
const char *profile_getarg(const char *str, char *buf, UINT leng);

#ifdef __cplusplus
}
#endif

