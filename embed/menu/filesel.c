#include	"compiler.h"
#include	"strres.h"
#include	"codecnv.h"
#include	"dosio.h"
#include	"soundmng.h"
#include	"pccore.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"filesel.h"
#include	"vramhdl.h"
#include	"menubase.h"
#include	"menustr.h"


enum {
	DID_FOLDER	= DID_USER,
	DID_PARENT,
	DID_FLIST,
	DID_FILE,
	DID_FILTER
};

#if !defined(RESOURCE_US) && (!defined(CHARSET_OEM) || defined(OSLANG_SJIS))
static const char str_dirname[] =				// ファイルの場所
			"\203\164\203\100\203\103\203\213\202\314\217\352\217\212";
static const char str_filename[] =				// ファイル名
			"\203\164\203\100\203\103\203\213\226\274";
static const char str_filetype[] =				// ファイルの種類
			"\203\164\203\100\203\103\203\213\202\314\216\355\227\336";
static const char str_open[] =					// 開く
			"\212\112\202\255";
#elif defined(OSLANG_EUC) && !defined(RESOURCE_US)
static const char str_dirname[] =				// ファイルの場所
			"\245\325\245\241\245\244\245\353\244\316\276\354\275\352";
static const char str_filename[] =				// ファイル名
			"\245\325\245\241\245\244\245\353\314\276";
static const char str_filetype[] =				// ファイルの種類
			"\245\325\245\241\245\244\245\353\244\316\274\357\316\340";
static const char str_open[] =					// 開く
			"\263\253\244\257";
#elif defined(OSLANG_UTF8) && !defined(RESOURCE_US)
static const char str_dirname[] =				// ファイルの場所
			"\343\203\225\343\202\241\343\202\244\343\203\253\343\201\256" \
			"\345\240\264\346\211\200";
static const char str_filename[] =				// ファイル名
			"\343\203\225\343\202\241\343\202\244\343\203\253\345\220\215";
static const char str_filetype[] =				// ファイルの種類
			"\343\203\225\343\202\241\343\202\244\343\203\253\343\201\256" \
			"\347\250\256\351\241\236";
static const char str_open[] =					// 開く
			"\351\226\213\343\201\217";
#else
static const char str_dirname[] = "Look in";
static const char str_filename[] = "File name";
static const char str_filetype[] = "Files of type";
static const char str_open[] = "Open";
#endif

#if defined(SIZE_QVGA)
enum {
	DLGFS_WIDTH		= 294,
	DLGFS_HEIGHT	= 187
};
static const MENUPRM res_fs[] = {
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_dirname,							  6,   9,  68,  11},
			{DLGTYPE_EDIT,		DID_FOLDER,		0,
				NULL,									 74,   6, 192,  16},
			{DLGTYPE_BUTTON,	DID_PARENT,		MENU_TABSTOP,
				NULL,									272,   6,  16,  16},
			{DLGTYPE_LIST,		DID_FLIST,		MENU_TABSTOP,
				NULL,									  5,  28, 284, 115},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_filename,							  6, 150,  68,  11},
			{DLGTYPE_EDIT,		DID_FILE,		0,
				NULL,									 74, 147, 159,  16},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_filetype,							  6, 169,  68,  11},
			{DLGTYPE_EDIT,		DID_FILTER,		0,
				NULL,									 74, 166, 159,  16},
			{DLGTYPE_BUTTON,	DID_OK,			MENU_TABSTOP,
				str_open,								237, 147,  52,  15},
			{DLGTYPE_BUTTON,	DID_CANCEL,		MENU_TABSTOP,
				mstr_cancel,							237, 166,  52,  15}};
#else
enum {
	DLGFS_WIDTH		= 499,
	DLGFS_HEIGHT	= 227
};
static const MENUPRM res_fs[] = {
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_dirname,							 12,  10, 102,  13},
			{DLGTYPE_EDIT,		DID_FOLDER,		0,
				NULL,									114,   7, 219,  18},
			{DLGTYPE_BUTTON,	DID_PARENT,		MENU_TABSTOP,
				NULL,									348,   7,  18,  18},
			{DLGTYPE_LIST,		DID_FLIST,		MENU_TABSTOP,
				NULL,									  7,  30, 481, 128},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_filename,							 12, 168, 104,  13},
			{DLGTYPE_EDIT,		DID_FILE,		0,
				NULL,									116, 165, 268,  18},
			{DLGTYPE_LTEXT,		DID_STATIC,		0,
				str_filetype,							 12, 192, 104,  13},
			{DLGTYPE_EDIT,		DID_FILTER,		0,
				NULL,									116, 189, 268,  18},
			{DLGTYPE_BUTTON,	DID_OK,			MENU_TABSTOP,
				str_open,								397, 165,  88,  23},
			{DLGTYPE_BUTTON,	DID_CANCEL,		MENU_TABSTOP,
				mstr_cancel,							397, 192,  88,  23}};
#endif

