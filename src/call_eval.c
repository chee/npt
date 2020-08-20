#include "callname.h"
#include "compile.h"
#include "condition.h"
#include "cons.h"
#include "cons_list.h"
#include "control_object.h"
#include "control_operator.h"
#include "declare.h"
#include "eval_execute.h"
#include "function.h"
#include "hold.h"
#include "lambda.h"
#include "parse.h"
#include "parse_macro.h"
#include "symbol.h"

/*
 *  lambda
 */
_g void lambda_common(addr form, addr *ret)
{
	addr symbol;
	GetConst(COMMON_FUNCTION, &symbol);
	list_heap(ret, symbol, form, NULL);
}


/*
 *  eval
 */
_g int eval_common(Execute ptr, addr var)
{
	return eval_execute_partial(ptr, var);
}


/*
 *  compiler-macro-function
 */
static int compiler_macro_function_symbol(addr var, addr env, addr *ret)
{
	if (env != Unbound) {
		Return(find_environment_(var, env, &env));
		if (env == Unbound) {
			/* compiler-macro-function is shadowed */
			return Result(ret, Nil);
		}
	}
	GetCallName(var, &var);
	get_compiler_macro_symbol(var, ret);
	return 0;
}

static int compiler_macro_function_setf(addr var, addr env, addr *ret)
{
	if (env != Unbound) {
		*ret = Nil;
		return fmte_("Don't use environment argument ~S "
				"in COMPILER-MACRO-FUNCTION setf-form.", env, NULL);
	}
	GetCallName(var, &var);
	get_setf_compiler_macro_symbol(var, ret);
	return 0;
}

_g int compiler_macro_function_common(addr var, addr env, addr *ret)
{
	Return(parse_callname_error_(&var, var));
	if (symbolp_callname(var))
		return compiler_macro_function_symbol(var, env, ret);
	else
		return compiler_macro_function_setf(var, env, ret);
}


/*
 *  (setf compiler-macro-function)
 */
static int setf_compiler_macro_function_symbol(addr var, addr env, addr value)
{
	if (env != Unbound) {
		Return(find_environment_(var, env, &env));
		if (env == Unbound) {
			/* compiler-macro-function is shadowed */
			return fmte_("COMPILER-MACRO-FUNCTION ~S "
					"is shadowed in the environment.", var, NULL);
		}
	}
	GetCallName(var, &var);
	return set_compiler_macro_symbol_(var, value);
}

static int setf_compiler_macro_function_setf(addr var, addr env, addr value)
{
	if (env != Unbound) {
		return fmte_("Don't use environment argument ~S "
				"in COMPILER-MACRO-FUNCTION setf-form.", env, NULL);
	}
	GetCallName(var, &var);
	return set_setf_compiler_macro_symbol_(var, value);
}

_g int setf_compiler_macro_function_common(addr value, addr var, addr env)
{
	if (! callnamep(var)) {
		Return(parse_callname_error_(&var, var));
	}
	if (symbolp_callname(var))
		return setf_compiler_macro_function_symbol(var, env, value);
	else
		return setf_compiler_macro_function_setf(var, env, value);
}


/*
 *  define-compiler-macro
 */
_g int define_compiler_macro_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr right, eval, name, args, decl, doc;

	/* (define-compiler-macro . form) */
	Return_getcdr(form, &right);
	if (right == Nil) {
		return fmte_("define-compiler-macro form "
				"must have at least a name and body.", NULL);
	}
	if (! consp(right))
		return fmte_("Invalid define-compiler-macro form.", NULL);

	/* name */
	Return_getcons(right, &name, &right);
	Return(parse_callname_error_(&name, name));
	if (right == Nil) {
		return fmte_("define-compiler-macro form "
				"must have at least a name and body.", NULL);
	}
	if (! consp(right))
		return fmte_("Invalid define-compiler-macro form.", NULL);

	/* args */
	Return_getcons(right, &args, &right);
	if (! IsList(right))
		return fmte_("Invalid define-compiler-macro form.", NULL);

	/* parse */
	Return(lambda_macro_(ptr->local, &args, args, Nil));
	Return(declare_body_documentation_(ptr, env, right, &doc, &decl, &right));

	/* (eval::define-compiler-macro name args decl doc body) */
	GetConst(SYSTEM_DEFINE_COMPILER_MACRO, &eval);
	list_heap(ret, eval, name, args, decl, doc, right, NULL);

	return 0;
}

