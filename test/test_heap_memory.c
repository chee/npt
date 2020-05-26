#include "heap_memory.c"
#include "heap.h"
#include "degrade.h"

#if 0
#define heap_debug_size  0x10000
static void gccheck_debug()
{
	GcCounter = 0;
	GcCheck1 = GcCheck2 = GcCheck3 = GcCheck4 = (heap_pos + heap_debug_size - 80);
}

static int test_length_space(void)
{
	byte mem[10000];
	size_t size;

	aatype(mem);
	heap_front = mem + 10000;
	SetType(mem, LISPTYPE_CONS);
	size = 0;
	test(length_space(mem, &size) == 0, "length_space1");

	aatype(mem);
	SetType(mem, LISPSYSTEM_SPACE1);
	SetSizeSpace1(mem, 12);
	SetType(mem + 12, LISPTYPE_CONS);
	test(length_space(mem, &size) != 0, "length_space2");
	test(size == 12, "length_space3");

	aatype(mem);
	SetType(mem, LISPSYSTEM_SPACE);
	SetSizeSpace(mem, 20);
	mem[20] = LISPTYPE_CONS;
	test(length_space(mem, &size) != 0, "length_space4");
	test(size == 20, "length_space5");

	aatype(mem);
	SetType(mem, LISPSYSTEM_SPACE1);
	SetSizeSpace1(mem, 80);
	SetType(mem + 80, LISPSYSTEM_SPACE);
	SetSizeSpace(mem + 80, 800);
	mem[80 + 800] = LISPTYPE_CONS;
	test(length_space(mem, &size) != 0, "length_space6");
	test(size == 80 + 800, "length_space7");

	RETURN;
}

static int test_check_spacememory(void)
{
	byte mem[10000];
	size_t size;

	/* space */
	aatype(mem);
	heap_front = mem + 10000;
	SetType(mem, LISPSYSTEM_SPACE1);
	SetSizeSpace1(mem, 80);
	size = 0;
	test(check_spacememory(mem, &size) != 0, "check_spacememory1");
	test(size == 80, "check_spacememory1a");

	/* object */
	aatype(mem);
	SetType(mem, LISPTYPE_CONS);
	SetStatus(mem, LISPSIZE_ARRAY4);
	*PtrValueL(mem) = 100;
	size = 0;
	test(check_spacememory(mem, &size) == 0, "check_spacememory2");
	test(size == 100, "check_spacememory3");

	/* reserved */
	aatype(mem);
	SetType(mem, LISPSYSTEM_RESERVED);
	SetSizeReserved(mem, 100);
	size = 0;
	test(check_spacememory(mem, &size) == 0, "check_spacememory4");
	test(size == 100, "check_spacememory5");

	RETURN;
}

static int test_searchmemory(void)
{
	byte mem[10000];
	addr pos;
	size_t size, value;

	/* space */
	aatype(mem);
	heap_front = mem + 10000;
	SetType(mem, LISPSYSTEM_SPACE1);
	SetSizeSpace1(mem, 80);
	size = 0;
	pos = searchmemory(mem, &size);
	test(pos == mem, "searchmemory1");
	test(size == 80, "searchmemory2");

	value = ConsLength;
	heap_front = mem + value;
	aatype(mem);
	SetType(mem, LISPTYPE_CONS);
	SetStatus(mem, LISPSIZE_ARRAY2);
	*PtrValue2L(mem) = ConsLength;
	pos = searchmemory(mem, &size);
	test(pos == NULL, "searchmemory3");

	heap_front = mem + 10000;
	SetType(mem + value, LISPSYSTEM_SPACE1);
	SetSizeSpace1(mem + value, 80);
	size = 0;
	pos = searchmemory(mem, &size);
	test(pos == mem + value, "searchmemory4");
	test(size == 80, "searchmemory5");

	RETURN;
}

