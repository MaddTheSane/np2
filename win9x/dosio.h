
enum {												// ver0.28
	FTYPE_NONE = 0,		// 自動判別 or PC
	FTYPE_SMIL,			// システム予約
	FTYPE_TEXT,			// テキストファイル
	FTYPE_BMP,			// Bitmap
	FTYPE_PICT,			// Picture (予約)
	FTYPE_PNG,			// Png (予約)
	FTYPE_WAV,			// Wave
	FTYPE_D88,			// D88
	FTYPE_BETA,			// ベタイメージ
	FTYPE_THD,			// .thd ハードディスクイメージ
	FTYPE_HDI,			// .hdi ハードディスクイメージ
	FTYPE_HDD,			// .hdd ハードディスクイメージ (予約)
	FTYPE_S98,			// .s98 ハードディスクイメージ
	FTYPE_MIMPI			// mimpi defaultファイル
};

enum {
	FILEATTR_READONLY	= 0x01,
	FILEATTR_HIDDEN		= 0x02,
	FILEATTR_SYSTEM		= 0x04,
	FILEATTR_VOLUME		= 0x08,
	FILEATTR_DIRECTORY	= 0x10,
	FILEATTR_ARCHIVE	= 0x20
};


#define		FILEH				HANDLE
#define		FILEH_INVALID		(INVALID_HANDLE_VALUE)

#define		FLISTH				HANDLE
#define		FLISTH_INVALID		(INVALID_HANDLE_VALUE)

enum {
	FSEEK_SET	= 0,
	FSEEK_CUR	= 1,
	FSEEK_END	= 2
};

typedef struct {
	UINT16	year;		// cx
	BYTE	month;		// dh
	BYTE	day;		// dl
} DOSDATE;

typedef struct {
	BYTE	hour;		// ch
	BYTE	minute;		// cl
	BYTE	second;		// dh
} DOSTIME;

typedef struct {
	char	path[MAX_PATH];
	UINT32	size;
	UINT32	attr;
} FLINFO;


#ifdef __cplusplus
extern "C" {
#endif

											// DOSIO:関数の準備
void dosio_init(void);
void dosio_term(void);
											// ファイル操作
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

											// カレントファイル操作
void file_setcd(const char *exepath);
char *file_getcd(const char *path);
FILEH file_open_c(const char *path);
FILEH file_open_rb_c(const char *path);
FILEH file_create_c(const char *path);
short file_delete_c(const char *path);
short file_attr_c(const char *path);

FLISTH file_list1st(const char *dir, FLINFO *fli);
BOOL file_listnext(FLISTH hdl, FLINFO *fli);
void file_listclose(FLISTH hdl);

#define	file_cpyname(a, b, c)	milsjis_ncpy(a, b, c)
#define	file_catname(a, b, c)	milsjis_ncat(a, b, c)
#define	file_cmpname(a, b)		milsjis_cmp(a, b)
char *file_getname(char *path);
void file_cutname(char *path);
char *file_getext(char *path);
void file_cutext(char *path);
void file_cutseparator(char *path);
void file_setseparator(char *path, int maxlen);

#ifdef __cplusplus
}
#endif

