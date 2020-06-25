#include "callname.h"
#include "condition.h"
#include "control_object.h"
#include "eval_execute.h"
#include "gc.h"
#include "integer.h"
#include "parse.h"
#include "parse_function.h"
#include "load_time_value.h"
#include "make_load_form.h"
#include "parse_macro.h"
#include "symbol.h"

_g void check_variable(addr symbol)
{
	if (! symbolp(symbol))
		fmte("The variable ~S must be a symbol.", symbol, NULL);
	if (GetStatusReadOnly(symbol))
		fmte("The variable ~S don't allow constant symbol.", symbol, NULL);
}

_g void check_function_variable(addr symbol)
{
	addr check;

	if (symbolp(symbol)) {
		if (GetStatusReadOnly(symbol))
			fmte("The variable ~S don't allow constant symbol.", symbol, NULL);
	}
	else if (callnamep(symbol)) {
		GetCallName(symbol, &check);
		if (! symbolp(check))
			fmte("The variable ~S must be a symbol.", check, NULL);
		if (constantp_callname(symbol))
			fmte("The variable ~S don't allow constant symbol.", check, NULL);
	}
	else {
		fmte("The ~S don't allow variable.", symbol, NULL);
	}
}

_g int tagbody_tag_p(addr pos)
{
	/*
	 * Common Lisp the Language, 2nd Edition
	 * 7.8.5. The ``Program Feature''
	 * a symbol or an integer, in which case it is called a tag, ...
	 */
	return symbolp(pos) || integerp(pos);
}


/*
 *  eval-parse
 */
static void init_parse_eval_when(Execute ptr, addr toplevel)
{
	push_toplevel_eval(ptr, toplevel);
	push_compile_time_eval(ptr, Nil);
	push_compile_toplevel_eval(ptr, Nil);
	push_load_toplevel_eval(ptr, T);
	push_execute_eval(ptr, T);
}

_g int eval_parse(Execute ptr, addr *ret, addr pos, addr toplevel)
{
	addr control;
	LocalHold hold;

	hold = LocalHold_array(ptr, 1);
	push_new_control(ptr, &control);
	init_parse_environment(ptr);
	init_parse_load_time_value(ptr);
	init_parse_make_load_form(ptr);
	init_parse_eval_when(ptr, toplevel);
	Return(parse_execute(ptr, &pos, pos));
	localhold_set(hold, 0, pos);
	Return(eval_parse_load_time_value(ptr, &pos, pos));
	localhold_set(hold, 0, pos);
	Return(free_control_(ptr, control));
	localhold_end(hold);
	return Result(ret, pos);
}

