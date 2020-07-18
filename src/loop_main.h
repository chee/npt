#ifndef __LOOP_MAIN_HEADER__
#define __LOOP_MAIN_HEADER__

#include "define.h"
#include "typedef.h"

struct loop_main {
	addr form, init, named;
};

_g void make_loop_main(struct loop_main *ptr, addr form, addr init, addr named);
_g int loop_push_main_(struct loop_main *ptr, addr list);
_g int loop_let_main_(addr *form, addr list);

#endif

