
typedef struct {
	int		reset_req;
} _CPUIO, *CPUIO;


#ifdef __cplusplus
extern "C" {
#endif

void cpuio_reset(void);
void cpuio_bind(void);

#ifdef __cplusplus
}
#endif

