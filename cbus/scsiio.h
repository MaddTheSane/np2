
#if defined(SUPPORT_SCSI)

typedef struct {
	UINT	port;
	UINT	phase;
	BYTE	reg[0x30];
	UINT8	auxstatus;
	UINT8	scsistatus;
	UINT8	membank;
	UINT8	memwnd;
	UINT8	resent;
	UINT8	datmap;
	UINT	cmdpos;
	UINT	wrdatpos;
	UINT	rddatpos;
	BYTE	cmd[12];
	BYTE	data[0x10000];
	BYTE	bios[2][0x2000];
} _SCSIIO, *SCSIIO;


#ifdef __cplusplus
extern "C" {
#endif

extern	_SCSIIO		scsiio;

void scsiioint(NEVENTITEM item);

void scsiio_reset(void);
void scsiio_bind(void);

#ifdef __cplusplus
}
#endif

#endif

