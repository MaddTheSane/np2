#include	"compiler.h"
#include	"i286.h"
#include	"pccore.h"


	_NEVENT		nevent;


void nevent_init(void) {

	// すべてをリセット
	ZeroMemory(&nevent, sizeof(nevent));
}

void nevent_get1stevent(void) {

	// 最短のイベントのクロック数をセット
	if (nevent.readyevents) {
		I286_BASECLOCK = nevent.item[nevent.level[0]].clock;
	}
	else {
		// イベントがない場合のクロック数をセット
		I286_BASECLOCK = NEVENT_MAXCLOCK;
	}

	// カウンタへセット
	I286_REMCLOCK = I286_BASECLOCK;
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

		// コールバックの実行
		if (item->proc != NULL) {
			item->proc(item);

			// 次回に持ち越しのイベントのチェック
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

	I286_CLOCK += I286_BASECLOCK;
	eventnum = 0;
	nextbase = NEVENT_MAXCLOCK;
	for (i=0; i<nevent.readyevents; i++) {
		curid = nevent.level[i];
		item = &nevent.item[curid];
		item->clock -= I286_BASECLOCK;
		if (item->clock > 0) {
			// イベント待ち中
			nevent.level[eventnum++] = curid;
			if (nextbase >= item->clock) {
				nextbase = item->clock;
			}
		}
		else {
			// イベント発生
			if (!(item->flag & (NEVENT_SETEVENT | NEVENT_WAIT))) {
				nevent.waitevent[nevent.waitevents++] = curid;
			}
			item->flag |= NEVENT_SETEVENT;
			item->flag &= ~(NEVENT_ENABLE);
		}
	}
	nevent.readyevents = eventnum;
	I286_BASECLOCK = nextbase;
	I286_REMCLOCK += nextbase;
	nevent_execute();
//	TRACEOUT((buf, "nextbase = %d (%d)", nextbase, I286_REMCLOCK));
}


void nevent_reset(UINT id) {

	UINT	i;

	// 現在進行してるイベントを検索
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			break;
		}
	}
	// イベントは存在した？
	if (i < nevent.readyevents) {
		// 存在していたら削る
		nevent.readyevents--;
		for (; i<nevent.readyevents; i++) {
			nevent.level[i] = nevent.level[i+1];
		}
	}
}

void nevent_waitreset(UINT id) {

	UINT	i;

	// 現在進行してるイベントを検索
	for (i=0; i<nevent.waitevents; i++) {
		if (nevent.waitevent[i] == id) {
			break;
		}
	}
	// イベントは存在した？
	if (i < nevent.waitevents) {
		// 存在していたら削る
		nevent.waitevents--;
		for (; i<nevent.waitevents; i++) {
			nevent.waitevent[i] = nevent.waitevent[i+1];
		}
	}
}

void nevent_set(UINT id, SINT32 eventclock, NEVENTCB proc, BOOL absolute) {

	SINT32		clock;
	NEVENTITEM	item;
	UINT		eventid;
	UINT		i;

	clock = I286_BASECLOCK - I286_REMCLOCK;
	item = &nevent.item[id];
	item->proc = proc;
	item->flag = 0;
	if (absolute) {
		item->clock = eventclock + clock;
	}
	else {
		item->clock += eventclock;
	}
#if 0
	if (item->clock < clock) {
		item->clock = clock;
	}
#endif
	// イベントの削除
	nevent_reset(id);

	// イベントの挿入位置の検索
	for (eventid=0; eventid<nevent.readyevents; eventid++) {
		if (item->clock < nevent.item[nevent.level[eventid]].clock) {
			break;
		}
	}

	// イベントの挿入
	for (i=nevent.readyevents; i>eventid; i--) {
		nevent.level[i] = nevent.level[i-1];
	}
	nevent.level[eventid] = id;
	nevent.readyevents++;

	// もし最短イベントだったら...
	if (eventid == 0) {
		clock = I286_BASECLOCK - item->clock;
		I286_BASECLOCK -= clock;
		I286_REMCLOCK -= clock;
//		TRACEOUT(("reset nextbase -%d (%d)", clock, I286_REMCLOCK));
	}
}

void nevent_setbyms(UINT id, SINT32 ms, NEVENTCB proc, BOOL absolute) {

	nevent_set(id, (pc.realclock / 1000) * ms, proc, absolute);
}

BOOL nevent_iswork(UINT id) {

	UINT	i;

	// 現在進行してるイベントを検索
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			return(TRUE);
		}
	}
	return(FALSE);
}

SINT32 nevent_getremain(UINT id) {

	UINT	i;

	// 現在進行してるイベントを検索
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			return(nevent.item[id].clock - (I286_BASECLOCK - I286_REMCLOCK));
		}
	}
	return(-1);
}

void nevent_forceexit(void) {

	if (I286_REMCLOCK > 0) {
		I286_BASECLOCK -= I286_REMCLOCK;
		I286_REMCLOCK = 0;
	}
}

