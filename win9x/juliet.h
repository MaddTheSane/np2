BOOL juliet_load(void);
void juliet_unload(void);

ULONG juliet_prepare(void);

void juliet_YM2151Reset(void);
int juliet_YM2151IsEnable(void);
int juliet_YM2151IsBusy(void);
void juliet_YM2151W(BYTE reg, BYTE data);

void juliet_YMF288Reset(void);
int juliet_YMF288Enable(void);
int juliet_YMF288IsBusy(void);
void juliet_YMF288A(BYTE addr, BYTE data);
void juliet_YMF288B(BYTE addr, BYTE data);
