/**
 * @file	recstat.h
 * @brief	Interface of the recording states
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

BOOL recstat_keyboard(REG8 data);
BOOL recstat_mouse(SINT16 sx, SINT16 sy, REG8 btn);
BOOL recstat_fdd(REG8 drv, const OEMCHAR *lpFilename, UINT ftype, int readonly);
BOOL recstat_joypad(int nPort, REG8* data);
BOOL recstat_sync(void);

void recstat_init(void);
void recstat_record(const OEMCHAR *lpFilename);
void recstat_continue(const OEMCHAR *lpFilename);
void recstat_play(const OEMCHAR *lpFilename);
void recstat_close(void);

#ifdef __cplusplus
}
#endif
