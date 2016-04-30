enum {
	NFD_TRKMAX		= 163,
	NFD_TRKMAX1		= 164,
	NFD_SECMAX		= 26,
	NFD_HEADERSIZE	= 288 + (16 * NFD_TRKMAX * NFD_SECMAX) + 0x10,
	NFD_HEADERSIZE1	= 288 + (16 * NFD_TRKMAX * NFD_SECMAX) + 0x10,
};

#if defined(__GNUC__)
//	�Z�N�^ID
typedef struct {
	BYTE	C;									//	C�i0xFF�̎��Z�N�^�����j
	BYTE	H;									//	H
	BYTE	R;									//	R
	BYTE	N;									//	N
	BYTE	flMFM;								//	0:FM / 1:MFM
	BYTE	flDDAM;								//	0:DAM / 1:DDAM
	BYTE	byStatus;							//	READ DATA(FDDBIOS)�̌���
	BYTE	byST0;								//	READ DATA(FDDBIOS)�̌��� ST0
	BYTE	byST1;								//	READ DATA(FDDBIOS)�̌��� ST1
	BYTE	byST2;								//	READ DATA(FDDBIOS)�̌��� ST2
	BYTE	byPDA;								//	FDDBIOS�Ŏg�p����A�h���X
	char	Reserve1[5];						//	�\��
} __attribute__ ((packed)) NFD_SECT_ID, *LP_NFD_SECT_ID;

//	nfd�w�b�_(r0)
typedef struct {
	char		szFileID[15];					//	����ID "T98FDDIMAGE.R0"
	char		Reserve1[1];					//	�\��
	char		szComment[0x100];				//	�C���[�W�R�����g(ASCIIz)
	DWORD		dwHeadSize;						//	�w�b�_���̃T�C�Y
	BYTE		flProtect;						//	0�ȊO : ���C�g�v���e�N�g
	BYTE		byHead;							//	�w�b�h��
	char		Reserve2[10];					//	�\��
	NFD_SECT_ID	si[NFD_TRKMAX][NFD_SECMAX];		//	�Z�N�^ID
	char		Reserve3[0x10];					//	�\��
} __attribute__ ((packed)) NFD_FILE_HEAD, *LP_NFD_FILE_HEAD;

//	nfd�w�b�_(r1)
typedef struct {
//	char	szFileID[sizeof(NFD_FILE_ID1)];		//	����ID	"T98FDDIMAGE.R1"
//	char	Reserv1[0x10-sizeof(NFD_FILE_ID1)];	//	�\��
	char	szFileID[15];						//	����ID	"T98FDDIMAGE.R1"
	char	Reserv1[1];							//	�\��
	char	szComment[0x100];					//	�R�����g
	DWORD	dwHeadSize;							//	�w�b�_�̃T�C�Y
	BYTE	flProtect;							//	���C�g�v���e�N�g0�ȊO
	BYTE	byHead;								//	�w�b�h��	1-2
	char	Reserv2[0x10-4-1-1];				//	�\��
	DWORD	dwTrackHead[NFD_TRKMAX1];			//	�g���b�NID�ʒu
	DWORD	dwAddInfo;							//	�ǉ����w�b�_�̃A�h���X
	char	Reserv3[0x10-4];					//	�\��
} __attribute__ ((packed)) NFD_FILE_HEAD1, *LP_NFD_FILE_HEAD1;

//	�g���b�NID
typedef struct {
	WORD	wSector;							//	�Z�N�^ID��
	WORD	wDiag;								//	���@��ID��
	char	Reserv1[0x10-4];					//	�\��
} __attribute__ ((packed)) NFD_TRACK_ID1, *LP_NFD_TRACK_ID1;

//	�Z�N�^���w�b�_
typedef struct {
	BYTE	C;									//	C
	BYTE	H;									//	H
	BYTE	R;									//	R
	BYTE	N;									//	N
	BYTE	flMFM;								//	MFM(1)/FM(0)
	BYTE	flDDAM;								//	DDAM(1)/DAM(0)
	BYTE	byStatus;							//	READ DATA RESULT
	BYTE	bySTS0;								//	ST0
	BYTE	bySTS1;								//	ST1
	BYTE	bySTS2;								//	ST2
	BYTE	byRetry;							//	RetryData�Ȃ�(0)����(1-)
	BYTE	byPDA;								//	PDA
	char	Reserv1[0x10-12];					//	�\��
} __attribute__ ((packed)) NFD_SECT_ID1, *LP_NFD_SECT_ID1;

