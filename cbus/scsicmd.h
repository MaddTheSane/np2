
#if defined(SUPPORT_SCSI)

#ifdef __cplusplus
extern "C" {
#endif

REG8 scsicmd_negate(REG8 id);
REG8 scsicmd_select(REG8 id);
REG8 scsicmd_transfer(REG8 id, BYTE *cdb);
BOOL scsicmd_send(void);

void scsicmd_bios(void);

#ifdef __cplusplus
}
#endif

#endif

