#include	"compiler.h"


LISTARRAY listarray_new(size_t listsize, UINT maxitems) {

	LISTARRAY	laRet = NULL;
	UINT		dwSize;

	dwSize = sizeof(_LISTARRAY);
	dwSize += listsize * maxitems;

#ifdef TRACE
	{
		char work[256];
		SPRINTF(work, "listarray %dx%d", listsize, maxitems);
		laRet = (LISTARRAY)_MALLOC(dwSize, work);
	}
#else
	laRet = (LISTARRAY)_MALLOC(dwSize, "listarray");
#endif
	if (laRet) {
		ZeroMemory(laRet, dwSize);
		laRet->maxitems = maxitems;
		laRet->listsize = listsize;
	}
	return(laRet);
}

void listarray_clr(LISTARRAY laHandle) {

	while(laHandle) {
		laHandle->items = 0;
		laHandle = laHandle->laNext;
	}
}

void listarray_destroy(LISTARRAY laHandle) {

	LISTARRAY	laNext;

	while(laHandle) {
		laNext = laHandle->laNext;
		_MFREE(laHandle);
		laHandle = laNext;
	}
}

UINT listarray_getitems(LISTARRAY laHandle) {

	UINT	dwRet;

	dwRet = 0;
	while(laHandle) {
		dwRet += laHandle->items;
		laHandle = laHandle->laNext;
	}
	return(dwRet);
}

void *listarray_append(LISTARRAY laHandle, const void *vpItem) {

	LISTARRAY	laNext;
	char		*p;

	if (laHandle == NULL) {
		goto laapp_err;
	}

	while(laHandle->items >= laHandle->maxitems) {
		laNext = laHandle->laNext;
		if (laNext == NULL) {
			laNext = listarray_new(laHandle->listsize, laHandle->maxitems);
			if (laNext == NULL) {
				goto laapp_err;
			}
			laHandle->laNext = laNext;
		}
		laHandle = laNext;
	}
	p = (char *)(laHandle + 1);
	p += laHandle->items * laHandle->listsize;
	if (vpItem) {
		CopyMemory(p, vpItem, laHandle->listsize);
	}
	else {
		ZeroMemory(p, laHandle->listsize);
	}
	laHandle->items++;
	return(p);

laapp_err:
	return(NULL);
}

void *listarray_getitem(LISTARRAY laHandle, UINT num) {

	while(laHandle) {
		if (num < laHandle->items) {
			return((char *)(laHandle + 1) + (laHandle->listsize * num));
		}
		num -= laHandle->items;
		laHandle = laHandle->laNext;
	}
	return(NULL);
}

void *listarray_enum(LISTARRAY laHandle,
				BOOL (*cbProc)(void *vpItem, void *vpArg), void *vpArg) {

	UINT	i;

	if (cbProc == NULL) {
		goto laenum_end;
	}

	while(laHandle) {
		char *p = (char *)(laHandle + 1);
		for (i=0; i<laHandle->items; i++) {
			if (cbProc((void *)p, vpArg)) {
				return((void *)p);
			}
			p += laHandle->listsize;
		}
		laHandle = laHandle->laNext;
	}

laenum_end:
	return(NULL);
}

