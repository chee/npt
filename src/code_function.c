#include "call_eval.h"
#include "callname.h"
#include "code_lambda.h"
#include "condition.h"
#include "cons.h"
#include "cons_list.h"
#include "control_execute.h"
#include "control_object.h"
#include "control_operator.h"
#include "declare.h"
#include "document.h"
#include "eval_table.h"
#include "execute.h"
#include "execute_object.h"
#include "execute_values.h"
#include "function.h"
#include "integer.h"
#include "restart_value.h"
#include "symbol.h"
#include "type_deftype.h"

/*
 *  system
 */
_g int nop_code(Execute ptr, CodeValue x)
{
	return 0;
}

_g int execute_simple_set_code(Execute ptr, CodeValue x)
{
	return runcode_simple(ptr, x.pos);
}

_g int execute_normal_set_code(Execute ptr, CodeValue x)
{
	return runcode_normal(ptr, x.pos);
}

_g int execute_control_set_code(Execute ptr, CodeValue x)
{
	return runcode_control(ptr, x.pos);
}

_g int execute_switch_set_code(Execute ptr, CodeValue x)
{
	return runcode_switch(ptr, x.pos);
}

_g int execute_simple_push_code(Execute ptr, CodeValue x)
{
	Return(runcode_simple(ptr, x.pos));
	getresult_control(ptr, &x.pos);
	pushargs_control(ptr, x.pos);
	return 0;
}

_g int execute_normal_push_code(Execute ptr, CodeValue x)
{
	Return(runcode_normal(ptr, x.pos));
	getresult_control(ptr, &x.pos);
	pushargs_control(ptr, x.pos);
	return 0;
}

_g int execute_control_push_code(Execute ptr, CodeValue x)
{
	Return(runcode_control(ptr, x.pos));
	getresult_control(ptr, &x.pos);
	pushargs_control(ptr, x.pos);
	return 0;
}

_g int execute_switch_push_code(Execute ptr, CodeValue x)
{
	Return(runcode_switch(ptr, x.pos));
	getresult_control(ptr, &x.pos);
	pushargs_control(ptr, x.pos);
	return 0;
}


/*
 *  object
 */
_g int set_code(Execute ptr, CodeValue x)
{
	setresult_control(ptr, x.pos);
	return 0;
}

_g int push_code(Execute ptr, CodeValue x)
{
	pushargs_control(ptr, x.pos);
	return 0;
}

_g int push_result_code(Execute ptr, CodeValue x)
{
	getresult_control(ptr, &x.pos);
	pushargs_control(ptr, x.pos);
	return 0;
}

_g int push_values_code(Execute ptr, CodeValue x)
{
	pushargs_allvalues(ptr);
	return 0;
}

_g int nil_set_code(Execute ptr, CodeValue x)
{
	setresult_control(ptr, Nil);
	return 0;
}

_g int nil_push_code(Execute ptr, CodeValue x)
{
	pushargs_control(ptr, Nil);
	return 0;
}

_g int t_set_code(Execute ptr, CodeValue x)
{
	setresult_control(ptr, T);
	return 0;
}

_g int t_push_code(Execute ptr, CodeValue x)
{
	pushargs_control(ptr, T);
	return 0;
}


/*
 *  symbol
 */
_g int lexical_code(Execute ptr, CodeValue x)
{
	addr list, pos;
	size_t index;

	/* allocate */
	GetCons(x.pos, &pos, &list);
	GetIndex(pos, &index);
	lexical_control(ptr, index);

	/* closure */
	while (list != Nil) {
		GetCons(list, &pos, &list);
		CheckType(pos, LISPTYPE_INDEX);
		GetIndex(pos, &index);
		reference_lexical_control(ptr, index);
	}

	return 0;
}

_g int lexical_set_code(Execute ptr, CodeValue x)
{
	addr pos;

	get_lexical_control(ptr, x.index, &pos);
	setresult_control(ptr, pos);

	return 0;
}

_g int lexical_push_code(Execute ptr, CodeValue x)
{
	addr pos;

	get_lexical_control(ptr, x.index, &pos);
	pushargs_control(ptr, pos);

	return 0;
}

_g int lexical_rem_code(Execute ptr, CodeValue x)
{
#ifdef LISP_DEBUG
	get_lexical_control(ptr, x.index, &x.pos);
#endif
	return 0;
}

