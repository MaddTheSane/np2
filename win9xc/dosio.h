
#define	DOSIOCALL	__stdcall

#define		FILEH				HANDLE
#define		FILEH_INVALID		(INVALID_HANDLE_VALUE)

#define		FLISTH				HANDLE
#define		FLISTH_INVALID		(INVALID_HANDLE_VALUE)

enum {
	FSEEK_SET	= 0,
	FSEEK_CUR	= 1,
	FSEEK_END	= 2
};

enum {
	FILEATTR_READONLY	= 0x01,
	FILEATTR_HIDDEN		= 0x02,
	FILEATTR_SYSTEM		= 0x04,
	FILEATTR_VOLUME		= 0x08,
	FILEATTR_DIRECTORY	= 0x10,
	FILEATTR_ARCHIVE	= 0x20
};

enum {
	FLICAPS_SIZE		= 0x0001,
	FLICAPS_ATTR		= 0x0002,
	FLICAPS_DATE		= 0x0004,
	FLICAPS_TIME		= 0x0008
};

typedef struct {
	UINT16	year;		// cx
	UINT8	month;		// dh
	UINT8	day;		// dl
} DOSDATE;

typedef struct {
	UINT8	hour;		// ch
	UINT8	minute;		// cl
	UINT8	second;		// dh
} DOSTIME;

typedef struct {
	UINT	caps;
	UINT32	size;
	UINT32	attr;
	DOSDATE	date;
	DOSTIME	time;
	OEMCHAR	path[MAX_PATH];
} FLINFO;


#ifdef __cplusplus
extern "C" {
#endif

											// DOSIO:�֐��̏���
void dosio_init(void);
void dosio_term(void);
											// �t�@�C������
FILEH DOSIOCALL file_open(const OEMCHAR *path);
FILEH DOSIOCALL file_open_rb(const OEMCHAR *path);
FILEH DOSIOCALL file_create(const OEMCHAR *path);
long DOSIOCALL file_seek(FILEH handle, long pointer, int method);
UINT DOSIOCALL file_read(FILEH handle, void *data, UINT length);
UINT DOSIOCALL file_write(FILEH handle, const void *data, UINT length);
short DOSIOCALL file_close(FILEH handle);
UINT DOSIOCALL file_getsize(FILEH handle);
short DOSIOCALL file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime);
short DOSIOCALL file_delete(const OEMCHAR *path);
short DOSIOCALL file_attr(const OEMCHAR *path);
short DOSIOCALL file_dircreate(const OEMCHAR *path);

											// �J�����g�t�@�C������
void DOSIOCALL file_setcd(const OEMCHAR *exepath);
OEMCHAR * DOSIOCALL file_getcd(const OEMCHAR *path);
FILEH DOSIOCALL file_open_c(const OEMCHAR *path);
FILEH DOSIOCALL file_open_rb_c(const OEMCHAR *path);
FILEH DOSIOCALL file_create_c(const OEMCHAR *path);
short DOSIOCALL file_delete_c(const OEMCHAR *path);
short DOSIOCALL file_attr_c(const OEMCHAR *path);

FLISTH DOSIOCALL file_list1st(const OEMCHAR *dir, FLINFO *fli);
BRESULT DOSIOCALL file_listnext(FLISTH hdl, FLINFO *fli);
void DOSIOCALL file_listclose(FLISTH hdl);

#define	file_cpyname(a, b, c)	milstr_ncpy(a, b, c)
#define	file_catname(a, b, c)	milstr_ncat(a, b, c)
#define	file_cmpname(a, b)		milstr_cmp(a, b)
OEMCHAR * DOSIOCALL file_getname(const OEMCHAR *path);
void DOSIOCALL file_cutname(OEMCHAR *path);
OEMCHAR * DOSIOCALL file_getext(const OEMCHAR *path);
void DOSIOCALL file_cutext(OEMCHAR *path);
void DOSIOCALL file_cutseparator(OEMCHAR *path);
void DOSIOCALL file_setseparator(OEMCHAR *path, int maxlen);

#ifdef __cplusplus
}
#endif

#define	file_createex(p, t)		file_create(p)
#define	file_createex_c(p, t)	file_create_c(p)

