
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


// ----

void commng_initialize(void);

#ifdef __cplusplus
}
#endif