static int test_makespace(void)
{
	byte mem[1000];
	size_t size, value;

	memset(mem, 0xBB, 1000);
	makespace(mem, 2);
	test(mem[0] == LISPSYSTEM_SPACE1, "makespace1");
	test(mem[1] == 0, "makespace2");
	test(mem[2] == 0xBB, "makespace3");

	memset(mem, 0xAA, 1000);
	size = 8UL + IdxSize - 1;
	makespace(mem, size);
	test(mem[0] == LISPSYSTEM_SPACE1, "makespace4");
	test(mem[1] == size - 2, "makespace5");

	memset(mem, 0xAA, 1000);
	size = 8UL + IdxSize;
	makespace(mem, size);
	test(mem[0] == LISPSYSTEM_SPACE, "makespace6");
	memcpy(&value, mem + 8, IdxSize);
	test(value == 0, "makespace7");

	memset(mem, 0xAA, 1000);
	size = 100;
	makespace(mem, size);
	test(mem[0] == LISPSYSTEM_SPACE, "makespace8");
	memcpy(&value, mem + 8, IdxSize);
	test(value == 100 - 8 - IdxSize, "makespace9");

	RETURN;
}

static int test_makereserved(void)
{
	byte mem[1000];
	size_t size;

	memset(mem, 0xAA, 1000);
	size = 8UL + IdxSize;
	makereserved(mem, size);
	test(mem[0] == LISPSYSTEM_RESERVED, "makereserved1");
	size = 999;
	memcpy(&size, mem + 8UL, IdxSize);
	test(size == 0, "makereserved2");

	memset(mem, 0xAA, 1000);
	makereserved(mem, 100);
	test(mem[0] == LISPSYSTEM_RESERVED, "makereserved3");
	size = 0;
	GetSizeReserved(mem, &size);
	test(size == 100, "makereserved4");

	RETURN;
}

static int test_writereserved(void)
{
	byte mem[heap_debug_size];
	size_t size;

	heap_front = mem + heap_debug_size;
	heap_pos = mem;
	gccheck_debug();

	aatype(mem);
	writereserved(mem, 80, 100);
	test(mem[0] == LISPSYSTEM_RESERVED, "writereserved1");
	size = 0;
	GetSizeReserved(mem, &size);
	test(size == 80, "writereserved2");
	test(mem[80] == LISPSYSTEM_SPACE, "writereserved3");
	size = 0;
	GetSizeSpace(mem + 80, &size);
	test(size == 100 - 80, "writereserved4");

	aatype(mem);
	writereserved(mem, 100, 100);
	test(mem[0] == LISPSYSTEM_RESERVED, "writereserved1");
	size = 0;
	GetSizeReserved(mem, &size);
	test(size == 100, "writereserved2");

	RETURN;
}

static int test_expandmemory(void)
{
	byte mem[heap_debug_size];

	aatype(mem);
	heap_pos = NULL;
	gccheck_debug();
	heap_front = mem;
	FrontMax = mem;
	heap_tail = mem + heap_debug_size;
	test(expandmemory(80) == mem, "expandmemory1");
	test(heap_pos == mem + 80, "expandmemory2");
	test(heap_front == mem + 80, "expandmemory3");
	test(FrontMax == mem + 80, "expandmemor4");

	heap_front = mem;
	heap_tail = mem + 80;
	test(expandmemory(80) == mem, "expandmemory5");

	heap_front = mem;
	heap_tail = mem + 79;
	lisp_info_enable = 0; /* infoerror */
	test(expandmemory(80) == NULL, "expandmemory6");
	lisp_info_enable = 1;

	RETURN;
}

static int test_allocfront(void)
{
	byte mem[heap_debug_size];
	addr pos;
	size_t size, spacesize;

	aatype(mem);
	heap_pos = heap_front = mem;
	gccheck_debug();
	heap_tail = mem + heap_debug_size;
	pos = allocfront_unlock(800);
	test(pos == mem, "allocfront1");
	test(heap_pos == mem + 800, "allocfront2");
	test(heap_front == mem + 800, "allocfront3");

	aatype(mem);
	heap_pos = mem;
	gccheck_debug();
	heap_front = mem + heap_debug_size;
	heap_tail = mem + heap_debug_size;
	SetType(mem, LISPTYPE_CONS);
	SetStatus(mem, LISPSIZE_ARRAY2);
	*PtrValue2L(mem) = ConsLength;
	size = ConsLength;
	SetType(mem + size, LISPSYSTEM_SPACE1);
	SetSizeSpace1(mem + size, 80);
	SetType(mem + size + 80, LISPTYPE_CONS);
	SetStatus(mem + size + 80, LISPSIZE_ARRAY2);
	pos = allocfront_unlock(48);
	test(pos == mem + size, "allocfront4");
	test(heap_pos == mem + size + 48, "allocfront5");
	test(mem[size + 48] == LISPSYSTEM_SPACE, "allocfront6");
	spacesize = 0;
	GetSizeSpace(mem + size + 48, &spacesize);
	test(spacesize == 80 - 48, "allocfront7");

	RETURN;
}

