
typedef struct {
	BYTE	bank;
} _ITF, *ITF;


#ifdef __cplusplus
extern "C" {
#endif

void itf_reset(void);
void itf_bind(void);

#ifdef __cplusplus
}
#endif

