
// ---- com manager midi for windows

typedef struct {
	BYTE	prog;
	BYTE	press;
	UINT16	bend;
	BYTE	ctrl[28];
} _MIDICH, *MIDICH;


#ifdef __cplusplus
extern "C" {
#endif

extern const char cmmidi_midimapper[];
#if defined(VERMOUTH_LIB)
extern const char cmmidi_vermouth[];
#endif
extern const char *cmmidi_mdlname[12];

void cmmidi_initailize(void);
COMMNG cmmidi_create(const char *midiout, const char *midiin,
														const char *module);
void cmmidi_recvdata(HMIDIIN hdr, UINT32 data);
void cmmidi_recvexcv(HMIDIIN hdr, MIDIHDR *data);

#ifdef __cplusplus
}
#endif

