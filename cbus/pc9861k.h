
typedef struct {
	BYTE	result;
	BYTE	data;
	BYTE	signal;
	BYTE	send;

	UINT	pos;
	UINT	dummyinst;
	UINT32	speed;
	SINT32	clk;

	BYTE	dip;
	BYTE	vect;
	BYTE	irq;
} _PC9861CH, *PC9861CH;


#ifdef __cplusplus
extern "C" {
#endif

extern _PC9861CH pc9861ch1;
extern _PC9861CH pc9861ch2;
extern const UINT32 pc9861k_speed[11];


void pc9861ch1cb(NEVENTITEM item);
void pc9861ch2cb(NEVENTITEM item);

void pc9861k_construct(void);
void pc9861k_destruct(void);

void pc9861k_reset(void);
void pc9861k_bind(void);

void pc9861k_midipanic(void);

#ifdef __cplusplus
}
#endif

