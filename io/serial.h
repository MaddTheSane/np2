
enum {
	KB_BUFBIT		= 7,
	KB_BUF			= (1 << KB_BUFBIT),
	KB_BUFMASK		= (KB_BUF - 1)
};

typedef struct {
	UINT8	data;
	UINT8	mode;
	UINT8	cmd;
	UINT8	status;
	int		buffers;
	int		pos;
	BYTE	buf[KB_BUF];
} _KEYB, *KEYB;

typedef struct {
	UINT8	result;
	UINT8	data;
	UINT8	send;
	UINT8	pad;
	UINT	pos;
	UINT	dummyinst;
	UINT	mul;
} _RS232C, *RS232C;



#ifdef __cplusplus
extern "C" {
#endif

void keystat_reset(void);
void keystat_senddata(REG8 data);
void keystat_allrelease(void);
void keystat_forcerelease(REG8 value);
void keystat_resetcopyhelp(void);
void keystat_resetjoykey(void);


void keyext_flash(void);
BYTE keyext_getjoy(void);
BYTE keyext_getmouse(SINT16 *x, SINT16 *y);


void keyb_callback(NEVENTITEM item);

void keyboard_reset(void);
void keyboard_bind(void);
void keyboard_resetsignal(void);



void rs232c_construct(void);
void rs232c_destruct(void);

void rs232c_reset(void);
void rs232c_bind(void);
void rs232c_open(void);
void rs232c_callback(void);

BYTE rs232c_stat(void);
void rs232c_midipanic(void);

#ifdef __cplusplus
}
#endif

