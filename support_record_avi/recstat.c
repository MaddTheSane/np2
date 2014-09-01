/**
 * @file	recstat.c
 * @brief	Implementation of the recording states
 */

#include "compiler.h"
#include "recstat.h"
#include "dosio.h"
#include "timemng.h"
#include "calendar.h"
#include "pccore.h"
#include "fdd/diskdrv.h"
#include "io/lsidef.h"
#include "io/mouseif.h"
#include "io/serial.h"
#include "sound/sound.h"
#include "sound/fmboard.h"

/**
 * Modes
 */
enum RecStatMode
{
	kRecStatNone		= 0,	/*!< nothing */
	kRecStatRecording,			/*!< recording */
	kRecStatPlaying				/*!< playing */
};

/**
 * @brief Recoding states structure
 */
struct _recstat
{
	enum RecStatMode mode;		/*!< mode */
	FILEH hFile;				/*!< file handle */
	UINT ptr;					/*!< buffer pointer */
	UINT size;					/*!< buffer size */
	UINT8 joypad[2];			/*!< pad registers */
	UINT8 buffer[8192];			/*!< buffer */
};
typedef struct _recstat		*RECSTAT;		/*!< defines recstat pointer */

/** Instance */
static struct _recstat s_recstat;

/**
 * Commands
 */
enum
{
	kStatTerminate		= 0,
	kStatNp2cfg,
	kStatCalendar,
	kStatKeyboard,
	kStatMouse,
	kStatFdd,
	kStatJoypad1,
	kStatJoypad2,

	kStatRerecording	= 254,
	kStatDelimiter		= 255
};

#pragma pack(push, 1)
/**
 * @brief Mouse status
 */
struct RecStatMouse
{
	UINT8 btn;		/*!< button */
	UINT16 sx;		/*!< moved x */
	UINT16 sy;		/*!< moved y */
};

/**
 * @brief FDD status
 */
struct RecStatFdd
{
	UINT8 drv;			/*!< drive */
	UINT8 readonly;		/*!< R/O */
	UINT ftype;			/*!< type */
	UINT cbFilename;	/*!< path bytes */
};
#pragma pack(pop)

/**
 * Flush the file buffer
 * @param[in] _this instance
 */
static void flushBuffer(RECSTAT _this)
{
	if (_this->mode == kRecStatRecording)
	{
		if (_this->size)
		{
			file_write(_this->hFile, _this->buffer, _this->size);
			_this->size = 0;
		}
	}
	else
	{
		_this->size -= _this->ptr;
		if (_this->size)
		{
			memmove(_this->buffer, _this->buffer + _this->ptr, _this->size);
		}

		_this->ptr = 0;
		_this->size += file_read(_this->hFile, _this->buffer + _this->size, sizeof(_this->buffer) - _this->size);
	}
}

/**
 * Reads data
 * @param[in] _this instance
 * @param[in] cbBuffer length
 */
static const void *read(RECSTAT _this, UINT cbBuffer)
{
	const void *ptr = NULL;

	if ((_this->size - _this->ptr) < cbBuffer)
	{
		flushBuffer(_this);
	}

	if ((_this->size - _this->ptr) >= cbBuffer)
	{
		ptr = _this->buffer + _this->ptr;
		_this->ptr += cbBuffer;
	}
	return ptr;
}

/**
 * Reads byte
 * @param[in] _this instance
 * @param[in] data data
 */
static UINT8 readByte(RECSTAT _this)
{
	const UINT8* p;

	p = read(_this, 1);
	if (p != NULL)
	{
		return *p;
	}
	return kStatTerminate;
}

/**
 * Reads to the buffer
 * @param[in] _this instance
 * @param[out] lpBuffer buffer
 * @param[in] cbBuffer length
 * @retval TRUE Succeeded
 * @retval FALSE Failed
 */
