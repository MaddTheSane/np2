#include	"compiler.h"
#include	"dosio.h"
#include	"newdisk.h"
#include	"fddfile.h"
#include	"hddboot.res"


void newdisk_hdd(const char *fname, UINT hddsize) {

	FILEH	fh;
	BYTE	work[256];
	UINT	size;
	UINT	wsize;

	if ((fname == NULL) || (hddsize < 5) || (hddsize > 256)) {
		return;
	}
	fh = file_create(fname);
	if (fh != FILEH_INVALID) {
		ZeroMemory(work, 256);
		size = hddsize * 15;
		STOREINTELWORD(work, size);
		file_write(fh, work, 256);
		file_write(fh, hdddiskboot, sizeof(hdddiskboot));
		ZeroMemory(work, sizeof(work));
		size = 0x400 - sizeof(hdddiskboot);
		while(size) {
			wsize = min(size, sizeof(work));
			size -= wsize;
			file_write(fh, work, wsize);
		}
		file_close(fh);
	}
}

void newdisk_fdd(const char *fname, BYTE type, const char *label) {

	_D88HEAD	d88head;
	FILEH		fh;

	ZeroMemory(&d88head, sizeof(d88head));
	STOREINTELDWORD(d88head.fd_size, sizeof(d88head));
	milstr_ncpy((char *)d88head.fd_name, label, sizeof(d88head.fd_name));
	d88head.fd_type = type;
	fh = file_create(fname);
	if (fh != FILEH_INVALID) {
		file_write(fh, &d88head, sizeof(d88head));
		file_close(fh);
	}
}

