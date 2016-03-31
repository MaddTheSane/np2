/**
 * @file	nevent.c
 * @brief	Implementation of the event
 */

#include "compiler.h"
#include "nevent.h"
#include "cpucore.h"
#include "pccore.h"

	_NEVENT g_nevent;

void nevent_allreset(void)
{
	/* ���ׂĂ����Z�b�g */
	memset(&g_nevent, 0, sizeof(g_nevent));
}

void nevent_get1stevent(void)
{
	/* �ŒZ�̃C�x���g�̃N���b�N�����Z�b�g */
	if (g_nevent.readyevents)
	{
		CPU_BASECLOCK = g_nevent.item[g_nevent.level[0]].clock;
	}
	else
	{
		/* �C�x���g���Ȃ��ꍇ�̃N���b�N�����Z�b�g */
		CPU_BASECLOCK = NEVENT_MAXCLOCK;
	}

	/* �J�E���^�փZ�b�g */
	CPU_REMCLOCK = CPU_BASECLOCK;
}

static void nevent_execute(void)
{
	UINT nEvents;
	UINT i;
	NEVENTID id;
	NEVENTITEM item;

	nEvents = 0;
	for (i = 0; i < g_nevent.waitevents; i++)
	{
		id = g_nevent.waitevent[i];
		item = &g_nevent.item[id];

		/* �R�[���o�b�N�̎��s */
		if (item->proc != NULL)
		{
			item->proc(item);

			/* ����Ɏ����z���̃C�x���g�̃`�F�b�N */
			if (item->flag & NEVENT_WAIT)
			{
				g_nevent.waitevent[nEvents++] = id;
			}
		}
		else {
			item->flag &= ~(NEVENT_WAIT);
		}
		item->flag &= ~(NEVENT_SETEVENT);
	}
	g_nevent.waitevents = nEvents;
}

void nevent_progress(void)
{
	UINT nEvents;
	SINT32 nextbase;
	UINT i;
	NEVENTID id;
	NEVENTITEM item;

	CPU_CLOCK += CPU_BASECLOCK;
	nEvents = 0;
	nextbase = NEVENT_MAXCLOCK;
	for (i = 0; i < g_nevent.readyevents; i++)
	{
		id = g_nevent.level[i];
		item = &g_nevent.item[id];
		item->clock -= CPU_BASECLOCK;
		if (item->clock > 0)
		{
			/* �C�x���g�҂��� */
			g_nevent.level[nEvents++] = id;
			if (nextbase >= item->clock)
			{
				nextbase = item->clock;
			}
		}
		else
		{
			/* �C�x���g���� */
			if (!(item->flag & (NEVENT_SETEVENT | NEVENT_WAIT)))
			{
				g_nevent.waitevent[g_nevent.waitevents++] = id;
			}
			item->flag |= NEVENT_SETEVENT;
			item->flag &= ~(NEVENT_ENABLE);
//			TRACEOUT(("event = %x", id));
		}
	}
	g_nevent.readyevents = nEvents;
	CPU_BASECLOCK = nextbase;
	CPU_REMCLOCK += nextbase;
	nevent_execute();
//	TRACEOUT(("nextbase = %d (%d)", nextbase, CPU_REMCLOCK));
}


void nevent_reset(NEVENTID id)
{
	UINT i;

	/* ���ݐi�s���Ă�C�x���g������ */
	for (i = 0; i < g_nevent.readyevents; i++)
	{
		if (g_nevent.level[i] == id)
		{
			break;
		}
	}
	/* �C�x���g�͑��݂����H */
	if (i < g_nevent.readyevents)
	{
		/* ���݂��Ă������� */
		g_nevent.readyevents--;
		for (; i < g_nevent.readyevents; i++)
		{
			g_nevent.level[i] = g_nevent.level[i + 1];
		}
	}
}

void nevent_waitreset(NEVENTID id)
{
	UINT i;

	/* ���ݐi�s���Ă�C�x���g������ */
	for (i = 0; i < g_nevent.waitevents; i++)
	{
		if (g_nevent.waitevent[i] == id)
		{
			break;
		}
	}
	/* �C�x���g�͑��݂����H */
	if (i < g_nevent.waitevents)
	{
		/* ���݂��Ă������� */
		g_nevent.waitevents--;
		for (; i < g_nevent.waitevents; i++)
		{
			g_nevent.waitevent[i] = g_nevent.waitevent[i + 1];
		}
	}
}

void nevent_set(NEVENTID id, SINT32 eventclock, NEVENTCB proc, NEVENTPOSITION absolute)
{
	SINT32 clk;
	NEVENTITEM item;
	UINT eventId;
	UINT i;

//	TRACEOUT(("event %d - %xclocks", id, eventclock));

	clk = CPU_BASECLOCK - CPU_REMCLOCK;
	item = &g_nevent.item[id];
	item->proc = proc;
	item->flag = 0;
	if (absolute)
	{
		item->clock = eventclock + clk;
	}
	else
	{
		item->clock += eventclock;
	}
#if 0
	if (item->clock < clk)
	{
		item->clock = clk;
	}
#endif
	/* �C�x���g�̍폜 */
	nevent_reset(id);

	/* �C�x���g�̑}���ʒu�̌��� */
	for (eventId = 0; eventId < g_nevent.readyevents; eventId++)
	{
		if (item->clock < g_nevent.item[g_nevent.level[eventId]].clock)
		{
			break;
		}
	}

	/* �C�x���g�̑}�� */
	for (i = g_nevent.readyevents; i > eventId; i--)
	{
		g_nevent.level[i] = g_nevent.level[i - 1];
	}
	g_nevent.level[eventId] = id;
	g_nevent.readyevents++;

	/* �����ŒZ�C�x���g��������... */
	if (eventId == 0)
	{
		clk = CPU_BASECLOCK - item->clock;
		CPU_BASECLOCK -= clk;
		CPU_REMCLOCK -= clk;
//		TRACEOUT(("reset nextbase -%d (%d)", clock, CPU_REMCLOCK));
	}
}

void nevent_setbyms(NEVENTID id, SINT32 ms, NEVENTCB proc, NEVENTPOSITION absolute)
{
	nevent_set(id, (pccore.realclock / 1000) * ms, proc, absolute);
}

BOOL nevent_iswork(NEVENTID id)
{
	UINT i;

	/* ���ݐi�s���Ă�C�x���g������ */
	for (i = 0; i < g_nevent.readyevents; i++)
	{
		if (g_nevent.level[i] == id)
		{
			return TRUE;
		}
	}
	return FALSE;
}

SINT32 nevent_getremain(NEVENTID id)
{
	UINT i;

	/* ���ݐi�s���Ă�C�x���g������ */
	for (i = 0; i < g_nevent.readyevents; i++)
	{
		if (g_nevent.level[i] == id)
		{
			return (g_nevent.item[id].clock - (CPU_BASECLOCK - CPU_REMCLOCK));
		}
	}
	return -1;
}

void nevent_forceexit(void)
{
	if (CPU_REMCLOCK > 0)
	{
		CPU_BASECLOCK -= CPU_REMCLOCK;
		CPU_REMCLOCK = 0;
	}
}
