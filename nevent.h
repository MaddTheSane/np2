
enum {
	NEVENT_MAXEVENTS	= 32,
	NEVENT_MAXCLOCK		= 0x3fffff,

	NEVENT_FLAMES		= 0,
	NEVENT_ITIMER		= 1,
	NEVENT_BEEP			= 2,
	NEVENT_RS232C		= 3,
	NEVENT_MUSICGEN		= 4,
	NEVENT_FMTIMERA		= 5,
	NEVENT_FMTIMERB		= 6,
	NEVENT_MOUSE		= 7,
	NEVENT_KEYBOARD		= 8,
	NEVENT_MIDIWAIT		= 9,
	NEVENT_MIDIINT		= 10,
	NEVENT_PICMASK		= 12,
	NEVENT_S98TIMER		= 13,
	NEVENT_CS4231		= 14,
	NEVENT_GDCSLAVE		= 17,
	NEVENT_FDBIOSBUSY	= 18,
	NEVENT_FDCBUSY		= 19,
	NEVENT_PC9861CH1	= 23,
	NEVENT_PC9861CH2	= 24,
	NEVENT_86PCM		= 25,

	NEVENT_ENABLE		= 0x0001,
	NEVENT_SETEVENT		= 0x0002,
	NEVENT_WAIT			= 0x0004,

	NEVENT_RELATIVE		= 0,
	NEVENT_ABSOLUTE		= 1
};

struct _neventitem;
typedef	struct _neventitem	_NEVENTITEM;
typedef	struct _neventitem	*NEVENTITEM;
typedef void (*NEVENTCB)(NEVENTITEM item);

struct _neventitem {
	SINT32		clock;
	UINT32		flag;
	NEVENTCB	proc;
	UINT32		padding;
};

typedef struct {
	SINT32		remainclock;
	SINT32		baseclock;
	UINT32		clock;
	UINT		readyevents;
	UINT		waitevents;
	UINT		level[NEVENT_MAXEVENTS];
	UINT		waitevent[NEVENT_MAXEVENTS];
	_NEVENTITEM	item[NEVENT_MAXEVENTS];
} _NEVENT, *NEVENT;


#ifdef __cplusplus
extern "C" {
#endif

extern	_NEVENT		nevent;

// ������
void nevent_init(void);

// �ŒZ�C�x���g�̃Z�b�g
void nevent_get1stevent(void);

// ���Ԃ�i�߂�
void nevent_progress(void);

// �C�x���g�̎��s
void nevent_execule(void);

// �C�x���g�̒ǉ�
void nevent_set(UINT id, SINT32 eventclock, NEVENTCB proc, BOOL absolute);
void nevent_setbyms(UINT id, SINT32 ms, NEVENTCB proc, BOOL absolute);

// �C�x���g�̍폜
void nevent_reset(UINT id);
void nevent_waitreset(UINT id);

// �C�x���g�̓����Ԏ擾
BOOL nevent_iswork(UINT id);

// �C�x���g���s�܂ł̃N���b�N���̎擾
SINT32 nevent_getremain(UINT id);

// NEVENT�̋����E�o
void nevent_forceexit(void);

#ifdef __cplusplus
}
#endif

