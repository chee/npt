#ifndef __MAKE_LOAD_FORM_HEADER__
#define __MAKE_LOAD_FORM_HEADER__

#include "execute.h"
#include "typedef.h"

#define init_parse_make_load_form _n(init_parse_make_load_form)
#define parse_clos _n(parse_clos)
#define init_write_make_load_form _n(init_write_make_load_form)
#define init_read_make_load_form _n(init_read_make_load_form)
#define get_write_make_load_form_ _n(get_write_make_load_form_)
#define get_read_make_load_form_ _n(get_read_make_load_form_)

void init_parse_make_load_form(Execute ptr);
int parse_clos(Execute ptr, addr *ret, addr pos);

/* compile-value */
void init_write_make_load_form(Execute ptr);
void init_read_make_load_form(Execute ptr);
int get_write_make_load_form_(Execute ptr, addr key, addr *ret);
int get_read_make_load_form_(Execute ptr, addr key, addr *ret);

#endif