_g int special_set_code(Execute ptr, CodeValue x)
{
	Return(symbol_special_restart(ptr, x.pos, &x.pos));
	setresult_control(ptr, x.pos);
	return 0;
}

_g int special_push_code(Execute ptr, CodeValue x)
{
	Return(symbol_special_restart(ptr, x.pos, &x.pos));
	pushargs_control(ptr, x.pos);
	return 0;
}

_g int special_rem_code(Execute ptr, CodeValue x)
{
	return symbol_special_restart(ptr, x.pos, &x.pos);
}


/*
 *  declaim
 */
_g int declaim_special_code(Execute ptr, CodeValue x)
{
	setspecial_symbol(x.pos);
	return 0;
}

_g int declaim_type_value_code(Execute ptr, CodeValue x)
{
	addr symbol, type;

	List_bind(x.pos, &symbol, &type, NULL);
	settype_value_symbol(symbol, type);
	return 0;
}

_g int declaim_type_function_code(Execute ptr, CodeValue x)
{
	addr key, symbol, type;

	List_bind(x.pos, &key, &type, NULL);
	GetCallName(key, &symbol);
	if (symbolp_callname(key))
		settype_function_symbol(symbol, type);
	else
		settype_setf_symbol(symbol, type);

	return 0;
}

_g int declaim_inline_code(Execute ptr, CodeValue x)
{
	addr symbol;

	GetCallName(x.pos, &symbol);
	if (symbolp_callname(x.pos))
		setinline_function_symbol(symbol);
	else
		setinline_setf_symbol(symbol);

	return 0;
}

_g int declaim_notinline_code(Execute ptr, CodeValue x)
{
	addr symbol;

	GetCallName(x.pos, &symbol);
	if (symbolp_callname(x.pos))
		setnotinline_function_symbol(symbol);
	else
		setnotinline_setf_symbol(symbol);

	return 0;
}

_g int declaim_compilation_code(Execute ptr, CodeValue x)
{
	apply_compilation_speed_declaim((OptimizeType)x.value);
	return 0;
}

_g int declaim_debug_code(Execute ptr, CodeValue x)
{
	apply_debug_declaim((OptimizeType)x.value);
	return 0;
}

_g int declaim_safety_code(Execute ptr, CodeValue x)
{
	apply_safety_declaim((OptimizeType)x.value);
	return 0;
}

_g int declaim_space_code(Execute ptr, CodeValue x)
{
	apply_space_declaim((OptimizeType)x.value);
	return 0;
}

_g int declaim_speed_code(Execute ptr, CodeValue x)
{
	apply_speed_declaim((OptimizeType)x.value);
	return 0;
}

_g int declaim_declaration_code(Execute ptr, CodeValue x)
{
	push_declaration_declaim(x.pos);
	return 0;
}


/*
 *  let
 */
static int let_args_code(Execute ptr, addr x)
{
	addr list, pos, value, symbol, type;
	size_t index;

	/* value */
	list = x;
	while (list != Nil) {
		GetCons(list, &pos, &list);
		GetCons(list, &value, &list);
		Return(runcode_simple(ptr, value));
		getresult_control(ptr, &value);
		/* type check */
		if (getcheck_tablevalue(pos)) {
			gettype_tablevalue(pos, &type);
			Return(typep_error(ptr, value, type));
		}
		/* setlet */
		index = getlet_tablevalue(pos);
		set_lexical_control(ptr, index, value);
	}

	/* bind */
	list = x;
	while (list != Nil) {
		GetCons(list, &pos, &list);
		GetCdr(list, &list);
		/* getlet */
		index = getlet_tablevalue(pos);
		get_lexical_control(ptr, index, &value);
		/* bind */
		if (getspecialp_tablevalue(pos)) {
			getname_tablevalue(pos, &symbol);
			pushspecial_control(ptr, symbol, value);
		}
		else {
			index = getlexical_tablevalue(pos);
			set_lexical_control(ptr, index, value);
		}
	}

	return 0;
}

_g int let_set_code(Execute ptr, CodeValue x)
{
	addr control, args, body, free, allocate;

	List_bind(x.pos, &args, &body, &free, &allocate, NULL);
	if (allocate != Nil)
		push_new_control(ptr, &control);
	Return(let_free_code(ptr, free));
	Return(let_args_code(ptr, args));
	Return(runcode_simple(ptr, body));
	if (allocate != Nil)
		return free_control_(ptr, control);

	return 0;
}

