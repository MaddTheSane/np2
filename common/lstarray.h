
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _la {
	UINT	maxitems;
	size_t	listsize;
	UINT	items;
struct _la	*laNext;
} _LISTARRAY, *LISTARRAY;

LISTARRAY listarray_new(size_t listsize, UINT maxitems);
void listarray_clr(LISTARRAY laHandle);
void listarray_destroy(LISTARRAY laHandle);

UINT listarray_getitems(LISTARRAY laHandle);
void *listarray_append(LISTARRAY laHandle, const void *vpItem);
void *listarray_getitem(LISTARRAY laHandle, UINT num);
void *listarray_enum(LISTARRAY laHandle,
				BOOL (*cbProc)(void *vpItem, void *vpArg), void *vpArg);

#ifdef __cplusplus
}
#endif

