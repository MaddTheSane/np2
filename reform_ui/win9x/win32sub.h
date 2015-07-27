
#ifdef __cplusplus
extern "C" {
#endif

void __msgbox(const char *title, const char *msg);
int loadstringresource(UINT uID, LPTSTR lpszBuffer, int nBufferMax);
LPTSTR lockstringresource(LPCTSTR lpcszString);
void unlockstringresource(LPTSTR lpszString);

#ifdef __cplusplus
}
#endif