static BOOL readBuffer(RECSTAT _this, void* lpBuffer, UINT cbBuffer)
{
	const void *ptr;

	ptr = read(_this, cbBuffer);
	if (ptr != NULL)
	{
		memcpy(lpBuffer, ptr, cbBuffer);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 * Writes data
 * @param[in] _this instance
 * @param[in] lpBuffer buffer
 * @param[in] cbBuffer length
 */
static void writeBuffer(RECSTAT _this, const void* lpBuffer, UINT cbBuffer)
{
	if ((sizeof(_this->buffer) - _this->size) < cbBuffer)
	{
		flushBuffer(_this);
	}

	memcpy(_this->buffer + _this->size, lpBuffer, cbBuffer);
	_this->size += cbBuffer;
}

/**
 * Writes byte
 * @param[in] _this instance
 * @param[in] data data
 */
static void writeByte(RECSTAT _this, REG8 data)
{
	writeBuffer(_this, &data, 1);
}

/**
 * Hooks a behavior of the keyboard
 * @param[in] data The serialized data of the keyboard
 * @retval TRUE Hooked
 * @retval FALSE Unhooked
 */
BOOL recstat_keyboard(REG8 data)
{
	RECSTAT _this = &s_recstat;

	if (_this->mode == kRecStatRecording)
	{
		writeByte(_this, kStatKeyboard);
		writeByte(_this, data);
	}
	else if (_this->mode == kRecStatPlaying)
	{
		return TRUE;
	}
	return FALSE;
}

/**
 * Hooks a behavior of the mouse
 * @param[in] sx The x-movement
 * @param[in] sy The y-movement
 * @param[in] btn The status of the buttons
 * @retval TRUE Hooked
 * @retval FALSE Unhooked
 */
BOOL recstat_mouse(SINT16 sx, SINT16 sy, REG8 btn)
{
	RECSTAT _this = &s_recstat;

	if (_this->mode == kRecStatRecording)
	{
		if ((sx != 0) || (sy != 0) || (btn != 0xa0))
		{
			writeByte(_this, kStatMouse);
			writeByte(_this, btn);
			writeBuffer(_this, &sx, sizeof(sx));
			writeBuffer(_this, &sy, sizeof(sy));
		}
	}
	else if (_this->mode == kRecStatPlaying)
	{
		return TRUE;
	}
	return FALSE;
}

/**
 * Hooks a behavior of the FDD
 * @param[in] drv The number of the drive
 * @param[in] lpFilename The name of the disk image file
 * @param[in] ftype The type of the file
 * @param[in] readonly Readonly if non-zero
 * @retval TRUE Hooked
 * @retval FALSE Unhooked
 */
BOOL recstat_fdd(REG8 drv, const OEMCHAR *lpFilename, UINT ftype, int readonly)
{
	RECSTAT stat = &s_recstat;

	if (stat->mode == kRecStatRecording)
	{
		struct RecStatFdd fdd;
		fdd.drv = drv;
		fdd.readonly = readonly ? 1 : 0;
		fdd.ftype = ftype;
		fdd.cbFilename = 0;
		if (lpFilename)
		{
			fdd.cbFilename = (OEMSTRLEN(lpFilename) + 1) * sizeof(OEMCHAR);
		}

		writeByte(stat, kStatFdd);
		writeBuffer(stat, &fdd, sizeof(fdd));
		if (fdd.cbFilename)
		{
			writeBuffer(stat, lpFilename, fdd.cbFilename);
		}
	}
	else if (stat->mode == kRecStatPlaying)
	{
		return TRUE;
	}
	return FALSE;
}

/**
 * Hooks a behavior of the jpypad
 * @param[in] nPort The number of the joypad
 * @param[in,out] data The status of the joypad
 * @retval TRUE Hooked
 * @retval FALSE Unhooked
 */
BOOL recstat_joypad(int nPort, REG8 *data)
{
	RECSTAT _this = &s_recstat;

	if ((_this->mode == kRecStatRecording) || (_this->mode == kRecStatPlaying))
	{
		*data = _this->joypad[nPort];
		return TRUE;
	}
	return FALSE;
}

/**
 * Synchronizes
 * @retval TRUE Break
 * @retval FALSE Non-break
 */
BOOL recstat_sync(void)
{
	RECSTAT _this = &s_recstat;

	if (_this->mode == kRecStatRecording)
	{
		_this->mode = kRecStatNone;
		_this->joypad[0] = fmboard_getjoypad(0);
		_this->joypad[1] = fmboard_getjoypad(1);
		_this->mode = kRecStatRecording;

		if (_this->joypad[0] != 0xff)
		{
			writeByte(_this, kStatJoypad1);
			writeByte(_this, _this->joypad[0]);
		}
		if (_this->joypad[1] != 0xff)
		{
			writeByte(_this, kStatJoypad2);
			writeByte(_this, _this->joypad[1]);
		}

		writeByte(_this, kStatDelimiter);
		np2cfg.calendar = 0;
		np2cfg.MOTOR = 0;
	}
	else if (_this->mode == kRecStatPlaying)
	{
		_this->mode = kRecStatNone;
		_this->joypad[0] = 0xff;
		_this->joypad[1] = 0xff;
		while (1 /*EVER*/)
		{
			REG8 mode = readByte(_this);
			if (mode == kStatDelimiter)
			{
				_this->mode = kRecStatPlaying;
				break;
			}
			else if (mode == kStatKeyboard)
			{
				keyboard_send(readByte(_this));
			}
			else if (mode == kStatMouse)
			{
				const struct RecStatMouse* mouse = read(_this, sizeof(*mouse));
				if (mouse == NULL)
				{
					break;
				}
				mouseif_set(mouse->sx, mouse->sy, mouse->btn);
			}
			else if (mode == kStatFdd)
			{
				struct RecStatFdd fdd;
				const OEMCHAR* lpFilename;

				if (!readBuffer(_this, &fdd, sizeof(fdd)))
				{
					break;
				}
				lpFilename = NULL;
				if (fdd.cbFilename)
				{
					lpFilename = read(_this, fdd.cbFilename);
				}
				diskdrv_setfddex(fdd.drv, lpFilename, fdd.ftype, fdd.readonly);
			}
			else if (mode == kStatJoypad1)
			{
				_this->joypad[0] = readByte(_this);
			}
			else if (mode == kStatJoypad2)
			{
				_this->joypad[1] = readByte(_this);
			}
			else if (mode == kStatRerecording)
			{
			}
			else
			{
				break;
			}
		}
	}
	return FALSE;
}

/**
 * Initalize
 */
void recstat_init(void)
{
	RECSTAT _this = &s_recstat;

	_this->mode = kRecStatNone;
	_this->hFile = FILEH_INVALID;
	_this->ptr = 0;
	_this->size = 0;
}

/**
 * Starts recording states
 * @param[in] lpFilename The name of the recording file
 */
void recstat_record(const OEMCHAR *lpFilename)
{
	RECSTAT _this = &s_recstat;

	recstat_close();

	_this->hFile = file_create(lpFilename);
	if (_this->hFile != FILEH_INVALID)
	{
		/* ejects disk */
		diskdrv_setfddex(0, NULL, 0, 0);
		diskdrv_setfddex(1, NULL, 0, 0);
		diskdrv_setfddex(2, NULL, 0, 0);
		diskdrv_setfddex(3, NULL, 0, 0);

		/* cpu reset */
		np2cfg.calendar = 0;
		np2cfg.MOTOR = 0;
		pccore_reset();

		/* writes initalize data */
		_this->mode = kRecStatRecording;
		writeByte(_this, kStatNp2cfg);
		writeBuffer(_this, &np2cfg, sizeof(np2cfg));

		writeByte(_this, kStatCalendar);
		writeBuffer(_this, &cal.dt, sizeof(cal.dt));
	}
}

/**
 * Continues recording states
 * @param[in] lpFilename The name of the recording file
 */
void recstat_continue(const OEMCHAR *lpFilename)
{
	RECSTAT _this = &s_recstat;

	recstat_close();

	_this->hFile = file_open(lpFilename);
	if (_this->hFile != FILEH_INVALID)
	{
		file_seek(_this->hFile, 0, FSEEK_END);
		_this->mode = kRecStatRecording;
		writeByte(_this, kStatRerecording);
	}
}

/**
 * Plays recorded states
 * @param[in] lpFilename The name of the playing file
 */
void recstat_play(const OEMCHAR *lpFilename)
{
	RECSTAT _this = &s_recstat;

	recstat_close();

	do
	{
		_this->hFile = file_open(lpFilename);
		if (_this->hFile == FILEH_INVALID)
		{
			break;
		}

		/* eject disk */
		diskdrv_setfddex(0, NULL, 0, 0);
		diskdrv_setfddex(1, NULL, 0, 0);
		diskdrv_setfddex(2, NULL, 0, 0);
		diskdrv_setfddex(3, NULL, 0, 0);

		if ((readByte(_this) != kStatNp2cfg) || (!readBuffer(_this, &np2cfg, sizeof(np2cfg))))
		{
			break;
		}

		/* cpu reset */
		np2cfg.calendar = 0;
		np2cfg.MOTOR = 0;
		pccore_reset();

		if ((readByte(_this) != kStatCalendar) || (!readBuffer(_this, &cal.dt, sizeof(cal.dt))))
		{
			break;
		}

		/* writes initalize data */
		_this->mode = kRecStatPlaying;
		return;

	} while (0 /*CONSTCOND*/);

	recstat_close();
}

/**
 * Closes
 */
void recstat_close(void)
{
	RECSTAT _this = &s_recstat;

	if (_this->hFile != FILEH_INVALID)
	{
		flushBuffer(_this);
		file_close(_this->hFile);

		recstat_init();
	}
}
