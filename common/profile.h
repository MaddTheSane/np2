
#ifdef __cplusplus
extern "C" {
#endif

BOOL profile_enum(const char *filename, void *arg,
							BOOL (*proc)(void *arg, const char *para,
									const char *key, const char *data));

UINT profile_setkeys(const char *str, UINT8 *key, UINT keymax);
void profile_getkeys(char *str, UINT strmax, const UINT8 *key, UINT keys);

#ifdef __cplusplus
}
#endif

