#include "array.h"
#include "array_access.h"
#include "array_make.h"
#include "compile_file.h"
#include "cons.h"
#include "control_execute.h"
#include "control_object.h"
#include "condition.h"
#include "execute_values.h"
#include "hashtable.h"
#include "hold.h"
#include "integer.h"
#include "load_time_value.h"
#include "parse_function.h"
#include "parse_object.h"
#include "parse_typedef.h"
#include "symbol.h"
#include "typedef.h"

/*
 *  hash-table
 */
static void make_load_form_symbol(Execute ptr, addr *ret)
{
	GetConst(SYSTEM_SPECIAL_MAKE_LOAD_FORM, ret);
}

_g void init_parse_make_load_form(Execute ptr)
{
	addr symbol, value;

	if (eval_compile_p(ptr)) {
		make_load_form_symbol(ptr, &symbol);
		hashtable_heap(&value);
		settest_hashtable(value, HASHTABLE_TEST_EQ);
		pushspecial_control(ptr, symbol, value);
	}
}

/* found=1, notfound=0 */
static int get_make_load_form_(Execute ptr, addr key, addr *ret)
{
	addr table;

	make_load_form_symbol(ptr, &table);
	Return(getspecialcheck_local_(ptr, table, &table));

	return find_hashtable_(table, key, ret);
}


/*
 *  make-load-form
 */
static int parse_make_load_form_object(Execute ptr, addr *ret, addr expr, addr init)
{
	addr eval;
	LocalHold hold;

	set_load_time_value_symbol(ptr, T);
	hold = LocalHold_local(ptr);
	Return(localhold_parse_self_(hold, ptr, expr));
	if (init != Nil) {
		Return(localhold_parse_self_(hold, ptr, init));
	}
	localhold_end(hold);

	/* eval */
	eval_parse_heap(&eval, EVAL_PARSE_LOAD_TIME_VALUE, 4);
	SetEvalParse(eval, 0, Nil);  /* nil */
	SetEvalParse(eval, 1, expr);
	SetEvalParse(eval, 2, Nil);  /* readonly */
	SetEvalParse(eval, 3, init);
	return Result(ret, eval);
}

static int parse_make_load_lambda_body_(addr pos, addr g, addr value, addr *ret);
static int parse_make_load_lambda_cons_(addr pos, addr g, addr value, addr *ret)
{
	addr car, cdr;

	GetCons(value, &car, &cdr);
	Return(parse_make_load_lambda_body_(pos, g, car, &car));
	Return(parse_make_load_lambda_body_(pos, g, cdr, &cdr));
	cons_heap(ret, car, cdr);

	return 0;
}

static int parse_make_load_lambda_vector_(addr pos, addr g, addr value, addr *ret)
{
	addr vector, x;
	size_t size, i;

	lenarray(value, &size);
	vector_type_heap(&vector, value, size);
	for (i = 0; i < size; i++) {
		getarray(value, i, &x);
		Return(parse_make_load_lambda_body_(pos, g, x, &x));
		setarray(vector, i, x);
	}

	return Result(ret, vector);
}

static int parse_make_load_lambda_array_heap_(addr *ret, addr value)
{
	addr array, x;
	struct array_struct *str;

	array_empty_heap(&array);
	str = ArrayInfoStruct(array);
	*str = *ArrayInfoStruct(value);
	str->displaced = 0;
	str->simple = str->adjustable == 0 && str->fillpointer == 0;
	str->offset = 0;

	GetArrayInfo(value, ARRAY_INDEX_TYPE, &x);
	SetArrayInfo(array, ARRAY_INDEX_TYPE, x);
	GetArrayInfo(value, ARRAY_INDEX_DIMENSION, &x);
	SetArrayInfo(array, ARRAY_INDEX_DIMENSION, x);
	Return(array_allocate_(NULL, array, str));

	return Result(ret, array);
}

static int parse_make_load_lambda_array_copy_(addr pos, addr g, addr array, addr value)
{
	addr x;
	size_t size, i;
	struct array_struct *str;

	str = ArrayInfoStruct(array);
	size = str->front;
	for (i = 0; i < size; i++) {
		Return(array_get_t_(value, i, &x));
		Return(parse_make_load_lambda_body_(pos, g, x, &x));
		Return(array_set_(array, i, x));
	}

	return 0;
}

static int parse_make_load_lambda_array_(addr pos, addr g, addr value, addr *ret)
{
	addr array;

	if (array_system_specialized_p(value))
		return Result(ret, value);

	Return(parse_make_load_lambda_array_heap_(&array, value));
	Return(parse_make_load_lambda_array_copy_(pos, g, array, value));

	return Result(ret, array);
}

static int parse_make_load_lambda_body_(addr pos, addr g, addr value, addr *ret)
{
	if (pos == value)
		return Result(ret, g);

	switch (GetType(value)) {
		case LISPTYPE_CONS:
			return parse_make_load_lambda_cons_(pos, g, value, ret);

		case LISPTYPE_VECTOR:
			return parse_make_load_lambda_vector_(pos, g, value, ret);

		case LISPTYPE_ARRAY:
			return parse_make_load_lambda_array_(pos, g, value, ret);

		default:
			return Result(ret, value);
	}
}

