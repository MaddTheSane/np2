
typedef struct {
	char	fcbname[12];
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


// 一覧取得
LISTARRAY hostdrvs_getpathlist(const char *realpath);

// ホスト側のファイル名を得る
BOOL hostdrvs_getrealpath(HDRVPATH *hdp, char *dospath);

// ホスト側のファイル名を作る
BOOL hostdrvs_newrealpath(HDRVPATH *hdp, char *dospath);

// ファイルハンドルリスト
void hostdrvs_fhdlreopen(LISTARRAY fhdl);
void hostdrvs_fhdlallclose(LISTARRAY fhdl);
HDRVFILE hostdrvs_fhdlsea(LISTARRAY fhdl);