_g int let_push_code(Execute ptr, CodeValue x)
{
	Return(let_set_code(ptr, x));
	getresult_control(ptr, &x.pos);
	pushargs_control(ptr, x.pos);

	return 0;
}

static int leta_args_code(Execute ptr, addr list)
{
	addr pos, value, symbol, type;

	while (list != Nil) {
		GetCons(list, &pos, &list);
		GetCons(list, &value, &list);
		getname_tablevalue(pos, &symbol);
		Return(runcode_simple(ptr, value));
		getresult_control(ptr, &value);
		/* type check */
		if (getcheck_tablevalue(pos)) {
			gettype_tablevalue(pos, &type);
			Return(typep_error(ptr, value, type));
		}
		/* bind */
		if (getspecialp_tablevalue(pos))
			pushspecial_control(ptr, symbol, value);
		else
			setvalue_tablevalue(ptr, pos, value);
	}

	return 0;
}

_g int leta_set_code(Execute ptr, CodeValue x)
{
	addr control, args, body, free, allocate;

	List_bind(x.pos, &args, &body, &free, &allocate, NULL);
	if (allocate != Nil)
		push_new_control(ptr, &control);
	Return(let_free_code(ptr, free));
	Return(leta_args_code(ptr, args));
	Return(runcode_simple(ptr, body));
	if (allocate != Nil)
		return free_control_(ptr, control);

	return 0;
}

_g int leta_push_code(Execute ptr, CodeValue x)
{
	Return(leta_set_code(ptr, x));
	getresult_control(ptr, &x.pos);
	pushargs_control(ptr, x.pos);

	return 0;
}


/*
 *  setq
 */
static int check_readonly_variable_(addr symbol)
{
	if (GetStatusReadOnly(symbol))
		return fmte_("Cannot set value to the constant variable ~S.", symbol, NULL);
	return 0;
}

_g int setq_set_code(Execute ptr, CodeValue x)
{
	addr list, pos, value, symbol, type;

	list = x.pos;
	while (list != Nil) {
		GetCons(list, &pos, &list);
		GetCons(list, &value, &list);
		/* readonly */
		getname_tablevalue(pos, &symbol);
		Return(check_readonly_variable_(symbol));
		/* value */
		Return(runcode_simple(ptr, value));
		getresult_control(ptr, &value);
		/* bind */
		if (getcheck_tablevalue(pos)) {
			gettype_tablevalue(pos, &type);
			Return(typep_error(ptr, value, type));
		}
		if (getspecialp_tablevalue(pos)) {
			setspecial_local(ptr, symbol, value);
		}
		else if (getglobalp_tablevalue(pos)) {
			SetValueSymbol(symbol, value);
		}
		else {
			setvalue_tablevalue(ptr, pos, value);
		}
	}

	return 0;
}

_g int setq_push_code(Execute ptr, CodeValue x)
{
	Return(setq_set_code(ptr, x));
	getresult_control(ptr, &x.pos);
	pushargs_control(ptr, x.pos);

	return 0;
}


/*
 *  function
 */
_g int function_set_code(Execute ptr, CodeValue x)
{
	Return(function_global_restart(ptr, x.pos, &x.pos));
	setresult_control(ptr, x.pos);
	return 0;
}

_g int function_push_code(Execute ptr, CodeValue x)
{
	Return(function_global_restart(ptr, x.pos, &x.pos));
	pushargs_control(ptr, x.pos);
	return 0;
}

_g int setf_set_code(Execute ptr, CodeValue x)
{
	Return(setf_global_restart(ptr, x.pos, &x.pos));
	setresult_control(ptr, x.pos);
	return 0;
}

_g int setf_push_code(Execute ptr, CodeValue x)
{
	Return(setf_global_restart(ptr, x.pos, &x.pos));
	pushargs_control(ptr, x.pos);
	return 0;
}


/*
 *  define
 */
_g int defmacro_code(Execute ptr, CodeValue x)
{
	addr symbol, value;

	List_bind(x.pos, &symbol, &value, NULL);
	setmacro_symbol(symbol, value);
	setresult_control(ptr, symbol);

	return 0;
}

