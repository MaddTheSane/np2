
typedef struct {
const char	*mnemonic;
	char	operand[32];
} _UNASM, *UNASM;


#ifdef __cplusplus
extern "C" {
#endif

UINT unasm(UNASM r, const BYTE *ptr, UINT leng, BOOL d, UINT32 addr);

#ifdef __cplusplus
}
#endif