struct _flist;
typedef struct _flist	 _FLIST;
typedef struct _flist	 *FLIST;

struct _flist {
	FLIST	next;
	UINT	isdir;
	char	name[MAX_PATH];
};

typedef struct {
const char	*title;
const char	*filter;
const char	*ext;
} FSELPRM;

typedef struct {
	BOOL		result;
	LISTARRAY	flist;
	FLIST		fbase;
const char		*filter;
const char		*ext;
	char		path[MAX_PATH];
} FILESEL;

static	FILESEL		filesel;


// ----

static FLIST getflist(int pos) {

	FLIST	ret;

	ret = NULL;
	if (pos >= 0) {
		ret = filesel.fbase;
		while((pos > 0) && (ret)) {
			pos--;
			ret = ret->next;
		}
	}
	return(ret);
}

static BOOL fappend(LISTARRAY flist, FLINFO *fli) {

	FLIST	fl;
	FLIST	*st;
	FLIST	cur;

	fl = (FLIST)listarray_append(flist, NULL);
	if (fl == NULL) {
		return(FAILURE);
	}
	fl->isdir = (fli->attr & 0x10)?1:0;
	file_cpyname(fl->name, fli->path, sizeof(fl->name));
	st = &filesel.fbase;
	while(1) {
		cur = *st;
		if (cur == NULL) {
			break;
		}
		if (fl->isdir > cur->isdir) {
			break;
		}
		if ((fl->isdir == cur->isdir) &&
			(file_cmpname(fl->name, cur->name) < 0)) {
			break;
		}
		st = &cur->next;
	}
	fl->next = *st;
	*st = fl;
	return(SUCCESS);
}

static BOOL checkext(char *path, const char *ext) {

const char	*p;

	if (ext == NULL) {
		return(TRUE);
	}
	p = file_getext(path);
	while(*ext) {
		if (!file_cmpname(p, ext)) {
			return(TRUE);
		}
		ext += strlen(ext) + 1;
	}
	return(FALSE);
}

static void dlgsetlist(void) {

	LISTARRAY	flist;
	FLISTH		flh;
	FLINFO		fli;
	BOOL		append;
	FLIST		fl;
	ITEMEXPRM	prm;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8)
	char		sjis[MAX_PATH];
#endif

	menudlg_itemreset(DID_FLIST);

#if defined(OSLANG_EUC)
	codecnv_euc2sjis(sjis, sizeof(sjis),
									file_getname(filesel.path), (UINT)-1);
	menudlg_settext(DID_FOLDER, sjis);
#elif defined(OSLANG_UTF8)
	oemtext_oem2sjis(sjis, sizeof(sjis),
									file_getname(filesel.path), (UINT)-1);
	menudlg_settext(DID_FOLDER, sjis);
#else
	menudlg_settext(DID_FOLDER, file_getname(filesel.path));
#endif
	listarray_destroy(filesel.flist);
	flist = listarray_new(sizeof(_FLIST), 64);
	filesel.flist = flist;
	filesel.fbase = NULL;
	flh = file_list1st(filesel.path, &fli);
	if (flh != FLISTH_INVALID) {
		do {
			append = FALSE;
			if (fli.attr & 0x10) {
				append = TRUE;
			}
			else if (!(fli.attr & 0x08)) {
				append = checkext(fli.path, filesel.ext);
			}
			if (append) {
				if (fappend(flist, &fli) != SUCCESS) {
					break;
				}
			}
		} while(file_listnext(flh, &fli) == SUCCESS);
		file_listclose(flh);
	}
	prm.pos = 0;
	fl = filesel.fbase;
	while(fl) {
		menudlg_itemappend(DID_FLIST, NULL);
		prm.icon = (fl->isdir)?MICON_FOLDER:MICON_FILE;
#if defined(OSLANG_EUC)
		codecnv_euc2sjis(sjis, sizeof(sjis), fl->name, (UINT)-1);
		prm.str = sjis;
#elif defined(OSLANG_UTF8)
		oemtext_oem2sjis(sjis, sizeof(sjis), fl->name, (UINT)-1);
		prm.str = sjis;
#else
		prm.str = fl->name;
#endif
		menudlg_itemsetex(DID_FLIST, &prm);
		fl = fl->next;
		prm.pos++;
	}
}

static void dlginit(void) {

#if defined(OSLANG_EUC) || defined(OSLANG_UTF8)
	char	sjis[MAX_PATH];
#endif

	menudlg_appends(res_fs, sizeof(res_fs)/sizeof(MENUPRM));
	menudlg_seticon(DID_PARENT, MICON_FOLDERPARENT);
#if defined(OSLANG_EUC)
	codecnv_euc2sjis(sjis, sizeof(sjis),
									file_getname(filesel.path), (UINT)-1);
	menudlg_settext(DID_FILE, sjis);
#elif defined(OSLANG_UTF8)
	oemtext_oem2sjis(sjis, sizeof(sjis),
									file_getname(filesel.path), (UINT)-1);
	menudlg_settext(DID_FILE, sjis);
#else
	menudlg_settext(DID_FILE, file_getname(filesel.path));
#endif
	menudlg_settext(DID_FILTER, filesel.filter);
	file_cutname(filesel.path);
	file_cutseparator(filesel.path);
	dlgsetlist();
}

