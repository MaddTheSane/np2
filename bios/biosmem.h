
enum {
	MEMB_EXPMMSZ			= 0x00401,
	MEMB_SYS_TYPE			= 0x00480,
	MEMB_DISK_EQUIPS		= 0x00482,

	MEMB_BIOS_FLAG0			= 0x00500,
	MEMB_BIOS_FLAG1			= 0x00501,
	MEMB_KB_COUNT			= 0x00528,
	MEMB_KB_RETRY			= 0x00529,
	MEMB_SHIFT_STS			= 0x0053a,
	MEMB_CRT_RASTER			= 0x0053b,
	MEMB_CRT_STS_FLAG		= 0x0053c,
	MEMB_PRXCRT				= 0x0054c,
	MEMB_RS_S_FLAG			= 0x0055b,
	MEMB_PRXDUPD			= 0x0054d,
	MEMB_DISK_BOOT			= 0x00584,
	MEMB_RS_D_FLAG			= 0x005c1,
	MEMB_F2DD_MODE			= 0x005ca,

	MEMB_MSW				= 0xa3fe2,
	MEMB_MSW1				= 0xa3fe2,
	MEMB_MSW2				= 0xa3fe6,
	MEMB_MSW3				= 0xa3fea,
	MEMB_MSW4				= 0xa3fee,
	MEMB_MSW5				= 0xa3ff2,
	MEMB_MSW6				= 0xa3ff6,
	MEMB_MSW7				= 0xa3ffa,
	MEMB_MSW8				= 0xa3ffe
};

enum {
	MEMW_KB_BUF				= 0x00502,
	MEMW_KB_SHIFT_TBL		= 0x00522,
	MEMW_KB_BUF_HEAD		= 0x00524,
	MEMW_KB_BUF_TAIL		= 0x00526,
	MEMW_RS_CH0_OFST		= 0x00556,
	MEMW_RS_CH0_SEG			= 0x00558,
	MEMW_DISK_EQUIP			= 0x0055c,
	MEMW_CA_TIM_CNT			= 0x0058a,
	MEMW_KB_CODE_OFF		= 0x005c6,
	MEMW_KB_CODE_SEG		= 0x005c8,
	MEMW_F2DD_P_OFF			= 0x005cc,
	MEMW_F2DD_P_SEG			= 0x005ce,
	MEMW_F2HD_P_OFF			= 0x005f8,
	MEMW_F2HD_P_SEG			= 0x005fa
};

enum {
	MEMD_F2DD_POINTER		= 0x005cc,
	MEMD_F2HD_POINTER		= 0x005f8
};


// #define	GETBIOSMEM8(a)		(mem[(a)])
// #define	SETBIOSMEM8(a, b)	mem[(a)] = (b)

#if defined(BYTESEX_LITTLE)

#define	GETBIOSMEM16(a)		(*(UINT16 *)(mem + (a)))
#define	SETBIOSMEM16(a, b)	*(UINT16 *)(mem + (a)) = (b)

#define	GETBIOSMEM32(a)		(*(UINT32 *)(mem + (a)))
#define	SETBIOSMEM32(a, b)	*(UINT32 *)(mem + (a)) = (b)

#elif defined(BYTESEX_BIG)

#define	GETBIOSMEM16(a)		((UINT16)(mem[(a)+0] + (mem[(a)+1] << 8)))
#define	SETBIOSMEM16(a, b)	mem[(a)+0] = (BYTE)(b);				\
							mem[(a)+1] = (BYTE)((b) >> 8)

#define	GETBIOSMEM32(a)		((UINT32)(mem[(a)+0] + (mem[(a)+1] << 8) +	\
							(mem[(a)+2] << 16) + (mem[(a)+3] << 24)))
#define	SETBIOSMEM32(a, b)	mem[(a)+0] = (BYTE)(b);				\
							mem[(a)+1] = (BYTE)((b) >> 8);		\
							mem[(a)+2] = (BYTE)((b) >> 16);		\
							mem[(a)+3] = (BYTE)((b) >> 24)

#endif

