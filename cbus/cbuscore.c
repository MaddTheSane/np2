#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"ideio.h"
#include	"sasiio.h"
#include	"scsiio.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"


static const IOCBFN resetfn[] = {
#if defined(SUPPORT_IDEIO)
			ideio_reset,
#endif
#if defined(SUPPORT_SASI)
			sasiio_reset,
#endif
#if defined(SUPPORT_SCSI)
			scsiio_reset,
#endif
#if defined(SUPPORT_PC9861K)
			pc9861k_reset,
#endif
			mpu98ii_reset};

static const IOCBFN bindfn[] = {
#if defined(SUPPORT_IDEIO)
			ideio_bind,
#endif
#if defined(SUPPORT_SASI)
			sasiio_bind,
#endif
#if defined(SUPPORT_SCSI)
			scsiio_bind,
#endif
#if defined(SUPPORT_PC9861K)
			pc9861k_bind,
#endif
			mpu98ii_bind};


void cbuscore_reset(void) {

	iocore_cb(resetfn, sizeof(resetfn)/sizeof(IOCBFN));
}

void cbuscore_bind(void) {

	iocore_cb(bindfn, sizeof(bindfn)/sizeof(IOCBFN));
}


void cbuscore_attachsndex(UINT port, const IOOUT *out, const IOINP *inp) {

	UINT	i;
	IOOUT	outfn;
	IOINP	inpfn;

	for (i=0; i<4; i++) {
		outfn = out[i];
		if (outfn) {
			iocore_attachsndout(port, outfn);
		}
		inpfn = inp[i];
		if (inpfn) {
			iocore_attachsndinp(port, inpfn);
		}
		port += 2;
	}
}

