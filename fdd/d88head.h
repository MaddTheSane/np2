
#if defined(__GNUC__)
// D88ヘッダ (size: 2b0h bytes)
typedef struct {
	BYTE	fd_name[17];		// Disk Name
	BYTE	reserved1[9]; 		// Reserved
	BYTE	protect;			// Write Protect bit:4
	BYTE	fd_type;			// Disk Format
	BYTE	fd_size[4];			// Disk Size
	BYTE	trackp[164][4];		// Track_pointer
} __attribute__ ((packed)) _D88HEAD, *D88HEAD;

// D88セクタ (size: 16bytes)
typedef struct {
	BYTE	c;
	BYTE	h;
	BYTE	r;
	BYTE	n;
	BYTE	sectors[2];			// Sector Count
	BYTE	mfm_flg;			// sides
	BYTE	del_flg;			// DELETED DATA
	BYTE	stat;				// STATUS (FDC ret)
	BYTE	seektime;			// Seek Time
	BYTE	reserved[3];		// Reserved
	BYTE	rpm_flg;			// rpm			0:1.2  1:1.44
	BYTE	size[2];			// Sector Size
} __attribute__ ((packed)) _D88SEC, *D88SEC, D88_SECTOR;
#else
#pragma pack(push, 1)
// D88ヘッダ (size: 2b0h bytes)
typedef struct {
	BYTE	fd_name[17];		// Disk Name
	BYTE	reserved1[9]; 		// Reserved
	BYTE	protect;			// Write Protect bit:4
	BYTE	fd_type;			// Disk Format
	BYTE	fd_size[4];			// Disk Size
	BYTE	trackp[164][4];		// Track_pointer
} _D88HEAD, *D88HEAD;

// D88セクタ (size: 16bytes)
typedef struct {
	BYTE	c;
	BYTE	h;
	BYTE	r;
	BYTE	n;
	BYTE	sectors[2];			// Sector Count
	BYTE	mfm_flg;			// sides
	BYTE	del_flg;			// DELETED DATA
	BYTE	stat;				// STATUS (FDC ret)
	BYTE	seektime;			// Seek Time
	BYTE	reserved[3];		// Reserved
	BYTE	rpm_flg;			// rpm			0:1.2  1:1.44
	BYTE	size[2];			// Sector Size
} _D88SEC, *D88SEC, D88_SECTOR;
#pragma pack(pop)
#endif