_g int set_define_compiler_macro(addr callname, addr value)
{
	return setf_compiler_macro_function_common(value, callname, Unbound);
}


/*
 *  compile
 */
static int compile_variable(Execute ptr, addr var, addr opt, addr *ret)
{
	addr call, check;

	Return(parse_callname_error_(&call, var));
	getglobal_callname(call, &check);
	if (check == Unbound) {
		if (! symbolp_callname(call))
			goto unbound;
		getmacro_symbol(var, &check);
		if (check == Unbound)
			goto unbound;
	}
	Return(fmtw_("This implementation cannot compile a function.", NULL));
	return Result(ret, var);

unbound:
	*ret = Nil;
	return fmte_("The function ~S is unbound.", var, NULL);
}

static int compile_lambda_p(addr opt)
{
	addr check;

	if (! consp(opt))
		return 0;
	GetCar(opt, &opt);
	GetConst(COMMON_LAMBDA, &check);
	return check == opt;
}

static int compile_lambda(Execute ptr, addr opt, addr *ret)
{
	if (functionp(opt)) {
		Return(fmtw_("This implementation cannot compile a function.", NULL));
		return Result(ret, opt);
	}
	if (compile_lambda_p(opt)) {
		Return(fmtw_("This implementation cannot compile a function.", NULL));
		Return(eval_object(ptr, opt, &opt));
		return Result(ret, opt);
	}

	*ret = Nil;
	return fmte_("The second argument ~S must be a lambda expression.", opt, NULL);
}

static int compile_symbol(Execute ptr, addr var, addr opt, addr *ret)
{
	addr call;
	LocalHold hold;

	Return(parse_callname_error_(&call, var));
	if (functionp(opt)) {
		Return(fmtw_("This implementation cannot compile a function.", NULL));
		Return(setglobal_callname_(call, opt));
		return Result(ret, var);
	}
	if (compile_lambda_p(opt)) {
		Return(fmtw_("This implementation cannot compile a function.", NULL));
		hold = LocalHold_local(ptr);
		localhold_pushva_force(hold, call, opt, NULL);
		Return(eval_object(ptr, opt, &opt));
		localhold_end(hold);
		Return(setglobal_callname_(call, opt));
		return Result(ret, var);
	}

	*ret = Nil;
	return fmte_("The second argument ~S must be a lambda expression.", opt, NULL);
}

static int compile_execute(Execute ptr, addr var, addr opt, addr *ret)
{
	if (opt == Unbound)
		return compile_variable(ptr, var, opt, ret);
	if (var == Nil)
		return compile_lambda(ptr, opt, ret);
	if (function_name_p(var))
		return compile_symbol(ptr, var, opt, ret);

	/* error */
	*ret = Nil;
	return fmte_("The first argument ~S "
			"in COMPILE must be a function-name.", var, NULL);
}

static int compile_common_call_(Execute ptr, LocalHold hold,
		addr var, addr opt, addr *ret1, addr *ret2, addr *ret3)
{
	Return(handler_compile_(ptr));
	Return(compile_execute(ptr, var, opt, ret1));
	localhold_set(hold, 0, *ret1);
	/* warning */
	GetConst(SYSTEM_COMPILE_WARNING, &var);
	Return(getspecialcheck_local_(ptr, var, ret2));
	/* style-warning */
	GetConst(SYSTEM_COMPILE_STYLE_WARNING, &var);
	Return(getspecialcheck_local_(ptr, var, ret3));

	return 0;
}

_g int compile_common(Execute ptr, addr var, addr opt,
		addr *ret1, addr *ret2, addr *ret3)
{
	addr control;
	LocalHold hold;

	hold = LocalHold_array(ptr, 1);
	push_control(ptr, &control);
	(void)compile_common_call_(ptr, hold, var, opt, ret1, ret2, ret3);
	Return(pop_control_(ptr, control));
	localhold_end(hold);

	return 0;
}


/*
 *  defmacro
 */
