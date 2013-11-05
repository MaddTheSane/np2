
#ifdef  __cplusplus
extern "C"
{
#endif

void recvideo_initialize(void);
void recvideo_deinitialize(void);
void recvideo_open(LPCTSTR lpcszFilename);
void recvideo_close(void);
void recvideo_write(void);
void recvideo_update(void);

#ifdef  __cplusplus
}
#endif

