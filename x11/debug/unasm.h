#if defined(SUPPORT_VIEWER)

typedef struct {
	BYTE	seg;
} PREFIX_T;

typedef struct {
const char	*mnemonic;
	char	operand[64];
	char	extend[16];
} UNASM_T;


#ifdef __cplusplus
extern "C" {
#endif

void unasm_reset(PREFIX_T *fix);
int unasm(UINT16 adrs, BYTE *bincode, PREFIX_T *fix, UNASM_T *unasm_t);

#ifdef __cplusplus
}
#endif

#endif
