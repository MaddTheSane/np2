
typedef struct {
	char	fcbname[11];
	UINT8	exist;
	UINT	caps;
	UINT32	size;
	UINT32	attr;
	DOSDATE	date;
	DOSTIME	time;
} HDRVDIR;

typedef struct {
	HDRVDIR	di;
	OEMCHAR	realname[MAX_PATH];
} _HDRVLST, *HDRVLST;

typedef struct {
	HDRVDIR	di;
	OEMCHAR	path[MAX_PATH];
} HDRVPATH;


// �ꗗ�擾
LISTARRAY hostdrvs_getpathlist(const OEMCHAR *realpath);

// �z�X�g���̃t�H���_�𓾂�
BRESULT hostdrvs_getrealdir(OEMCHAR *path, int size, char *fcb, char *dospath);

// �z�X�g���̃t�@�C�����𓾂�
BRESULT hostdrvs_getrealpath(HDRVPATH *hdp, char *dospath);

// �z�X�g���̃t�@�C���������
BRESULT hostdrvs_newrealpath(HDRVPATH *hdp, char *dospath);

// �t�@�C���n���h�����X�g
void hostdrvs_fhdlallclose(LISTARRAY fhdl);
HDRVFILE hostdrvs_fhdlsea(LISTARRAY fhdl);