static int test_alloctail(void)
{
	byte mem[10000];
	addr pos, ret;

	aatype(mem);
	Align8Front(mem, &pos);
	heap_tail = pos + 8000;
	heap_front = pos;
	ret = alloctail_unlock(80);
	test(ret == heap_tail, "alloctail1");
	test(ret == pos + 8000 - 80, "alloctail2");

	heap_tail = pos + 8000;
	heap_front = pos;
	ret = alloctail_unlock(8000);
	test(ret == heap_tail, "alloctail3");
	test(ret == pos, "alloctail4");

	RETURN;
}

static int test_fillheapmemory(void)
{
	byte mem[heap_debug_size];
	addr pos, base;
	struct heapcell *cell;
	size_t size;

	aatype(mem);
	make_mutexlite(&Mutex);
	Align8Front(mem, &pos);
	heap_pos = heap_front = pos;
	gccheck_debug();
	heap_tail = pos + heap_debug_size - 8;
	cell = (struct heapcell *)alloctail(sizeof(struct heapcell));
	base = heap_pos;
	fillheapmemory(cell, 32);
	test(heap_pos == heap_front, "fillheapmemory1");
	test(heap_pos == pos + HeapCount * 32, "fillheapmemory2");

	test(cell->point[0] == base, "fillheapmemory3");
	test(base[0] == LISPSYSTEM_RESERVED, "fillheapmemory4");
	GetSizeReserved(base, &size);
	test(size == 32, "fillheapmemory5");

	base += 32;
	test(cell->point[1] == base, "fillheapmemory3");
	test(base[0] == LISPSYSTEM_RESERVED, "fillheapmemory4");
	GetSizeReserved(base, &size);
	test(size == 32, "fillheapmemory5");

	destroy_mutexlite(&Mutex);

	RETURN;
}

static int test_fillcellunbound(void)
{
	struct heapcell cell;

	aatype(cell);
	fillcellunbound(&cell);
	test(cell.point[0] == Unbound, "fillcellunbound1");
	test(cell.point[HeapCount - 1] == Unbound, "fillcellunbound2");

	RETURN;
}

static int test_cellalloc(void)
{
	byte mem[heap_debug_size];
	addr pos;
	struct heapcell *cell, *cell2;

	make_mutexlite(&Mutex);
	aatype(mem);
	Align8Front(mem, &pos);
	heap_pos = heap_front = pos;
	gccheck_debug();
	heap_tail = pos + heap_debug_size - 8;

	CellRoot = NULL;
	cell = cellalloc();
	test(cell == CellRoot, "cellalloc1");
	test(cell == CellPos, "cellalloc2");
	test(cell->next == NULL, "cellalloc3");
	test(cell->chain == NULL, "cellalloc4");
	test(cell->count == 0, "cellalloc5");

	cell2 = cellalloc();
	test(cell == cell2, "cellalloc6");
	cell->count = 1;
	cell2 = cellalloc();
	test(cell != cell2, "cellalloc7");
	test(cell->next == NULL, "cellalloc8");
	test(cell->chain == cell2, "cellalloc9");
	test(cell2 == CellPos, "cellalloc10");

	destroy_mutexlite(&Mutex);

	RETURN;
}

static int test_cellexpand(void)
{
	byte mem[heap_debug_size];
	addr pos;
	struct heapinfo root;
	struct heapcell cell;

	make_mutexlite(&Mutex);
	aatype(mem);
	Align8Front(mem, &pos);
	heap_pos = heap_front = pos;
	gccheck_debug();
	heap_tail = pos + heap_debug_size - 8;

	CellRoot = NULL;
	cleartype(root);
	cellexpand(&root, NULL);
	test(root.root, "cellexpand1");
	test(root.root == root.front, "cellexpand2");
	root.root->count = 1;

	cellexpand(&root, &cell);
	test(root.root, "cellexpand3");
	test(root.root != root.front, "cellexpand4");
	test(root.root->next == root.front, "cellexpand5");

	destroy_mutexlite(&Mutex);

	RETURN;
}