_g int defmacro_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr eval, name, args, decl, doc;

	/* (defmacro . form) */
	Return_getcdr(form, &form);
	if (form == Nil)
		return fmte_("defmacro form must have at least a name and body.", NULL);
	if (GetType(form) != LISPTYPE_CONS)
		return fmte_("Invalid defmacro form.", NULL);

	/* name */
	Return_getcons(form, &name, &form);
	if (! symbolp(name))
		return fmte_("defmacro name ~S must be a symbol.", name, NULL);
	if (form == Nil)
		return fmte_("defmacro form must have at least a name and body.", NULL);
	if (! consp(form))
		return fmte_("Invalid defmacro form.", NULL);

	/* args */
	Return_getcons(form, &args, &form);
	if (! IsList(form))
		return fmte_("Invalid defmacro form.", NULL);

	/* parse */
	Return(check_function_variable_(name));
	Return(lambda_macro_(ptr->local, &args, args, Nil));
	Return(declare_body_documentation_(ptr, env, form, &doc, &decl, &form));

	/* (eval::defmacro name args decl doc body) */
	GetConst(SYSTEM_DEFMACRO, &eval);
	list_heap(ret, eval, name, args, decl, doc, form, NULL);

	return 0;
}


/*
 *  macro-function
 */
_g int macro_function_common_(addr symbol, addr env, addr *ret)
{
	if (env == Unbound)
		env = Nil;
	Return(find_environment_(symbol, env, &env));
	return Result(ret, env != Unbound? env: Nil);
}


/*
 *  macroexpand
 */
_g int macroexpand_common(Execute ptr, addr form, addr env, addr *ret, addr *sec)
{
	int check;

	if (env == Unbound)
		env = Nil;
	Return(macroexpand(ptr, &form, form, env, &check));
	if (check) {
		*ret = form;
		*sec = T;
	}
	else {
		*ret = Nil;
		*sec = Nil;
	}

	return 0;
}


/*
 *  macroexpand_1
 */
_g int macroexpand_1_common(Execute ptr, addr form, addr env, addr *ret, addr *sec)
{
	int check;

	if (env == Unbound)
		env = Nil;
	Return(macroexpand1(ptr, &form, form, env, &check));
	if (check) {
		*ret = form;
		*sec = T;
	}
	else {
		*ret = Nil;
		*sec = Nil;
	}

	return 0;
}


/*
 *  define-symbol-macro
 */
_g int define_symbol_macro_common(addr form, addr env, addr *ret)
{
	addr cons, symbol, expansion;

	Return_getcdr(form, &cons);
	if (! consp_getcons(cons, &symbol, &cons))
		goto error;
	if (! consp_getcons(cons, &expansion, &cons))
		goto error;
	if (cons != Nil)
		goto error;
	if (! symbolp(symbol))
		return fmte_("The argument ~S must be a symbol.", NULL);
	/* (lisp-system::define-symbol-macro symbol expansion) */
	GetConst(SYSTEM_DEFINE_SYMBOL_MACRO, &form);
	list_heap(ret, form, symbol, expansion, NULL);
	return 0;

error:
	*ret = Nil;
	return fmte_("define-symbol-macro argument ~S "
			"must be a (symbol expansion) form.", form, NULL);
}


/*
 *  declaim
 */
_g int declaim_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr symbol;

	GetConst(SYSTEM_DECLAIM, &symbol);
	Return_getcdr(form, &form); /* (declaim . form) */
	Return(parse_declaim_heap_(ptr, Nil, form, &form));
	conscar_heap(&form, form);
	cons_heap(ret, symbol, form);

	return 0;
}


/*
 *  constantp
 */
static int eval_constantp_stable(addr var)
{
	addr check;

	switch (GetType(var)) {
		case LISPTYPE_CONS:
			GetCar(var, &var);
			GetConst(COMMON_QUOTE, &check);
			return check == var;

		case LISPTYPE_SYMBOL:
			if (keywordp(var))
				return 1;
			return GetStatusReadOnly(var);

		default:
			return 1;
	}
}

static int eval_constantp(Execute ptr, addr var, addr env, int *ret)
{
	int check;
	addr pos;

	Return(macroexpand(ptr, &pos, var, env, &check));
	if (check)
		var = pos;
	*ret = eval_constantp_stable(var);

	return 0;
}

_g int constantp_common(Execute ptr, addr var, addr opt, addr *ret)
{
	int check;

	if (opt == Unbound)
		opt = Nil;
	Return(eval_constantp(ptr, var, opt, &check));
	*ret = check? T: Nil;

	return 0;
}

