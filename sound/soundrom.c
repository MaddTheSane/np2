#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"memory.h"
#include	"soundrom.h"


	SOUNDROM	soundrom;


static const char file_sound[] = "sound";
static const char file_extrom[] = ".rom";
static const BYTE defsoundrom[9] = {
							0x01,0x00,0x00,0x00,0xd2,0x00,0x08,0x00,0xcb};


static BOOL loadsoundrom(UINT address, const char *name) {

	char	romname[24];
	char	path[MAX_PATH];
	FILEH	fh;
	UINT	rsize;

	file_cpyname(romname, file_sound, sizeof(romname));
	if (name) {
		file_catname(romname, name, sizeof(romname));
	}
	file_catname(romname, file_extrom, sizeof(romname));
	getbiospath(path, romname, sizeof(path));
	fh = file_open_rb(path);
	if (fh == FILEH_INVALID) {
		goto lsr_err;
	}
	rsize = file_read(fh, mem + address, 0x4000);
	file_close(fh);
	if (rsize != 0x4000) {
		goto lsr_err;
	}
	file_cpyname(soundrom.name, romname, sizeof(soundrom.name));
	soundrom.address = address;
	return(SUCCESS);

lsr_err:
	return(FAILURE);
}


// ----

void soundrom_reset(void) {

	ZeroMemory(&soundrom, sizeof(soundrom));
}

void soundrom_load(UINT32 address, const char *primary) {

	if (primary != NULL) {
		if (loadsoundrom(address, primary) == SUCCESS) {
			return;
		}
	}
	if (loadsoundrom(address, NULL) == SUCCESS) {
		return;
	}
	CopyMemory(mem + address + 0x2e00, defsoundrom, sizeof(defsoundrom));
	soundrom.name[0] = '\0';
	soundrom.address = address;
}

void soundrom_loadex(BYTE sw, const char *primary) {

	if (sw < 4) {
		soundrom_load((0xc8000 + ((UINT32)sw << 14)), primary);
	}
	else {
		ZeroMemory(&soundrom, sizeof(soundrom));
	}
}

