
typedef struct {
	long	fh;
	long	fhpos;
	UINT	buffersize;
	UINT	flag;
	UINT	pos;
	UINT	remain;
} _TEXTFILE, *TEXTFILEH;


#ifdef __cplusplus
extern "C" {
#endif

TEXTFILEH textfile_open(const OEMCHAR *filename, UINT buffersize);
BRESULT textfile_read(TEXTFILEH fh, OEMCHAR *buffer, UINT size);
void textfile_close(TEXTFILEH fh);

#ifdef __cplusplus
}
#endif

