#include	"compiler.h"
#include	"fontmng.h"
#include	"vramhdl.h"
#include	"menudeco.inc"
#include	"menubase.h"


typedef struct {
	int		ret;
	UINT	type;
	int		width;
	int		height;
	int		lines;
	int		fontsize;
	char	string[MENUMBOX_MAXLINE][MENUMBOX_MAXTEXT];
} MBOX;

static	MBOX	mbox;

static const char *menumbox_txt[7] = {
				(char *)mstr_ok, 	(char *)mstr_cancel,(char *)mstr_abort,
				(char *)mstr_retry,	(char *)mstr_ignore,(char *)mstr_yes,
				(char *)mstr_no};

static const BYTE b_res[6][4] = {
				{1, DID_OK,		0,				0},
				{2, DID_OK,		DID_CANCEL,		0},
				{3, DID_ABORT,	DID_RETRY,		DID_IGNORE},
				{3, DID_YES,	DID_NO,			DID_CANCEL},
				{2, DID_YES,	DID_NO,			0},
				{2, DID_RETRY,	DID_NO,			0}};


// ----

static BOOL setmboxitem(MBOX *mb, const char *str, UINT type) {

	char	*dst;
	int		rem;
	int		w;
	char	work[4];
	int		width;
	POINT_T	pt;

	if (mb == NULL) {
		goto smbi_err;
	}
	ZeroMemory(mb, sizeof(MBOX));
	if (str == NULL) {
		goto smbi_set;
	}
	work[2] = '\0';
	if ((type & 0xf) >= 6) {
		type &= ~0x0f;
	}
	mb->type = type;
	width = MENUMBOX_WIDTH -
							(MENUMBOX_SX + MENUMBOX_PXTEXT + MENUMBOX_LXTEXT);
	if (mb->type & 0xf0) {
		width -= MENUMBOX_CXICON;
	}
	dst = NULL;
	rem = 0;
	w = 0;
	while(1) {
		work[0] = *str++;
		if (work[0] & (~0x1f)) {
			if ((((work[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
				work[1] = *str++;
				if (work[1] == 0) {
					break;
				}
				rem--;
			}
			else {
				work[1] = '\0';
			}
			rem--;

			fontmng_getsize(menubase.font, work, &pt);
			if ((rem < 0) || ((w + pt.x) > width)) {
				dst = mb->string[mb->lines];
				mb->lines++;
				if (mb->lines >= MENUMBOX_MAXLINE) {
					break;
				}
				rem += sizeof(mb->string[0]) - 1;
				if (mb->width < w) {
					mb->width = w;
				}
				w = 0;
			}
			w += pt.x;
			*dst++ = work[0];
			if (work[1]) {
				*dst++ = work[1];
			}
		}
		else {
			if (work[0] == '\0') {
				break;
			}
			else if (work[0] == '\n') {
				dst = mb->string[mb->lines];
				mb->lines++;
				if (mb->lines >= MENUMBOX_MAXLINE) {
					break;
				}
				rem = sizeof(mb->string[0]) - 1;
				if (mb->width < w) {
					mb->width = w;
				}
				w = 0;
			}
		}
	}
	if (mb->width < w) {
		mb->width = w;
	}

smbi_set:
	mb->width += (MENUMBOX_SX + MENUMBOX_PXTEXT + MENUMBOX_LXTEXT);
	fontmng_getsize(menubase.font, mstr_fontcheck, &pt);
	mb->fontsize = pt.y;
	mb->height = mb->lines * mb->fontsize;
	if (mb->type & 0xf0) {
		mb->width += MENUMBOX_CXICON;
		if (mb->height < MENUMBOX_CYICON) {
			mb->height = MENUMBOX_CYICON;
		}
	}
	mb->height += MENUMBOX_SY +
						MENUMBOX_SYBTN + MENUMBOX_CYBTN + MENUMBOX_LYBTN;

	width = b_res[mb->type & 0x0f][0];
	width *= (MENUMBOX_CXBTN + MENUMBOX_PXBTN);
	width += (MENUMBOX_SXBTN * 2) - MENUMBOX_PXBTN;
	if (mb->width < width) {
		mb->width = width;
	}
	return(SUCCESS);

smbi_err:
	return(FAILURE);
}


static void mbox_open(MBOX *mb) {

	int		posy;
	int		posx;
const BYTE	*btn;
	int		cnt;
	int		btnid;

	posx = MENUMBOX_SX + MENUMBOX_PXTEXT;
	posy = 0;
	if (mb->type & 0xf0) {
		menudlg_append(DLGTYPE_ICON, 0, 0, (void *)((mb->type >> 4) & 0xf),
				MENUMBOX_SX, MENUMBOX_SY, MENUMBOX_SZICON, MENUMBOX_SZICON);
		posx += MENUMBOX_CXICON;
		posy = MENUMBOX_CYICON - (mb->lines * mb->fontsize);
	}
	if (posy > 0) {
		posy /= 2;
		posy += MENUMBOX_SY;
	}
	else {
		posy = MENUMBOX_SY;
	}
	cnt = 0;
	while(cnt < mb->lines) {
		menudlg_append(DLGTYPE_LTEXT, 0, 0, mb->string[cnt],
								posx, posy, mb->width - posx, mb->fontsize);
		posy += mb->fontsize;
		cnt++;
	}

	cnt = mb->type & 0x0f;
	if (cnt >= 6) {
		cnt = 0;
	}
	btn = b_res[cnt];
	cnt = *btn++;
	posy = mb->height - (MENUMBOX_CYBTN + MENUMBOX_LYBTN);
	posx = mb->width;
	posx -= ((MENUMBOX_CXBTN + MENUMBOX_PXBTN) * cnt) - MENUMBOX_PXBTN;
	posx >>= 1;
	while(cnt) {
		cnt--;
		btnid = *btn++;
		menudlg_append(DLGTYPE_BUTTON, (MENUID)btnid, 0,
								menumbox_txt[btnid-1],
								posx, posy, MENUMBOX_CXBTN, MENUMBOX_CYBTN);
		posx += MENUMBOX_CXBTN + MENUMBOX_PXBTN;
	}
}


static int mbox_cmd(int msg, MENUID id, long param) {

	MBOX	*mb;

	switch(msg) {
		case DLGMSG_CREATE:
			mb = &mbox;
			mbox_open(mb);
			break;

		case DLGMSG_COMMAND:
			switch(id) {
				case DID_OK:
				case DID_CANCEL:
				case DID_ABORT:
				case DID_RETRY:
				case DID_IGNORE:
				case DID_YES:
				case DID_NO:
					mb = &mbox;
					mb->ret = id;
					menubase_close();
					break;
			}
			break;

		case DLGMSG_CLOSE:
			menubase_close();
			break;
	}
	(void)param;
	return(0);
}


int menumbox(const char *string, const char *title, UINT type) {

	MBOX	*mb;

	mb = &mbox;
	if (!setmboxitem(mb, string, type)) {
		menudlg_create(mb->width, mb->height, title, mbox_cmd);
		menubase_modalproc();
		return(mb->ret);
	}
	else {
		return(0);
	}
}

