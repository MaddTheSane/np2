
enum {												// ver0.28
	FTYPE_NONE = 0,		// �������� or PC
	FTYPE_SMIL,			// �V�X�e���\��
	FTYPE_TEXT,			// �e�L�X�g�t�@�C��
	FTYPE_BMP,			// Bitmap
	FTYPE_PICT,			// Picture (�\��)
	FTYPE_PNG,			// Png (�\��)
	FTYPE_WAV,			// Wave
	FTYPE_D88,			// D88
	FTYPE_BETA,			// �x�^�C���[�W
	FTYPE_THD,			// .thd �n�[�h�f�B�X�N�C���[�W
	FTYPE_HDI,			// .hdi �n�[�h�f�B�X�N�C���[�W
	FTYPE_HDD,			// .hdd �n�[�h�f�B�X�N�C���[�W (�\��)
	FTYPE_S98,			// .s98 �n�[�h�f�B�X�N�C���[�W
	FTYPE_MIMPI			// mimpi default�t�@�C��
};

typedef FILE *			FILEH;
#define	FILEH_INVALID	NULL

#define	FSEEK_SET	SEEK_SET
#define	FSEEK_CUR	SEEK_CUR
#define	FSEEK_END	SEEK_END

enum {
	FILEATTR_READONLY	= 0x01,
	FILEATTR_HIDDEN		= 0x02,
	FILEATTR_SYSTEM		= 0x04,
	FILEATTR_VOLUME		= 0x08,
	FILEATTR_DIRECTORY	= 0x10,
	FILEATTR_ARCHIVE	= 0x20
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


#ifdef	__cplusplus
extern "C" {
#endif

/* DOSIO:�֐��̏��� */
void dosio_init(void);
void dosio_term(void);

/* �t�@�C������ */
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

/* �J�����g�t�@�C������ */
void file_setcd(const char *exepath);
char *file_getcd(const char *path);
FILEH file_open_c(const char *path);
FILEH file_open_rb_c(const char *path);
FILEH file_create_c(const char *path);
short file_delete_c(const char *path);
short file_attr_c(const char *path);


#define file_cpyname(p, n, m)	milstr_ncpy(p, n, m)
#if defined(WIN32)
#define file_cmpname(p, n)		milstr_cmp(p, n)
#else
#define file_cmpname(p, n)		strcmp(p, n)
#endif
void file_catname(char *path, const char *name, int maxlen);
char *file_getname(char *path);
void file_cutname(char *path);
char *file_getext(char *path);
void file_cutext(char *path);
void file_cutseparator(char *path);
void file_setseparator(char *path, int maxlen);

#ifdef	__cplusplus
}
#endif