_g int deftype_code(Execute ptr, CodeValue x)
{
	addr pos, symbol, doc;

	List_bind(x.pos, &symbol, &doc, NULL);
	getresult_control(ptr, &pos);
	setdocumentation_function(pos, doc);
	setdeftype(symbol, pos);
	setresult_control(ptr, symbol);

	return 0;
}

_g int define_compiler_macro_code(Execute ptr, CodeValue x)
{
	addr pos, name, doc;

	List_bind(x.pos, &name, &doc, NULL);
	getresult_control(ptr, &pos);
	setdocumentation_function(pos, doc);
	set_define_compiler_macro(name, pos);
	name_callname_heap(name, &name);
	setresult_control(ptr, name);

	return 0;
}

_g int define_symbol_macro_code(Execute ptr, CodeValue x)
{
	addr symbol, eval, form;

	List_bind(x.pos, &symbol, &eval, &form, NULL);
	Check(! symbolp(symbol), "type error");
	setsymbol_macro_symbol(symbol, eval, form);
	setresult_control(ptr, symbol);

	return 0;
}

_g int defun_code(Execute ptr, CodeValue x)
{
	addr pos, call, symbol;

	getresult_control(ptr, &pos);
	GetNameFunction(pos, &call);
	GetCallName(call, &symbol);

	if (symbolp_callname(call)) {
		SetFunctionSymbol(symbol, pos);
		setresult_control(ptr, symbol);
	}
	else {
		setsetf_symbol(symbol, pos);
		GetConst(COMMON_SETF, &pos);
		list_heap(&pos, pos, symbol, NULL);
		setresult_control(ptr, pos);
	}

	return 0;
}


/*
 *  call
 */
_g int call_code(Execute ptr, CodeValue x)
{
	return execute_control(ptr, x.pos);
}

_g int call_result_code(Execute ptr, CodeValue x)
{
	getresult_control(ptr, &x.pos);
	return execute_control(ptr, x.pos);
}

_g int call_type_code(Execute ptr, CodeValue x)
{
	addr value;
	getargs_tail_control(ptr, &value);
	return typep_error(ptr, value, x.pos);
}

_g int call_function_code(Execute ptr, CodeValue x)
{
	addr value;

	GetFunctionSymbol(x.pos, &value);
	if (value == Unbound) {
		Return(function_global_restart(ptr, x.pos, &value));
	}

	return execute_control(ptr, value);
}

_g int call_setf_code(Execute ptr, CodeValue x)
{
	addr value;

	getsetf_symbol(x.pos, &value);
	if (value == Unbound) {
		Return(setf_global_restart(ptr, x.pos, &value));
	}

	return execute_control(ptr, value);
}

_g int call_lexical_code(Execute ptr, CodeValue x)
{
	addr value;
	getlow_lexical_control(ptr, x.index, &value);
	return execute_control(ptr, value);
}


/*
 *  values
 */
_g int values_nil_code(Execute ptr, CodeValue x)
{
	setvalues_nil_control(ptr);
	return 0;
}

_g int values_set_code(Execute ptr, CodeValue x)
{
	addr pos;

	getargs_list_control_unsafe(ptr, 0, &pos);
	setvalues_list_control(ptr, pos);

	return 0;
}

_g int the_set_code(Execute ptr, CodeValue x)
{
	addr value;
	getresult_control(ptr, &value);
	return typep_error(ptr, value, x.pos);
}

_g int the_push_code(Execute ptr, CodeValue x)
{
	addr value;
	getargs_control(ptr, 0, &value);
	return typep_error(ptr, value, x.pos);
}


/*
 *  control
 */
_g int if_code(Execute ptr, CodeValue x)
{
	addr pos;

	getresult_control(ptr, &pos);
	if (pos == Nil) {
		GetCdr(x.pos, &x.pos);
	}
	GetCar(x.pos, &pos);
	return runcode_simple(ptr, pos);
}

_g int goto_code(Execute ptr, CodeValue x)
{
	return goto_control_(ptr, x.index);
}

_g int go_code(Execute ptr, CodeValue x)
{
	addr pos;
	get_lexical_control(ptr, x.index, &pos);
	return go_control_(ptr, pos);
}

_g int return_from_code(Execute ptr, CodeValue x)
{
	addr pos;
	get_lexical_control(ptr, x.index, &pos);
	return return_from_control_(ptr, pos);
}

_g int catch_code(Execute ptr, CodeValue x)
{
	getresult_control(ptr, &x.pos);
	catch_control(ptr, x.pos);
	return 0;
}

