
// ---- com manager interface

enum {
	COMCREATE_SERIAL		= 0,
	COMCREATE_PC9861K1,
	COMCREATE_PC9861K2,
	COMCREATE_PRINTER,
	COMCREATE_MPU98II
};

enum {
	COMCONNECT_OFF			= 0,
	COMCONNECT_SERIAL,
	COMCONNECT_MIDI,
	COMCONNECT_PARALLEL
};

enum {
	COMMSG_MIDIRESET		= 0,
	COMMSG_USER
};

struct _commng;
typedef struct _commng	_COMMNG;
typedef struct _commng	*COMMNG;

struct _commng {
	UINT	connect;
	UINT	(*read)(COMMNG self, BYTE *data);
	UINT	(*write)(COMMNG self, BYTE data);
	BYTE	(*getstat)(COMMNG self);
	UINT	(*msg)(COMMNG self, UINT msg, long param);
	void	(*release)(COMMNG self);
};


#ifdef __cplusplus
extern "C" {
#endif

UINT commng_ncread(COMMNG self, BYTE *data);
UINT commng_ncwrite(COMMNG self, BYTE data);
BYTE commng_ncgetstat(COMMNG self);
UINT commng_ncmsg(COMMNG self, UINT msg, long param);

COMMNG commng_create(UINT device);
void commng_destroy(COMMNG hdl);

#ifdef __cplusplus
}
#endif


// ---- com manager for windows

enum {
	COMPORT_NONE			= 0,
	COMPORT_COM1,
	COMPORT_COM2,
	COMPORT_COM3,
	COMPORT_COM4,
	COMPORT_MIDI
};

enum {
	COMMSG_MIMPIDEFFILE		= COMMSG_USER,
	COMMSG_MIMPIDEFEN,
	COMMSG_MIDISTATSET,
	COMMSG_MIDISTATGET
};

void commng_initialize(void);

#include	"cmserial.h"
#include	"cmmidi.h"
#include	"cmpara.h"

