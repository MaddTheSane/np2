#include	"compiler.h"
#include	"codecnv.h"
#include	"textcnv.h"


BRESULT textcnv_tooem(UINT code, TCTOOEM *tcto) {

	TCTOOEM	fn;

	fn = NULL;
#if defined(OSLANG_SJIS)
	switch(code) {
		case TEXTCNV_DEFAULT:
		case TEXTCNV_SJIS:
			break;

		default:
			return(FAILURE);
	}
#elif defined(OSLANG_EUC)
	switch(code) {
		case TEXTCNV_DEFAULT:
		case TEXTCNV_EUC:
			break;

		default:
			return(FAILURE);
	}
#elif defined(OSLANG_UTF8)
	switch(code) {
		case TEXTCNV_UTF8:
			break;

		case TEXTCNV_UCS2:
			fn = (TCTOOEM)codecnv_ucs2toutf8;
			break;

		default:
			return(FAILURE);
	}
#elif defined(OSLANG_UCS2)
	switch(code) {
		case TEXTCNV_UTF8:
			fn = (TCTOOEM)codecnv_utf8toucs2;
			break;

		case TEXTCNV_UCS2:
			break;

		default:
			return(FAILURE);
	}
#else
	switch(code) {
		case TEXTCNV_DEFAULT:
			break;

		default:
			return(FAILURE);
	}
#endif
	if (tcto) {
		*tcto = fn;
	}
	return(SUCCESS);
}

BRESULT textcnv_fromoem(UINT code, TCFROMOEM *tcfo) {

	return(FAILURE);
}

