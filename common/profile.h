
#ifdef __cplusplus
extern "C" {
#endif

BOOL profile_enum(const char *filename, void *arg,
							BOOL (*proc)(void *arg, const char *para,
									const char *key, const char *data));

#ifdef __cplusplus
}
#endif