static int test_allocheap_small(void)
{
	byte mem[heap_debug_size];
	addr pos, temp1, temp2;
	struct heapinfo root[LISPCLASS_SIZE];
	size_t size;

	make_mutexlite(&Mutex);
	aatype(mem);
	Align8Front(mem, &pos);
	heap_pos = heap_front = pos;
	gccheck_debug();
	heap_tail = pos + heap_debug_size - 8;

	CellRoot = NULL;
	cleartype(root);
	heap_info = root;
	allocheap_small(32, 0, &temp1);
	test(heap_info[0].root->count == 1, "allocheap_small1");
	test(heap_info[0].root->point[0] == temp1, "allocheap_small2");
	test(temp1[0] == LISPSYSTEM_RESERVED, "allocheap_small3");
	size = 0;
	GetSizeReserved(temp1, &size);
	test(size == 32, "allocheap_small4");

	allocheap_small(32, 0, &temp2);
	test(heap_info[0].root->count == 2, "allocheap_small5");
	test(heap_info[0].root->point[1] == temp2, "allocheap_small6");
	test(temp1 != temp2, "allocheap_small7");
	test(temp2[0] == LISPSYSTEM_RESERVED, "allocheap_small8");
	size = 0;
	GetSizeReserved(temp2, &size);
	test(size == 32, "allocheap_small9");

	heap_info[0].root->count = HeapCount;
	allocheap_small(32, 0, &temp1);
	test(temp1 != temp2, "allocheap_small10");
	test(heap_info[0].front->count == 1, "allocheap_small11");
	test(heap_info[0].front->point[0] == temp1, "allocheap_small12");
	test(heap_info[0].root->next == heap_info[0].front, "allocheap_small13");
	test(temp1[0] == LISPSYSTEM_RESERVED, "allocheap_small14");
	size = 0;
	GetSizeReserved(temp1, &size);
	test(size == 32, "allocheap_small15");

	allocheap_small(32, 0, &temp1);
	test(heap_info[0].front->count == 2, "allocheap_small16");
	test(heap_info[0].front->point[1] == temp1, "allocheap_small17");
	test(temp1[0] == LISPSYSTEM_RESERVED, "allocheap_small18");
	size = 0;
	GetSizeReserved(temp1, &size);
	test(size == 32, "allocheap_small15");

	destroy_mutexlite(&Mutex);

	RETURN;
}

static int test_allocheap_large(void)
{
	byte mem[heap_debug_size];
	addr pos, temp1;
	struct heapinfo root[LISPCLASS_SIZE];

	make_mutexlite(&Mutex);
	aatype(mem);
	Align8Front(mem, &pos);
	heap_pos = heap_front = pos;
	gccheck_debug();
	heap_tail = pos + heap_debug_size - 8;

	CellRoot = NULL;
	cleartype(root);
	heap_info = root;
	allocheap_large(8192, 0, &temp1);
	test(heap_info[0].root->count == 1, "allocheap_large1");
	test(heap_info[0].root->point[0] == temp1, "allocheap_large2");

	RETURN;
}

static int test_allocheap(void)
{
	byte mem[heap_debug_size];
	addr pos, temp;
	struct heapinfo root[LISPCLASS_SIZE];

	make_mutexlite(&Mutex);
	aatype(mem);
	Align8Front(mem, &pos);
	heap_pos = heap_front = pos;
	gccheck_debug();
	heap_tail = pos + heap_debug_size - 8;

	CellRoot = NULL;
	cleartype(root);
	heap_info = root;

	/* size2 */
	allocheap(256, LISPTYPE_CONS, &temp, 1);
	test(temp[0] == LISPTYPE_CONS, "allocheap1");
	test(*PtrValue2L(temp) == 256, "allocheap2");

	allocheap(64, LISPTYPE_SYMBOL, &temp, 1);
	test(temp[0] == LISPTYPE_SYMBOL, "allocheap3");
	test(*PtrValue2L(temp) == 64, "allocheap4");

	/* size */
	allocheap(256, LISPTYPE_CONS, &temp, 0);
	test(temp[0] == LISPTYPE_CONS, "allocheap5");
	test(*PtrValueL(temp) == 256, "allocheap6");

	allocheap(64, LISPTYPE_SYMBOL, &temp, 0);
	test(temp[0] == LISPTYPE_SYMBOL, "allocheap7");
	test(*PtrValueL(temp) == 64, "allocheap8");

	destroy_mutexlite(&Mutex);

	RETURN;
}

