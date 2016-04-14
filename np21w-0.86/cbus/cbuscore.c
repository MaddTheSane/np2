#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"ideio.h"
#include	"sasiio.h"
#include	"scsiio.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#ifdef SUPPORT_NET
#include	"network/net.h"
#endif
#ifdef SUPPORT_LGY98
#include	"network/lgy98.h"
#endif
#ifdef SUPPORT_WAB
#include	"wab/wab.h"
#endif
#ifdef SUPPORT_CL_GD5430
#include	"wab/cirrus_vga_extern.h"
#endif


static const FNIORESET resetfn[] = {
#if defined(SUPPORT_IDEIO)
			ideio_reset,
#endif
#if defined(SUPPORT_NET)
			np2net_reset,
#endif
#if defined(SUPPORT_LGY98)
			lgy98_reset,
#endif
#if defined(SUPPORT_WAB)
			np2wab_reset,
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
			mpu98ii_reset,
	};

static const FNIOBIND bindfn[] = {
#if defined(SUPPORT_IDEIO)
			ideio_bind,
#endif
#if defined(SUPPORT_NET)
			np2net_bind,
#endif
#if defined(SUPPORT_LGY98)
			lgy98_bind,
#endif
#if defined(SUPPORT_WAB)
			np2wab_bind,
#endif
#if defined(SUPPORT_CL_GD5430)
			pc98_cirrus_vga_bind,
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
			mpu98ii_bind,
	};


void cbuscore_reset(const NP2CFG *pConfig) {

	iocore_cbreset(resetfn, NELEMENTS(resetfn), pConfig);
}

void cbuscore_bind(void) {

	iocore_cbbind(bindfn, NELEMENTS(bindfn));
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

