#include	"compiler.h"
#include	"np2arg.h"
#include	"dosio.h"

												// ver0.26 np2arg.cpp append
// コマンドラインの解析

// OS依存しそうなので　切り分けます。


#define	MAXARG		32
#define	ARG_BASE	0				// win32のlpszCmdLineの場合
									// 実行ファイル名無し

	NP2ARG_T	np2arg = {{NULL, NULL, NULL, NULL},
							NULL, FALSE};

// ---------------------------------------------------------------------

// win32の場合 コマンドラインが stringが返ってくる・・・

static char argstrtmp[1024];

void np2arg_analize(char *argstr) {

	int		np2argc;
	char	*np2argv[MAXARG];
	int		i;
	int		drv = 0;
	char	c;
const char	*p;

	milstr_ncpy(argstrtmp, argstr, sizeof(argstrtmp));
	np2argc = milstr_getarg(argstrtmp, np2argv, sizeof(argstrtmp));

	for (i=ARG_BASE; i<np2argc; i++) {
		c = np2argv[i][0];
		if ((c == '/') || (c == '-')) {
			switch(np2argv[i][1]) {
				case 'F':
				case 'f':
					np2arg.fullscreen = TRUE;
					break;
				case 'I':
				case 'i':
					if (!np2arg.ini) {
						np2arg.ini = np2argv[i];
					}
					break;
			}
		}
		else {														// ver0.29
			p = file_getext(np2argv[i]);
			if (!file_cmpname(p, "INI")) {
				if (!np2arg.ini) {
					np2arg.ini = np2argv[i];
				}
			}
			else {
				if (drv < 4) {
					np2arg.disk[drv++] = np2argv[i];
				}
			}
		}
	}
}