_g int throw_operator_code(Execute ptr, CodeValue x)
{
	getargs_control(ptr, 0, &x.pos);
	return throw_control_(ptr, x.pos);
}

_g int taginfo_code(Execute ptr, CodeValue x)
{
	set_taginfo_control(ptr, x.pos);
	return 0;
}

_g int blockinfo_code(Execute ptr, CodeValue x)
{
	set_blockinfo_control(ptr, x.pos);
	return 0;
}

_g int unwind_protect_code(Execute ptr, CodeValue x)
{
	set_protect_control(ptr, x.pos);
	return 0;
}


/*
 *  control-switch
 */
static void push_handler_code(Execute ptr, int escape)
{
	addr args, symbol, lambda;

	getargs_list_control_unsafe(ptr, 0, &args);
	while (args != Nil) {
		GetCons(args, &symbol, &args);
		GetCons(args, &lambda, &args);
		pushhandler_common(ptr, symbol, lambda, escape);
	}
	reverse_handler_control(ptr);
}

_g int handler_bind_code(Execute ptr, CodeValue x)
{
	push_handler_code(ptr, 0);
	return 0;
}

_g int handler_case_code(Execute ptr, CodeValue x)
{
	push_handler_code(ptr, 1);
	return 0;
}

static void push_restart_code(Execute ptr, int escape)
{
	addr args, list;

	getargs_list_control_unsafe(ptr, 0, &args);
	while (args != Nil) {
		GetCons(args, &list, &args);
		pushbind_restart_control(ptr, list, escape);
	}
	reverse_restart_control(ptr);
}

_g int restart_bind_code(Execute ptr, CodeValue x)
{
	push_restart_code(ptr, 0);
	return 0;
}

_g int restart_case_code(Execute ptr, CodeValue x)
{
	push_restart_code(ptr, 1);
	return 0;
}


/*
 *  eval
 */
_g int prog1_set_code(Execute ptr, CodeValue x)
{
	addr list, protect, cleanup, control, values;
	size_t size;

	GetCons(x.pos, &protect, &list);
	GetCar(list, &cleanup);
	push_new_control(ptr, &control);
	Return(runcode_simple(ptr, protect));
	save_values_control(ptr, &values, &size);
	Return(runcode_simple(ptr, cleanup));
	restore_values_control(ptr, values, size);

	return free_control_(ptr, control);
}

_g int prog1_push_code(Execute ptr, CodeValue x)
{
	Return(prog1_set_code(ptr, x));
	getresult_control(ptr, &x.pos);
	pushargs_control(ptr, x.pos);

	return 0;
}

_g int funcall_code(Execute ptr, CodeValue x)
{
	getargs_list_control_unsafe(ptr, 0, &x.pos);
	return call_control(ptr, x.pos);
}

_g int nth_value_code(Execute ptr, CodeValue x)
{
	addr pos;
	size_t index;

	getargs_control(ptr, 0, &pos);
	if (! integerp(pos))
		return fmte_("NTH-VALUE argument ~S must be integer type.", pos, NULL);
	if (! zerop_or_plusp_integer(pos))
		return fmte_("NTH-VALUE argument ~S must be greater than equal to 0.", pos, NULL);
	if (GetIndex_integer(pos, &index)) {
		setresult_control(ptr, Nil);
	}
	else {
		getvalues_control(ptr, index, &pos);
		setresult_control(ptr, (pos == Unbound)? Nil: pos);
	}

	return 0;
}

_g int progv_code(Execute ptr, CodeValue x)
{
	addr symbols, values, symbol, value;

	getargs_control(ptr, 0, &symbols);
	getargs_control(ptr, 1, &values);
	while (symbols != Nil) {
		if (! consp(symbols))
			return fmte_("PROGV form ~S must be a cons.", symbols, NULL);
		GetCons(symbols, &symbol, &symbols);
		if (! symbolp(symbol))
			return fmte_("PROGV argument ~S must be a symbol.", symbol, NULL);
		if (values == Nil) {
			pushspecial_control(ptr, symbol, Unbound);
		}
		else if (consp(values)) {
			GetCons(values, &value, &values);
			pushspecial_control(ptr, symbol, value);
		}
		else {
			return fmte_("PROGV form ~S must be a cons.", values, NULL);
		}
	}

	return 0;
}

