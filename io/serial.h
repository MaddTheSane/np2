
enum {
	KB_BUFBIT		= 7,
	KB_BUF			= (1 << KB_BUFBIT),
	KB_BUFMASK		= (KB_BUF - 1)
};

typedef struct {
	BYTE	data;
	BYTE	mode;
	BYTE	cmd;
	BYTE	status;
	int		buffers;
	int		pos;
	BYTE	buf[KB_BUF];
} _KEYB, *KEYB;

typedef struct {
	BYTE	result;
	BYTE	data;
	BYTE	send;
	BYTE	pad;
	UINT	pos;
	UINT	dummyinst;
	UINT	mul;
} _RS232C, *RS232C;



#ifdef __cplusplus
extern "C" {
#endif

void keystat_reset(void);
void keystat_senddata(BYTE data);
void keystat_forcerelease(BYTE value);
void keystat_resetcopyhelp(void);
void keystat_resetjoykey(void);
BYTE keystat_getjoy(void);



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