static BOOL dlgupdate(void) {

	FLIST	fl;

	fl = getflist(menudlg_getval(DID_FLIST));
	if (fl == NULL) {
		return(FALSE);
	}
	file_setseparator(filesel.path, sizeof(filesel.path));
	file_catname(filesel.path, fl->name, sizeof(filesel.path));
	if (fl->isdir) {
		dlgsetlist();
		menudlg_settext(DID_FILE, NULL);
		return(FALSE);
	}
	else {
		filesel.result = TRUE;
		return(TRUE);
	}
}

static void dlgflist(void) {

	FLIST	fl;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8)
	char	sjis[MAX_PATH];
#endif

	fl = getflist(menudlg_getval(DID_FLIST));
	if ((fl != NULL) && (!fl->isdir)) {
#if defined(OSLANG_EUC)
		codecnv_euc2sjis(sjis, sizeof(sjis), fl->name, (UINT)-1);
		menudlg_settext(DID_FILE, sjis);
#elif defined(OSLANG_UTF8)
		oemtext_oem2sjis(sjis, sizeof(sjis), fl->name, (UINT)-1);
		menudlg_settext(DID_FILE, sjis);
#else
		menudlg_settext(DID_FILE, fl->name);
#endif
	}
}

static int dlgcmd(int msg, MENUID id, long param) {

	switch(msg) {
		case DLGMSG_CREATE:
			dlginit();
			break;

		case DLGMSG_COMMAND:
			switch(id) {
				case DID_OK:
					if (dlgupdate()) {
						menubase_close();
					}
					break;

				case DID_CANCEL:
					menubase_close();
					break;

				case DID_PARENT:
					file_cutname(filesel.path);
					file_cutseparator(filesel.path);
					dlgsetlist();
					menudlg_settext(DID_FILE, NULL);
					break;

				case DID_FLIST:
					dlgflist();
					break;
			}
			break;

		case DLGMSG_CLOSE:
			menubase_close();
			break;

		case DLGMSG_DESTROY:
			listarray_destroy(filesel.flist);
			filesel.flist = NULL;
			break;
	}
	(void)param;
	return(0);
}

static BOOL selectfile(const FSELPRM *prm, char *path, int size, 
															const char *def) {

const char	*title;

	soundmng_stop();
	ZeroMemory(&filesel, sizeof(filesel));
	if ((def) && (def[0])) {
		file_cpyname(filesel.path, def, sizeof(filesel.path));
	}
	else {
		file_cpyname(filesel.path, file_getcd(str_null),
														sizeof(filesel.path));
		file_cutname(filesel.path);
	}
	title = NULL;
	if (prm) {
		title = prm->title;
		filesel.filter = prm->filter;
		filesel.ext = prm->ext;
	}
	menudlg_create(DLGFS_WIDTH, DLGFS_HEIGHT, title, dlgcmd);
	menubase_modalproc();
	soundmng_play();
	if (filesel.result) {
		file_cpyname(path, filesel.path, size);
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}


// ----

static const char diskfilter[] = "All supported files";
static const char fddtitle[] = "Select floppy image";
static const char fddext[] = "d88\088d\0d98\098d\0fdi\0" \
								"xdf\0hdm\0dup\02hd\0tfd\0";
static const char hddtitle[] = "Select HDD image";
static const char sasiext[] = "thd\0nhd\0hdi\0";

static const FSELPRM fddprm = {fddtitle, diskfilter, fddext};
static const FSELPRM sasiprm = {hddtitle, diskfilter, sasiext};

#if defined(SUPPORT_SCSI)
static const char scsiext[] = "hdd\0";
static const FSELPRM scsiprm = {hddtitle, diskfilter, scsiext};
#endif


void filesel_fdd(REG8 drv) {

	char	path[MAX_PATH];

	if (drv < 4) {
		if (selectfile(&fddprm, path, sizeof(path), fdd_diskname(drv))) {
			diskdrv_setfdd(drv, path, 0);
		}
	}
}

void filesel_hdd(REG8 drv) {

	UINT		num;
	char		*p;
const FSELPRM	*prm;
	char		path[MAX_PATH];

	num = drv & 0x0f;
	p = NULL;
	prm = NULL;
	if (!(drv & 0x20)) {		// SASI/IDE
		if (num < 2) {
			p = np2cfg.sasihdd[num];
			prm = &sasiprm;
		}
	}
#if defined(SUPPORT_SCSI)
	else {						// SCSI
		if (num < 4) {
			p = np2cfg.scsihdd[num];
			prm = &scsiprm;
		}
	}
#endif
	if ((prm) && (selectfile(prm, path, sizeof(path), p))) {
		diskdrv_sethdd(drv, path);
	}
}

