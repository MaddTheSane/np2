#ifndef	NP2_X11_DOSIO_H__
#define	NP2_X11_DOSIO_H__

enum {
	FTYPE_NONE = 0,		// ��ưȽ�� or PC
	FTYPE_SMIL,		// �����ƥ�ͽ��
	FTYPE_TEXT,		// �ƥ����ȥե�����
	FTYPE_BMP,		// Bitmap
	FTYPE_PICT,		// Picture (ͽ��)
	FTYPE_PNG,		// Png (ͽ��)
	FTYPE_WAV,		// Wave
	FTYPE_D88,		// D88
	FTYPE_BETA,		// �٥����᡼��
	FTYPE_THD,		// .thd �ϡ��ɥǥ��������᡼��
	FTYPE_HDI,		// .hdi �ϡ��ɥǥ��������᡼��
	FTYPE_HDD,		// .hdd �ϡ��ɥǥ��������᡼�� (ͽ��)
	FTYPE_S98,		// .s98 �ϡ��ɥǥ��������᡼��
	FTYPE_MIMPI		// mimpi default�ե�����
};

typedef FILE *			FILEH;
#define	FILEH_INVALID		NULL

typedef	void *			FLISTH;
#define	FLISTH_INVALID		NULL

#define	FSEEK_SET		SEEK_SET
#define	FSEEK_CUR		SEEK_CUR
#define	FSEEK_END		SEEK_END

enum {
	FILEATTR_READONLY	= 0x01,
	FILEATTR_HIDDEN		= 0x02,
	FILEATTR_SYSTEM		= 0x04,
	FILEATTR_VOLUME		= 0x08,
	FILEATTR_DIRECTORY	= 0x10,
	FILEATTR_ARCHIVE	= 0x20
};

enum {
	FLICAPS_SIZE		= (1 << 0),
	FLICAPS_ATTR		= (1 << 1),
	FLICAPS_DATE		= (1 << 2),
	FLICAPS_TIME		= (1 << 3)
};

typedef struct {
	UINT16	year;		/* cx */
	BYTE	month;		/* dh */
	BYTE	day;		/* dl */
} DOSDATE;

typedef struct {
	BYTE	hour;		/* ch */
	BYTE	minute;		/* cl */
	BYTE	second;		/* dh */
} DOSTIME;

typedef struct {
	UINT	caps;
	UINT32	size;
	UINT32	attr;
	DOSDATE	date;
	DOSTIME	time;
	char	path[MAX_PATH];
} FLINFO;


#ifdef	__cplusplus
extern "C" {
#endif

/* DOSIO:�ؿ��ν��� */
void dosio_init(void);
void dosio_term(void);

/* �ե�������� */
FILEH file_open(const char *path);
FILEH file_open_rb(const char *path);
FILEH file_create(const char *path);
long file_seek(FILEH handle, long pointer, int method);
UINT file_read(FILEH handle, void *data, UINT length);
UINT file_write(FILEH handle, const void *data, UINT length);
short file_close(FILEH handle);
UINT file_getsize(FILEH handle);
short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime);
short file_delete(const char *path);
short file_attr(const char *path);
short file_dircreate(const char *path);

/* �����ȥե�������� */
void file_setcd(const char *exepath);
char *file_getcd(const char *sjis);
FILEH file_open_c(const char *sjis);
FILEH file_open_rb_c(const char *sjis);
FILEH file_create_c(const char *sjis);
short file_delete_c(const char *sjis);
short file_attr_c(const char *sjis);

FLISTH file_list1st(const char *dir, FLINFO *fli);
BOOL file_listnext(FLISTH hdl, FLINFO *fli);
void file_listclose(FLISTH hdl);

void file_cpyname(char *dst, const char *src, int maxlen);
void file_catname(char *path, const char *sjis, int maxlen);
BOOL file_cmpname(const char *path, const char *sjis);
char *file_getname(char *path);
void file_cutname(char *path);
char *file_getext(char *path);
void file_cutext(char *path);
void file_cutseparator(char *path);
void file_setseparator(char *path, int maxlen);

#ifdef	__cplusplus
};
#endif

#endif	/* NP2_X11_DOSIO_H__ */
