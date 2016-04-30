enum {
	VFDD_TRKMAX		= 160,
	VFDD_SECMAX		= 26,
	VFDD_HEADERSIZE	= 220 + (12 * VFDD_TRKMAX * VFDD_SECMAX) + 32,
};

#if defined(__GNUC__)
//	���z�e�c�c�w�b�_�[��
typedef struct {
	SINT8	verID[8];		//	�o�[�W������\���h�c
	SINT8	memo[128];		//	�f�B�X�N����
	SINT16	write_protect;	//	�������݋֎~
	SINT16	spdrv;			//	����ǂݍ��݃h���C�u
	SINT8	dmy[80];		//	�\��̈�
} __attribute__ ((packed)) _VFDD_HEAD, *VFDD_HEAD;

//	���z�e�c�c�̂h�c�f�[�^
typedef struct {
	UINT8	C;			//	�b �V�����_�ԍ�
	UINT8	H;			//	�g �T�[�t�F�[�X�ԍ�
	UINT8	R;			//	�q �Z�N�^�ԍ�
	UINT8	N;			//	�m �Z�N�^��
	UINT8	D;			//	�c �f�[�^�p�^�[��
	UINT8	DDAM;		//	�c�c�`�l �f���[�e�b�h�f�[�^�t���O
	UINT8	flMF;		//	�l�e �{���x�t���O
	UINT8	flHD;		//	�Q�g�c�t���O
	SINT32	dataPoint;	//	�f�[�^�ւ̃t�@�C���|�C���^
} __attribute__ ((packed)) _VFDD_ID, *VFDD_ID;

/*	����ǂݍ��ݎ��̃f�[�^	*/
typedef struct {
	SINT16	trk;		//	���݂̃g���b�N�ʒu
	UINT16	iax;		//	���̓��W�X�^�̒l
	UINT16	ibx;
	UINT16	icx;
	UINT16	idx;
	UINT16	oax;		//	�o�̓��W�X�^�̒l
	UINT16	obx;
	UINT16	ocx;
	UINT16	odx;
	UINT16	ofl;
	SINT32	dataPoint;	//	�f�[�^�ւ̃t�@�C���|�C���^
	SINT32	nextPoint;	//	���̃f�[�^�ւ̃t�@�C���|�C���^
	SINT16	count;		//	����ǂݍ��݃J�E���^
	SINT16	neg_count;	//	�J�E���g�����t���O
} __attribute__ ((packed)) _VFDD_SP, *VFDD_SP;
#else
#pragma pack(push, 1)
//	���z�e�c�c�w�b�_�[��
typedef struct {
	SINT8	verID[8];		//	�o�[�W������\���h�c
	SINT8	memo[128];		//	�f�B�X�N����
	SINT16	write_protect;	//	�������݋֎~
	SINT16	spdrv;			//	����ǂݍ��݃h���C�u
	SINT8	dmy[80];		//	�\��̈�
} _VFDD_HEAD, *VFDD_HEAD;

//	���z�e�c�c�̂h�c�f�[�^
typedef struct {
	UINT8	C;			//	�b �V�����_�ԍ�
	UINT8	H;			//	�g �T�[�t�F�[�X�ԍ�
	UINT8	R;			//	�q �Z�N�^�ԍ�
	UINT8	N;			//	�m �Z�N�^��
	UINT8	D;			//	�c �f�[�^�p�^�[��
	UINT8	DDAM;		//	�c�c�`�l �f���[�e�b�h�f�[�^�t���O
	UINT8	flMF;		//	�l�e �{���x�t���O
	UINT8	flHD;		//	�Q�g�c�t���O
	SINT32	dataPoint;	//	�f�[�^�ւ̃t�@�C���|�C���^
} _VFDD_ID, *VFDD_ID;

/*	����ǂݍ��ݎ��̃f�[�^	*/
typedef struct {
	SINT16	trk;		//	���݂̃g���b�N�ʒu
	UINT16	iax;		//	���̓��W�X�^�̒l
	UINT16	ibx;
	UINT16	icx;
	UINT16	idx;
	UINT16	oax;		//	�o�̓��W�X�^�̒l
	UINT16	obx;
	UINT16	ocx;
	UINT16	odx;
	UINT16	ofl;
	SINT32	dataPoint;	//	�f�[�^�ւ̃t�@�C���|�C���^
	SINT32	nextPoint;	//	���̃f�[�^�ւ̃t�@�C���|�C���^
	SINT16	count;		//	����ǂݍ��݃J�E���^
	SINT16	neg_count;	//	�J�E���g�����t���O
} _VFDD_SP, *VFDD_SP;
#pragma pack(pop)
#endif

