#define		BIOS_SEG	0xfd80
#define		BIOS_BASE	(BIOS_SEG << 4)

// #define	BIOS_OFF	0x0100
// #define	BIOS_ADR	((BIOS_SEG << 4) + BIOS_OFF)


enum {	// オフセット～
	BIOSOFST_IRET	= 0x0100,					// IRET
	BIOSOFST_EOIM	= 0x0102,					// EOI Master
	BIOSOFST_EOIS	= 0x0104,					// EOI Slave

	BIOSOFST_02		= 0x0106,					// NMI
	BIOSOFST_08		= 0x0108,					// PIT
	BIOSOFST_09		= 0x010a,					// Keyboard
	BIOSOFST_0c		= 0x010c,					// Serial

	BIOSOFST_12		= 0x010e,					// FDC
	BIOSOFST_13		= 0x0110,					// FDC

	BIOSOFST_18		= 0x0112,					// Common
	BIOSOFST_19		= 0x0114,					// RS-232C
	BIOSOFST_1a		= 0x0116,					// Printer
	BIOSOFST_1b		= 0x0118,					// Disk
	BIOSOFST_1c		= 0x011a,					// Timer
	BIOSOFST_1e		= 0x0000,					// BASIC

	BIOSOFST_PRT	= (BIOSOFST_1a + 0x19),		// Printer Main
	BIOSOFST_WAIT	= 0x0140					// FDD waiting
};


#ifdef __cplusplus
extern "C" {
#endif

extern	BOOL	biosrom;


void bios_init(void);
UINT MEMCALL biosfunc(UINT32 adrs);

void bios0x02(void);

void bios0x08(void);

void bios0x09(void);
void bios0x09_init(void);

void bios0x0c(void);

void bios0x12(void);
void bios0x13(void);

void bios0x18(void);
void bios0x18_16(BYTE chr, BYTE atr);

void bios0x19(void);

void bios0x1a(void);
void bios0x1a_main(void);											// ver0.30

void bios0x1b(void);

UINT16 bootstrapload(void);

void bios0x1c(void);
void bios0x1c_03(void);

#ifdef __cplusplus
}
#endif

