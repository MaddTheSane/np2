#include	"compiler.h"
#include	"np2arg.h"
#include	"oemtext.h"
#include	"dosio.h"

												// ver0.26 np2arg.cpp append
// �R�}���h���C���̉��

// OS�ˑ��������Ȃ̂Ł@�؂蕪���܂��B


#define	MAXARG		32
#define	ARG_BASE	1				// win32��lpszCmdLine�̏ꍇ
									// ���s�t�@�C��������

	NP2ARG_T	np2arg = {{NULL, NULL, NULL, NULL},
							NULL, FALSE};

// ---------------------------------------------------------------------

static	OEMCHAR	argstrtmp[2048];

void np2arg_analize(void) {

	int			np2argc;
	OEMCHAR		*np2argv[MAXARG];
	int			i;
	int			drv = 0;
	OEMCHAR		c;
const OEMCHAR	*p;

#if defined(OSLANG_UTF8)
	tchartooem(argstrtmp, NELEMENTS(argstrtmp), GetCommandLine(), -1);
#else
	milstr_ncpy(argstrtmp, GetCommandLine(), NELEMENTS(argstrtmp));
#endif
	np2argc = milstr_getarg(argstrtmp, np2argv, NELEMENTS(np2argv));

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
			if (!file_cmpname(p, OEMTEXT("ini"))) {
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