static int test_make_mutexheap(void)
{
	int i, a;
	struct heapinfo root[LISPCLASS_SIZE];
	mutexlite *lock;

	memset(root, 0, sizeof(root));
	heap_info = root;
	test(make_mutexheap() == 0, "make_mutexheap1");

	a = 0;
	for (i = 0; i < LISPCLASS_Length; i++) {
		lock = heap_info[i].mutex;
		if (lock == NULL) {
			a = 1; break;
		}
	}
	test(a == 0, "make_mutexheap2");

	lock = heap_info[3].mutex;
	lock_mutexlite(lock);
	unlock_mutexlite(lock);
	free_mutexheap();

	RETURN;
}

static int test_alloc_heap(void)
{
	addr pos;

	lisp_info_enable = 0; /* infoerror */
	test(alloc_heap(10000) == 1, "alloc_heap1");
	lisp_info_enable = 1;
	free_heap();
	test(alloc_heap(1000*1000) == 0, "alloc_heap2");
	free_heap();

	/* error */
	CellRoot = CellPos = (struct heapcell *)Unbound;
	test(alloc_heap(1000*1000) == 0, "alloc_heap3");
	heap_cons(&pos);
	free_heap();

	RETURN;
}

static int call_foreach_heap_check[0xFF];
static void call_foreach_heap(struct heapinfo *heap)
{
	int i = (int)heap->type;
	call_foreach_heap_check[i] = i;
}

static int test_foreach_heap(void)
{
	int i, check;
	struct heapinfo array[LISPCLASS_Length];

	heap_info = array;
	memset(array, 0, sizeof(array));
	for (i = 0; i < LISPCLASS_Length; i++)
		array[i].type = (enum LISPCLASS)i;
	memset(call_foreach_heap_check, 0, sizeof(call_foreach_heap_check));
	foreach_heap(call_foreach_heap);
	check = 1;
	for (i = 0; i < LISPCLASS_Length; i++)
		check = check && (call_foreach_heap_check[i] == i);
	test(check, "foreach_heap1");

	RETURN;
}

static int test_cellupdate_heap(void)
{
	struct heapcell cell[10];

	CellRoot = &cell[0];
	cell[0].count = 0;
	CellPos = (struct heapcell *)Unbound;
	cellupdate_heap();
	test(CellPos == &cell[0], "cellupdate_heap1");

	cell[0].next = NULL;
	cell[0].chain = &cell[1];
	cell[0].count = 10;
	cell[1].next = NULL;
	cell[1].chain = &cell[2];
	cell[1].count = 20;
	cell[2].next = NULL;
	cell[2].chain = NULL;
	cell[2].count = 30;
	CellPos = (struct heapcell *)Unbound;
	cellupdate_heap();
	test(CellPos == &cell[2], "cellupdate_heap2");

	cell[0].next = NULL;
	cell[0].chain = &cell[1];
	cell[0].count = 10;
	cell[1].next = NULL;
	cell[1].chain = &cell[2];
	cell[1].count = 0;
	cell[2].next = NULL;
	cell[2].chain = NULL;
	cell[2].count = 30;
	CellPos = (struct heapcell *)Unbound;
	cellupdate_heap();
	test(CellPos == &cell[1], "cellupdate_heap3");
	CellRoot = CellPos = NULL;

	RETURN;
}
#endif


/*
 *  main
 */
int test_heap_memory(void)
{
	TITLE;

#if 0
	TestBreak(test_length_space);
	TestBreak(test_check_spacememory);
	TestBreak(test_searchmemory);
	TestBreak(test_makespace);
	TestBreak(test_makereserved);
	TestBreak(test_writereserved);
	TestBreak(test_expandmemory);
	TestBreak(test_allocfront);
	TestBreak(test_alloctail);
	TestBreak(test_fillheapmemory);
	TestBreak(test_fillcellunbound);
	TestBreak(test_cellalloc);
	TestBreak(test_cellexpand);
	TestBreak(test_allocheap_small);
	TestBreak(test_allocheap_large);
	TestBreak(test_allocheap);
	TestBreak(test_make_mutexheap);
	TestBreak(test_alloc_heap);
	TestBreak(test_foreach_heap);
	TestBreak(test_cellupdate_heap);
#endif

	return 0;
}

