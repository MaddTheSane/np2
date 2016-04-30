
#ifdef __cplusplus
extern "C" {
#endif

enum {
	SXSIMEDIA_DATA = 0x10,
	SXSIMEDIA_AUDIO = 0x20
};

#ifdef SUPPORT_KAI_IMAGES

#define	TRACK_DATA	0x14
#define	TRACK_AUDIO	0x10

typedef struct {
	UINT8	adr_ctl;		//	Adr/Ctl
							//		ISO:0x14
							//		CUE:MODE1=0x14�AMODE2=0x14�AAUDIO=0x10
							//		CCD:
							//		CDM:
							//		MDS:MDS_TrackBlock.adr_ctl
	UINT8	point;			//	Track Number
							//		ISO:1
							//		CUE:TRACK ??=??
							//		CCD:
							//		CDM:
							//		MDS:MDS_TrackBlock.point
	UINT32	pos;			//	�g���b�N�̃C���[�W�t�@�C�����ł̊J�n�Z�N�^�ʒu
							//		ISO:0
							//		CUE:INDEX 1
							//		CCD:
							//		CDM:
							//		MDS:((MDS_TrackBlock.min * 60) + MDS_TrackBlock.sec) * 75 + MDS_TrackBlock.frame
//	--------
	UINT32	pos0;			//	CUE�V�[�g��"INDEX 00"���Ŏw�肳�ꂽPREGAP�̃C���[�W���ł̊J�n�Z�N�^�ʒu
							//		ISO:0
							//		CUE:INDEX 0
							//		CCD:
							//		CDM:
							//		MDS:0
	UINT32	str_sec;		//	�g���b�N�̃C���[�W�t�@�C����ł̊J�n�Z�N�^�ʒu
							//		ISO:
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:
	UINT32	end_sec;		//	�g���b�N�̃C���[�W�t�@�C����ł̏I���Z�N�^�ʒu
							//		ISO:
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:
	UINT32	sectors;		//	�g���b�N�̃C���[�W�t�@�C����ł̃Z�N�^��
							//		ISO:
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:
//	--------
	UINT16	sector_size;	//	�g���b�N�̃Z�N�^�T�C�Y
							//		ISO:2048 or 2352 or 2448(�t�@�C���T�C�Y�������ė]��̏o�Ȃ����l)
							//		CUE:MODE1/????=????�AMODE2/????=????�AAUDIO=2352
							//		CCD:2352(�Œ�Ő������H)
							//		CDM:2352(�Œ�Ő������H)
							//		MDS:MDS_TrackBlock.sector_size
							//		NRG:NRG_DAO_Block.sector_size

	//	CD��̊e�Z�N�^�J�n�ʒu
	//	���C���[�W�t�@�C�����PREGAP�̈����ɂ���Č�q�̃C���[�W�t�@�C����̊e�Z�N�^�J�n�ʒu��
	//	�@���ꂽ�l�ɂȂ邱�Ƃ�����
	UINT32	pregap_sector;	//	�g���b�N��PREGAP�J�n�Z�N�^�ʒu
							//	��PREGAP�������ꍇ��PREGAP�̎��̂������ꍇ��
							//	�@�g���b�N��start_sector�Ɠ����l
							//		ISO:0
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:start_sector - pregap_sectors
	UINT32	start_sector;	//	�g���b�N�̊J�n�Z�N�^�ʒu
							//		ISO:0
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:MDS_TrackBlock.start_sector(���g���G���f�B�A��)
	UINT32	end_sector;		//	�g���b�N�̏I���Z�N�^�ʒu
							//		ISO:track_sectors - 1
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:start_sector + track_sectors - 1

	//	�C���[�W�t�@�C����̊e�Z�N�^�J�n�ʒu
	UINT32	img_pregap_sec;	//	�g���b�N��PREGAP�J�n�Z�N�^�ʒu
							//	��PREGAP�������ꍇ��PREGAP�̎��̂������ꍇ��
							//	�@�g���b�N��start_sector�Ɠ����l
							//		ISO:0
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:
	UINT32	img_start_sec;	//	�g���b�N�̊J�n�Z�N�^�ʒu
							//		ISO:0
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:
	UINT32	img_end_sec;	//	�g���b�N�̏I���Z�N�^�ʒu
							//		ISO:track_sectors - 1
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:

	//	�e�Z�N�^�J�n�ʒu�̃C���[�W�t�@�C����ł�offset
	UINT64	pregap_offset;	//	�C���[�W�t�@�C����̃g���b�N��PREGAP��offset
							//	���ʏ�͑O�g���b�N��end_offset�Ɠ����l
							//	��PREGAP�������ꍇ��PREGAP�̎��̂������ꍇ��
							//	�@start_offset�Ɠ����l
							//		ISO:0
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:start_offset
	UINT64	start_offset;	//	�C���[�W�t�@�C����̃g���b�N�J�n�ʒu��offset
							//	��PREGAP�������ꍇ��PREGAP�̎��̂������ꍇ��
							//	�@�O�g���b�N��end_offset�Ɠ����l
							//		ISO:0
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:MDS_TrackBlock.start_offset(���g���G���f�B�A��)
	UINT64	end_offset;		//	�C���[�W�t�@�C����̃g���b�N�I���ʒu��offset
							//		ISO:track_sectors * sector_size
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:start_offset + (track_sectors * sector_size)

	UINT32	pregap_sectors;	//	�g���b�N��PREGAP�̃Z�N�^��
							//		ISO:0
							//		CUE:PREGAP
							//		CCD:
							//		CDM:
							//		MDS:MDS_TrackExtraBlock.pregap(���g���G���f�B�A��)
	UINT32	track_sectors;	//	�g���b�N�̃Z�N�^��
							//		ISO:�t�@�C���T�C�Y / sector_size
							//		CUE:
							//		CCD:
							//		CDM:
							//		MDS:MDS_TrackExtraBlock.length(���g���G���f�B�A��)
//	--------
} _CDTRK, *CDTRK;

#else
typedef struct {
	UINT8	type;
	UINT8	track;
	FILEPOS	pos;
} _CDTRK, *CDTRK;

#endif

BRESULT sxsicd_open(SXSIDEV sxsi, const OEMCHAR *fname);

CDTRK sxsicd_gettrk(SXSIDEV sxsi, UINT *tracks);
BRESULT sxsicd_readraw(SXSIDEV sxsi, FILEPOS pos, void *buf);

#ifdef __cplusplus
}
#endif

