#include	"compiler.h"
#include	"commng.h"


typedef struct {
	HANDLE	hdl;
} _CMPARA, *CMPARA;


static UINT pararead(COMMNG self, BYTE *data) {

	return(0);
}

static UINT parawrite(COMMNG self, BYTE data) {

	CMPARA	para;
	DWORD	writesize;

	para = (CMPARA)(self + 1);
	WriteFile(para->hdl, &data, 1, &writesize, NULL);
	return(1);
}

static BYTE paragetstat(COMMNG self) {

	return(0);
}

static long paramsg(COMMNG self, UINT msg, long param) {

	(void)self;
	(void)msg;
	(void)param;
	return(0);
}

static void pararelease(COMMNG self) {

	CMPARA	para;

	para = (CMPARA)(self + 1);
	CloseHandle(para->hdl);
	_MFREE(self);
}


// ----

COMMNG cmpara_create(UINT port) {

	char	commstr[16];
	HANDLE	hdl;
	COMMNG	ret;
	CMPARA	para;

	wsprintf(commstr, "LPT%u", port);
	hdl = CreateFile(commstr, GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	if (hdl == INVALID_HANDLE_VALUE) {
		goto cpcre_err1;
	}
	ret = (COMMNG)_MALLOC(sizeof(_COMMNG) + sizeof(_CMPARA), "PARALLEL");
	if (ret == NULL) {
		goto cpcre_err2;
	}
	ret->connect = COMCONNECT_PARALLEL;
	ret->read = pararead;
	ret->write = parawrite;
	ret->getstat = paragetstat;
	ret->msg = paramsg;
	ret->release = pararelease;
	para = (CMPARA)(ret + 1);
	para->hdl = hdl;
	return(ret);

cpcre_err2:
	CloseHandle(hdl);

cpcre_err1:
	return(NULL);
}