//	����ǂݍ��ݏ��w�b�_
typedef struct {
	BYTE	Cmd;								//	Command
	BYTE	C;									//	C
	BYTE	H;									//	H
	BYTE	R;									//	R
	BYTE	N;									//	N
	BYTE	byStatus;							//	READ DATA RESULT
	BYTE	bySTS0;								//	ST0
	BYTE	bySTS1;								//	ST1
	BYTE	bySTS2;								//	ST2
	BYTE	byRetry;							//	RetryData�Ȃ�(0)����(1-)
	DWORD	dwDataLen;							//	�]�����s���f�[�^�T�C�Y
	BYTE	byPDA;								//	PDA
	char	Reserv1[0x10-15];					//	�\��
} __attribute__ ((packed)) NFD_DIAG_ID1, *LP_NFD_DIAG_ID1;
#else
#pragma pack(push, 1)
//	�Z�N�^ID
typedef struct {
	BYTE	C;									//	C�i0xFF�̎��Z�N�^�����j
	BYTE	H;									//	H
	BYTE	R;									//	R
	BYTE	N;									//	N
	BYTE	flMFM;								//	0:FM / 1:MFM
	BYTE	flDDAM;								//	0:DAM / 1:DDAM
	BYTE	byStatus;							//	READ DATA(FDDBIOS)�̌���
	BYTE	byST0;								//	READ DATA(FDDBIOS)�̌��� ST0
	BYTE	byST1;								//	READ DATA(FDDBIOS)�̌��� ST1
	BYTE	byST2;								//	READ DATA(FDDBIOS)�̌��� ST2
	BYTE	byPDA;								//	FDDBIOS�Ŏg�p����A�h���X
	char	Reserve1[5];						//	�\��
} NFD_SECT_ID, *LP_NFD_SECT_ID;

//	nfd�w�b�_(r0)
typedef struct {
	char		szFileID[15];					//	����ID	"T98FDDIMAGE.R0"
	char		Reserve1[1];					//	�\��
	char		szComment[0x100];				//	�C���[�W�R�����g(ASCIIz)
	DWORD		dwHeadSize;						//	�w�b�_���̃T�C�Y
	BYTE		flProtect;						//	0�ȊO : ���C�g�v���e�N�g
	BYTE		byHead;							//	�w�b�h��
	char		Reserve2[10];					//	�\��
	NFD_SECT_ID	si[NFD_TRKMAX][NFD_SECMAX];		//	�Z�N�^ID
	char		Reserve3[0x10];					//	�\��
} NFD_FILE_HEAD, *LP_NFD_FILE_HEAD;

//	nfd�w�b�_(r1)
typedef struct {
//	char	szFileID[sizeof(NFD_FILE_ID1)];		//	����ID	"T98FDDIMAGE.R1"
//	char	Reserv1[0x10-sizeof(NFD_FILE_ID1)];	//	�\��
	char	szFileID[15];						//	����ID	"T98FDDIMAGE.R1"
	char	Reserv1[1];							//	�\��
	char	szComment[0x100];					//	�R�����g
	DWORD	dwHeadSize;							//	�w�b�_�̃T�C�Y
	BYTE	flProtect;							//	���C�g�v���e�N�g0�ȊO
	BYTE	byHead;								//	�w�b�h��	1-2
	char	Reserv2[0x10-4-1-1];				//	�\��
	DWORD	dwTrackHead[NFD_TRKMAX1];			//	�g���b�NID�ʒu
	DWORD	dwAddInfo;							//	�ǉ����w�b�_�̃A�h���X
	char	Reserv3[0x10-4];					//	�\��
} NFD_FILE_HEAD1, *LP_NFD_FILE_HEAD1;

//	�g���b�NID
typedef struct {
	WORD	wSector;							//	�Z�N�^ID��
	WORD	wDiag;								//	���@��ID��
	char	Reserv1[0x10-4];					//	�\��
} NFD_TRACK_ID1, *LP_NFD_TRACK_ID1;

//	�Z�N�^���w�b�_
typedef struct {
	BYTE	C;									//	C
	BYTE	H;									//	H
	BYTE	R;									//	R
	BYTE	N;									//	N
	BYTE	flMFM;								//	MFM(1)/FM(0)
	BYTE	flDDAM;								//	DDAM(1)/DAM(0)
	BYTE	byStatus;							//	READ DATA RESULT
	BYTE	bySTS0;								//	ST0
	BYTE	bySTS1;								//	ST1
	BYTE	bySTS2;								//	ST2
	BYTE	byRetry;							//	RetryData�Ȃ�(0)����(1-)
	BYTE	byPDA;								//	PDA
	char	Reserv1[0x10-12];					//	�\��
} NFD_SECT_ID1, *LP_NFD_SECT_ID1;

//	����ǂݍ��ݏ��w�b�_
typedef struct {
	BYTE	Cmd;								//	Command
	BYTE	C;									//	C
	BYTE	H;									//	H
	BYTE	R;									//	R
	BYTE	N;									//	N
	BYTE	byStatus;							//	READ DATA RESULT
	BYTE	bySTS0;								//	ST0
	BYTE	bySTS1;								//	ST1
	BYTE	bySTS2;								//	ST2
	BYTE	byRetry;							//	RetryData�Ȃ�(0)����(1-)
	DWORD	dwDataLen;							//	�]�����s���f�[�^�T�C�Y
	BYTE	byPDA;								//	PDA
	char	Reserv1[0x10-15];					//	�\��
} NFD_DIAG_ID1, *LP_NFD_DIAG_ID1;
#pragma pack(pop)
#endif

