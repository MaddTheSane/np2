
#pragma once

class CDebugUtyView;
typedef CDebugUtyView NP2VIEW_T;

enum {
	ALLOCTYPE_NONE = 0,
	ALLOCTYPE_REG,
	ALLOCTYPE_SEG,
	ALLOCTYPE_1MB,
	ALLOCTYPE_ASM,
	ALLOCTYPE_SND,

	ALLOCTYPE_ERROR = 0xffffffff
};

typedef struct {
	UINT32	type;
	UINT32	arg;
	UINT32	size;
	void	*ptr;
} VIEWMEMBUF;


extern const char viewcmn_hex[16];

void viewcmn_putcaption(NP2VIEW_T *view);

BOOL viewcmn_alloc(VIEWMEMBUF *buf, UINT32 size);
void viewcmn_free(VIEWMEMBUF *buf);

void viewcmn_setmode(NP2VIEW_T *dst, NP2VIEW_T *src, UINT8 type);
LRESULT viewcmn_dispat(NP2VIEW_T* dbg, UINT msg, WPARAM wp, LPARAM lp);

void viewcmn_paint(NP2VIEW_T *view, UINT32 bkgcolor,
						void (*callback)(NP2VIEW_T *view, RECT *rc, HDC hdc));

