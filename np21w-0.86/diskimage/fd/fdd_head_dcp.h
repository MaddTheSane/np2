enum {
	DCP_TRACKMAX		= 160,
	DCP_HEADERSIZE		= 0x01 + DCP_TRACKMAX + 0x01,

	DCP_DISK_2HD_08		= 0x01,	//	2HD- 8�Z�N�^(1.25MB)
	DCP_DISK_2HD_15		= 0x02,	//	2HD-15�Z�N�^(1.21MB)
	DCP_DISK_2HQ_18		= 0x03,	//	2HQ-18�Z�N�^(1.44MB)
	DCP_DISK_2DD_08		= 0x04,	//	2DD- 8�Z�N�^( 640KB)
	DCP_DISK_2DD_09		= 0x05,	//	2DD- 9�Z�N�^( 720KB)
	DCP_DISK_2HD_09		= 0x08,	//	2HD- 9�Z�N�^(1.44MB)
	DCP_DISK_2HD_BAS	= 0x11,	//	2HD-BASIC
	DCP_DISK_2DD_BAS	= 0x19,	//	2DD-BASIC
	DCP_DISK_2HD_26		= 0x21	//	2HD-26�Z�N�^
};

#if defined(__GNUC__)
//	DCP�w�b�_ (size: a2h bytes)
typedef struct {
	UINT8	mediatype;
	UINT8	trackmap[DCP_TRACKMAX];
	UINT8	alltrackflg;
} __attribute__ ((packed)) _DCPHEAD, *DCPHEAD;
#else
#pragma pack(push, 1)
//	DCP�w�b�_ (size: a2h bytes)
typedef struct {
	UINT8	mediatype;
	UINT8	trackmap[DCP_TRACKMAX];
	UINT8	alltrackflg;
} _DCPHEAD, *DCPHEAD;
#pragma pack(pop)
#endif

