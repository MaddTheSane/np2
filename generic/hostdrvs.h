
// �ꗗ�擾
LISTARRAY hostdrvs_getpathlist(const char *realpath);

// �z�X�g���̃t�@�C�����𓾂�
BOOL hostdrvs_getrealpath(HDRVPATH *hdp, char *dospath);

// �z�X�g���̃t�@�C���������
BOOL hostdrvs_newrealpath(HDRVPATH *hdp, char *dospath);

// �t�@�C���n���h�����X�g
void hostdrvs_fhdlreopen(LISTARRAY fhdl);
void hostdrvs_fhdlallclose(LISTARRAY fhdl);
HDRVFILE hostdrvs_fhdlsea(LISTARRAY fhdl);

