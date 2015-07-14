#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"


	_NEVENT		nevent;


void nevent_allreset(void) {

	// ���ׂĂ����Z�b�g
	ZeroMemory(&nevent, sizeof(nevent));
}

void nevent_get1stevent(void) {

	// �ŒZ�̃C�x���g�̃N���b�N�����Z�b�g
	if (nevent.readyevents) {
		CPU_BASECLOCK = nevent.item[nevent.level[0]].clock;
	}
	else {
		// �C�x���g���Ȃ��ꍇ�̃N���b�N�����Z�b�g
		CPU_BASECLOCK = NEVENT_MAXCLOCK;
	}

	// �J�E���^�փZ�b�g
	CPU_REMCLOCK = CPU_BASECLOCK;
}

static void nevent_execute(void) {

	UINT		eventnum;
	UINT		i;
	UINT		curid;
	NEVENTITEM	item;

	eventnum = 0;
	for (i=0; i<nevent.waitevents; i++) {
		curid = nevent.waitevent[i];
		item = &nevent.item[curid];

		// �R�[���o�b�N�̎��s
		if (item->proc != NULL) {
			item->proc(item);

			// ����Ɏ����z���̃C�x���g�̃`�F�b�N
			if (item->flag & NEVENT_WAIT) {
				nevent.waitevent[eventnum++] = curid;
			}
		}
		else {
			item->flag &= ~(NEVENT_WAIT);
		}
		item->flag &= ~(NEVENT_SETEVENT);
	}
	nevent.waitevents = eventnum;
}

void nevent_progress(void) {

	UINT		eventnum;
	SINT32		nextbase;
	UINT		i;
	UINT		curid;
	NEVENTITEM	item;

	CPU_CLOCK += CPU_BASECLOCK;
	eventnum = 0;
	nextbase = NEVENT_MAXCLOCK;
	for (i=0; i<nevent.readyevents; i++) {
		curid = nevent.level[i];
		item = &nevent.item[curid];
		item->clock -= CPU_BASECLOCK;
		if (item->clock > 0) {
			// �C�x���g�҂���
			nevent.level[eventnum++] = curid;
			if (nextbase >= item->clock) {
				nextbase = item->clock;
			}
		}
		else {
			// �C�x���g����
			if (!(item->flag & (NEVENT_SETEVENT | NEVENT_WAIT))) {
				nevent.waitevent[nevent.waitevents++] = curid;
			}
			item->flag |= NEVENT_SETEVENT;
			item->flag &= ~(NEVENT_ENABLE);
//			TRACEOUT(("event = %x", curid));
		}
	}
	nevent.readyevents = eventnum;
	CPU_BASECLOCK = nextbase;
	CPU_REMCLOCK += nextbase;
	nevent_execute();
//	TRACEOUT(("nextbase = %d (%d)", nextbase, CPU_REMCLOCK));
}


void nevent_reset(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			break;
		}
	}
	// �C�x���g�͑��݂����H
	if (i < nevent.readyevents) {
		// ���݂��Ă�������
		nevent.readyevents--;
		for (; i<nevent.readyevents; i++) {
			nevent.level[i] = nevent.level[i+1];
		}
	}
}

void nevent_waitreset(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.waitevents; i++) {
		if (nevent.waitevent[i] == id) {
			break;
		}
	}
	// �C�x���g�͑��݂����H
	if (i < nevent.waitevents) {
		// ���݂��Ă�������
		nevent.waitevents--;
		for (; i<nevent.waitevents; i++) {
			nevent.waitevent[i] = nevent.waitevent[i+1];
		}
	}
}

void nevent_set(UINT id, SINT32 eventclock, NEVENTCB proc, NEVENTPOSITION absolute) {

	SINT32		clk;
	NEVENTITEM	item;
	UINT		eventid;
	UINT		i;

//	TRACEOUT(("event %d - %xclocks", id, eventclock));

	clk = CPU_BASECLOCK - CPU_REMCLOCK;
	item = &nevent.item[id];
	item->proc = proc;
	item->flag = 0;
	if (absolute) {
		item->clock = eventclock + clk;
	}
	else {
		item->clock += eventclock;
	}
#if 0
	if (item->clock < clk) {
		item->clock = clk;
	}
#endif
	// �C�x���g�̍폜
	nevent_reset(id);

	// �C�x���g�̑}���ʒu�̌���
	for (eventid=0; eventid<nevent.readyevents; eventid++) {
		if (item->clock < nevent.item[nevent.level[eventid]].clock) {
			break;
		}
	}

	// �C�x���g�̑}��
	for (i=nevent.readyevents; i>eventid; i--) {
		nevent.level[i] = nevent.level[i-1];
	}
	nevent.level[eventid] = id;
	nevent.readyevents++;

	// �����ŒZ�C�x���g��������...
	if (eventid == 0) {
		clk = CPU_BASECLOCK - item->clock;
		CPU_BASECLOCK -= clk;
		CPU_REMCLOCK -= clk;
//		TRACEOUT(("reset nextbase -%d (%d)", clock, CPU_REMCLOCK));
	}
}

void nevent_setbyms(UINT id, SINT32 ms, NEVENTCB proc, NEVENTPOSITION absolute) {

	nevent_set(id, (pccore.realclock / 1000) * ms, proc, absolute);
}

BOOL nevent_iswork(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			return(TRUE);
		}
	}
	return(FALSE);
}

SINT32 nevent_getremain(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			return(nevent.item[id].clock - (CPU_BASECLOCK - CPU_REMCLOCK));
		}
	}
	return(-1);
}

void nevent_forceexit(void) {

	if (CPU_REMCLOCK > 0) {
		CPU_BASECLOCK -= CPU_REMCLOCK;
		CPU_REMCLOCK = 0;
	}
}

