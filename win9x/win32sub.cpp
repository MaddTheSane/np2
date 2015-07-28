#include	"compiler.h"
#include	"np2.h"

void __msgbox(const char *title, const char *msg) {

#if !defined(_UNICODE)
	const TCHAR *_title = title;
	const TCHAR *_msg = msg;
#else
	TCHAR _title[256];
	TCHAR _msg[2048];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, title, -1,
												_title, NELEMENTS(_title));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, msg, -1,
												_msg, NELEMENTS(_msg));
#endif
	MessageBox(NULL, _msg, _title, MB_OK);
}
