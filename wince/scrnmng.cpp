#include	"compiler.h"
#include	<gx.h>
#include	"scrnmng.h"
// #include	"pccore.h"
// #include	"scrndraw.h"
// #include	"palettes.h"
// #include	"menu.h"


		GXDisplayProperties		gx_dp;

static	HWND		TagWnd = NULL;
static	BOOL		TagRot = FALSE;
static	DWORD		TagPosx = 0;
static	DWORD		TagPosy = 0;
static	DWORD		TagWidth = 0;
static	DWORD		TagHeight = 0;
static	long		TagStart = 0;
static	BYTE		gx_disable = 1;

static const TCHAR	errmsg[] = STRLITERAL("Error");



#if 0
void gdraws_enable(BOOL enable) {

	if (enable) {
		gx_disable &= ~2;
	}
	else {
		gx_disable |= 2;
	}
}

BOOL gdraws_mousepos(LPARAM *lp) {

	DWORD	x, y;

	if (!TagRot) {
		x = LOWORD(*lp) - TagPosx;
		y = HIWORD(*lp) - TagPosy;
	}
	else {
		x = TagWidth - HIWORD(*lp) - 1 - TagPosy;
		y = LOWORD(*lp) - TagPosx;
	}
	if ((x >= TagWidth) || (y >= TagHeight)) {
		return(FAILURE);
	}
	*lp = (x & 0xffff) | ((y << 16) & 0xffff0000);
	return(SUCCESS);
}

void gdraws_keybinds(struct GXKeyList *gxkey) {

	short	tmp;

	if (TagRot) {
		tmp = gxkey->vkLeft;
		gxkey->vkLeft = gxkey->vkDown;
		gxkey->vkDown = gxkey->vkRight;
		gxkey->vkRight = gxkey->vkUp;
		gxkey->vkUp = tmp;
	}
}

long gdraws_getyalign(void) {

	if (!TagRot) {
		return(gx_dp.cbyPitch);
	}
	else {
		return(gx_dp.cbxPitch);
	}
}
#endif


// ----

typedef struct {
	int		width;
	int		height;
} SCRNSTAT;

static	SCRNSTAT	scrnstat;
static	SCRNSURF	scrnsurf;


// ----

void scrnmng_initialize(void) {

	scrnstat.width = 640;
	scrnstat.height = 400;
}

BOOL scrnmng_create(HWND hWnd, LONG width, LONG height) {

	BOOL	ret = SUCCESS;

	if ((width <= 0) || (height <= 0)) {
		return(FAILURE);
	}
	if (GXOpenDisplay(hWnd, GX_FULLSCREEN) == 0) {
		MessageBox(hWnd, STRLITERAL("Couldn't GameX Object"),
											errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}
	gx_dp = GXGetDisplayProperties();
	if (gx_dp.cBPP != 16) {
		MessageBox(hWnd, STRLITERAL("Only 16bit color support..."),
											errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}
	if (!(gx_dp.ffFormat & kfDirect565)) {
		MessageBox(hWnd, STRLITERAL("Only 16bit(565) support..."),
											errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}

	if ((gx_dp.cxWidth >= (DWORD)width) &&
		(gx_dp.cyHeight >= (DWORD)height)) {
		TagRot = FALSE;
	}
	else if ((gx_dp.cxWidth >= (DWORD)height) &&
			(gx_dp.cyHeight >= (DWORD)width)) {
		TagRot = TRUE;
	}
	else {
		TCHAR	msg[32];
		wsprintf(msg, STRLITERAL("Required %dx%d..."), width, height);
		MessageBox(hWnd, msg, errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}

	if (!TagRot) {
		TagPosx = ((gx_dp.cxWidth - width) / 2);
		TagPosy = ((gx_dp.cyHeight - height) / 2);
		TagStart = TagPosx * gx_dp.cbxPitch + TagPosy * gx_dp.cbyPitch;
		TagWidth = width;
		TagHeight = height;
	}
	else {
		TagPosx = ((gx_dp.cxWidth - height) / 2);
		TagPosy = ((gx_dp.cyHeight - width) / 2);
		TagStart = ((TagPosy + width - 1) * gx_dp.cbyPitch) +
					(TagPosx * gx_dp.cbxPitch);
		TagWidth = width;
		TagHeight = height;
	}
	gx_disable = 0;
	return(SUCCESS);
}

void scrnmng_destroy(void) {

	gx_disable = 1;
	GXCloseDisplay();
}

RGB16 scrnmng_makepal16(RGB32 pal32) {

	RGB16	ret;

	ret = (pal32.p.r & 0xf8) << 8;
	ret += (pal32.p.g & 0xfc) << (3 + 16);
	ret += pal32.p.b >> 3;
	return(ret);
}

void scrnmng_setwidth(int posx, int width) {

	scrnstat.width = width;
}

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
}

const SCRNSURF *scrnmng_surflock(void) {

	if (gx_disable) {
		return(NULL);
	}
	scrnsurf.ptr = (BYTE *)GXBeginDraw() + TagStart;
	if (!TagRot) {
		scrnsurf.xalign = gx_dp.cbxPitch;
		scrnsurf.yalign = gx_dp.cbyPitch;
	}
	else {
		scrnsurf.xalign = -gx_dp.cbyPitch;
		scrnsurf.yalign = gx_dp.cbxPitch;
	}
	scrnsurf.width = min(scrnstat.width, 640);
	scrnsurf.height = min(scrnstat.height, 400);
	scrnsurf.bpp = 16;
	scrnsurf.extend = 0;
	return(&scrnsurf);
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

	if (surf) {
		GXEndDraw();
	}
}

void scrnmng_update(void) {
}