static int parse_make_load_lambda_(Execute ptr, addr pos, addr init, addr *ret)
{
	addr g, lambda;

	if (init == Nil)
		return Result(ret, Nil);

	/* (lambda (g) [replace pos -> g]) */
	Return(make_gensym_(ptr, &g));
	GetConst(COMMON_LAMBDA, &lambda);
	Return(parse_make_load_lambda_body_(pos, g, init, &init));
	list_heap(&g, g, NULL);
	list_heap(ret, lambda, g, init, NULL);

	return 0;
}

static int parse_make_load_form_generic_call_(
		Execute ptr, LocalHold hold, addr pos, addr *rexpr, addr *rinit)
{
	addr call, expr, init;

	GetConst(COMMON_MAKE_LOAD_FORM, &call);
	GetFunctionSymbol(call, &call);
	Return(funcall_control(ptr, call, pos, NULL));
	/* result */
	getresult_control(ptr, &expr);
	localhold_set(hold, 0, expr);
	getvalues_control(ptr, 1, &init);
	if (init == Unbound)
		init = Nil;
	localhold_set(hold, 1, init);

	*rexpr = expr;
	*rinit = init;
	return 0;
}

static int parse_make_load_form_generic(Execute ptr, addr pos, addr *ret1, addr *ret2)
{
	addr control, expr, init;
	LocalHold hold;

	hold = LocalHold_array(ptr, 2);
	/* (make-load-form clos) */
	push_control(ptr, &control);
	expr = init = Nil;
	(void)parse_make_load_form_generic_call_(ptr, hold, pos, &expr, &init);
	Return(pop_control_(ptr, control));
	localhold_end(hold);

	*ret1 = expr;
	return parse_make_load_lambda_(ptr, pos, init, ret2);
}

static int parse_make_load_form(Execute ptr, addr *ret, addr pos)
{
	addr expr, init;

	Return(parse_make_load_form_generic(ptr, pos, &expr, &init));
	Return(parse_make_load_form_object(ptr, ret, expr, init));

	return 0;
}

_g int parse_clos(Execute ptr, addr *ret, addr pos)
{
	addr value;

	if (! eval_compile_p(ptr)) {
		eval_single_parse_heap(ret, EVAL_PARSE_CLOS, pos);
		return 0;
	}

	Return(get_make_load_form_(ptr, pos, &value));
	if (value != Unbound)
		return Result(ret, value);
	else
		return parse_make_load_form(ptr, ret, pos);
}


/*
 *  init
 */
static void compile_make_load_form_symbol(Execute ptr, addr *ret)
{
	GetConst(SYSTEM_COMPILE_MAKE_LOAD_FORM, ret);
}

_g void init_write_make_load_form(Execute ptr)
{
	addr symbol, index, table, cons;

	compile_make_load_form_symbol(ptr, &symbol);
	/* (index . hash-table) */
	fixnum_heap(&index, 0);
	hashtable_heap(&table);
	settest_hashtable(table, HASHTABLE_TEST_EQL);
	cons_heap(&cons, index, table);
	pushspecial_control(ptr, symbol, cons);
}

_g void init_read_make_load_form(Execute ptr)
{
	addr symbol, table;

	compile_make_load_form_symbol(ptr, &symbol);
	/* hash-table */
	hashtable_heap(&table);
	settest_hashtable(table, HASHTABLE_TEST_EQL);
	pushspecial_control(ptr, symbol, table);
}

_g int get_write_make_load_form_(Execute ptr, addr key, addr *ret)
{
	int check;
	addr symbol, special, index, table, cons, value;

	/* table */
	compile_make_load_form_symbol(ptr, &symbol);
	Return(getspecialcheck_local_(ptr, symbol, &special));
	GetCons(special, &index, &table); 

	/* object */
	CheckType(key, LISPTYPE_LOAD_TIME_VALUE);
	make_index_integer_heap(&key, (size_t)key);
	Return(internp_hashheap_(table, key, &cons, &check));
	if (check) {
		GetCdr(cons, ret);
		return 0;
	}

	/* intern */
	SetCdr(cons, index);
	Return(oneplus_integer_common_(ptr->local, index, &value));
	SetCar(special, value);

	return Result(ret, index);
}

_g int get_read_make_load_form_(Execute ptr, addr key, addr *ret)
{
	int check;
	addr table, cons, value;

	/* table */
	compile_make_load_form_symbol(ptr, &table);
	Return(getspecialcheck_local_(ptr, table, &table));

	/* intern */
	Check(! integerp(key), "type error");
	Return(internp_hashheap_(table, key, &cons, &check));
	if (check) {
		GetCdr(cons, ret);
		return 0;
	}

	/* add value */
	load_time_value_heap(&value);
	SetCdr(cons, value);

	return Result(ret, value);
}

