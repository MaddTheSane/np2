
typedef struct {
	char	*disk[4];
	char	*ini;
	BOOL	fullscreen;
} NP2ARG_T;

extern NP2ARG_T	np2arg;

void np2arg_analize(char *argstr);
