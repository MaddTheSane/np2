
typedef struct {
	char	fcbname[11];
	BYTE	exist;
	UINT	caps;
	UINT32	size;
	UINT32	attr;
	DOSDATE	date;
	DOSTIME	time;
} HDRVDIR;

typedef struct {
	HDRVDIR	di;
	char	realname[MAX_PATH];
} _HDRVLST, *HDRVLST;

typedef struct {
	HDRVDIR	di;
	char	path[MAX_PATH];
} HDRVPATH;


// �ꗗ�擾
LISTARRAY hostdrvs_getpathlist(const char *realpath);

// �z�X�g���̃t�H���_�𓾂�
BOOL hostdrvs_getrealdir(char *path, int size, char *fcb, char *dospath);

// �z�X�g���̃t�@�C�����𓾂�
BOOL hostdrvs_getrealpath(HDRVPATH *hdp, char *dospath);

// �z�X�g���̃t�@�C���������
BOOL hostdrvs_newrealpath(HDRVPATH *hdp, char *dospath);

// �t�@�C���n���h�����X�g
void hostdrvs_fhdlallclose(LISTARRAY fhdl);
HDRVFILE hostdrvs_fhdlsea(LISTARRAY fhdl);

