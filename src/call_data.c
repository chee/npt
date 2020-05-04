#include "condition.h"
#include "cons.h"
#include "cons_list.h"
#include "constant.h"
#include "control_execute.h"
#include "declare.h"
#include "format.h"
#include "function.h"
#include "gc.h"
#include "lambda.h"
#include "local.h"
#include "parse.h"
#include "print.h"
#include "print_write.h"
#include "restart_value.h"
#include "sequence.h"
#include "setf.h"
#include "stream.h"
#include "stream_string.h"
#include "strtype.h"
#include "symbol.h"

/*
 *  apply
 */
_g int apply_common(Execute ptr, addr call, addr arg, addr args)
{
	lista_local_safe(ptr->local, &args, arg, args);
	return apply_control(ptr, call, args);
}


/*
 *  defun
 */
_g int defun_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr right, eval, name, args, decl, doc;
	LocalHold hold;

	/* (defun . right) */
	getcdr(form, &right);
	if (right == Nil)
		fmte("defun form must have at least a name and body.", NULL);
	if (GetType(right) != LISPTYPE_CONS)
		fmte("Invalid defun form.", NULL);

	/* name */
	hold = LocalHold_local(ptr);
	getcons(right, &name, &right);
	if (parse_callname_heap(&name, name))
		fmte("defun name ~S must be a symbol or (setf name) form.", name, NULL);
	localhold_push(hold, name);
	if (right == Nil)
		fmte("defun form must have at least a name and body.", NULL);
	if (GetType(right) != LISPTYPE_CONS)
		fmte("Invalid defun form.", NULL);

	/* args */
	getcons(right, &args, &right);
	if (! IsList(args))
		fmte("defun argument ~S don't allow dotted list.", args, NULL);
	if (! IsList(right))
		fmte("Invalid defun form.", NULL);

	/* parse */
	check_function_variable(name);
	lambda_ordinary(ptr->local, &args, args);
	localhold_push(hold, args);
	if (declare_body_documentation(ptr, env, right, &doc, &decl, &right))
		return 1;

	/* (eval::defun name args decl doc body) */
	GetConst(SYSTEM_DEFUN, &eval);
	list_heap(ret, eval, name, args, decl, doc, right, form, NULL);
	localhold_end(hold);
	return 0;
}


/*
 *  fdefinition
 */
_g int fdefinition_common(Execute ptr, addr name, addr *ret)
{
	parse_callname_error(&name, name);
	return callname_global_restart(ptr, name, ret);
}


/*
 *  (setf fdefinition)
 */
_g void setf_fdefinition_common(addr value, addr name)
{
	parse_callname_error(&name, name);
	remtype_funcion_callname_global(name);
	setfunction_callname_global(name, value);
}


/*
 *  fboundp
 */
_g int fboundp_common(addr name)
{
	parse_callname_error(&name, name);
	getfunction_callname_global(name, &name);
	return name != Unbound;
}


/*
 *  fmakunbound
 */
_g void fmakunbound_common(addr name)
{
	parse_callname_error(&name, name);
	remtype_funcion_callname_global(name);
	setfunction_callname_global(name, Unbound);
}


/*
 *  function-lambda-expression
 */
static int function_closure_p(addr var)
{
	addr check;

	GetClosureValueFunction(var, &check);
	if (check != Nil) return 1;
	GetClosureFunctionFunction(var, &check);
	if (check != Nil) return 1;
	GetClosureTagbodyFunction(var, &check);
	if (check != Nil) return 1;
	GetClosureBlockFunction(var, &check);
	if (check != Nil) return 1;

	return 0;
}

_g void function_lambda_expression_common(addr var, addr *ret1, addr *ret2, addr *ret3)
{
	addr pos1, pos2, pos3;

	/* lambda-expression */
	getlambda_expression_function(var, &pos1);
	/* closure-p */
	pos2 = function_closure_p(var)? T: Nil;
	/* name */
	GetNameFunction(var, &pos3);
	if (GetType(pos3) == LISPTYPE_CALLNAME)
		name_callname_heap(pos3, &pos3);
	/* result */
	*ret1 = pos1;
	*ret2 = pos2;
	*ret3 = pos3;
}


/*
 *  lambda-list-keywords
 */
_g void lambda_list_keywords_common(addr *ret)
{
	static const constindex lambda_list_keywords[] = {
		CONSTANT_AMPERSAND_WHOLE,
		CONSTANT_AMPERSAND_OPTIONAL,
		CONSTANT_AMPERSAND_REST,
		CONSTANT_AMPERSAND_BODY,
		CONSTANT_AMPERSAND_KEY,
		CONSTANT_AMPERSAND_ALLOW,
		CONSTANT_AMPERSAND_AUX,
		CONSTANT_AMPERSAND_ENVIRONMENT,
		CONSTANT_EMPTY
	};
	int i;
	addr list, pos;
	constindex index;

	list = Nil;
	for (i = 0; ; i++) {
		index = lambda_list_keywords[i];
		if (index == CONSTANT_EMPTY) break;
		GetConstant(index, &pos);
		cons_heap(&list, pos, list);
	}
	*ret = list;
}


/*
 *  defconstant
 */
_g void defconstant_common(addr form, addr env, addr *ret)
{
	/* (lisp-system::defconstant symbol value doc) */
	addr args, symbol, value, doc, quote;

	getcdr(form, &args);
	if (! consp(args))
		goto error;
	GetCons(args, &symbol, &args);
	if (! symbolp(symbol))
		fmte("The defconstant argument ~S must be a symbol.", symbol, NULL);
	if (! consp(args))
		goto error;
	GetCons(args, &value, &args);
	if (args == Nil) {
		doc = Nil;
	}
	else {
		if (! consp(args))
			goto error;
		GetCons(args, &doc, &args);
		if (! stringp(doc))
			fmte("The defconstant argument ~S must be a string.", doc, NULL);
		if (args != Nil)
			goto error;
	}
	GetConst(SYSTEM_DEFCONSTANT, &args);
	GetConst(COMMON_QUOTE, &quote);
	list_heap(&symbol, quote, symbol, NULL);
	list_heap(&doc, quote, doc, NULL);
	list_heap(ret, args, symbol, value, doc, NULL);
	return;

error:
	fmte("The defconstant argument ~S must be a "
			"(symbol value &optional documentation) form.", form, NULL);
}


/*
 *  defparameter
 */
static void expand_defparameter(addr symbol, addr value, addr doc, addr *ret)
{
	/* `(progn (declaim (special ,symbol))
	 *         (setf (symbol-value ',symbol) ,value)
	 *         ,(when doc
	 *            `(lisp-system::setdoc-variable ',symbol ',doc))
	 *         ',symbol)
	 */
	addr progn, declaim, special, setf, symbolv, setdoc, quote;

	GetConst(COMMON_PROGN, &progn);
	GetConst(COMMON_DECLAIM, &declaim);
	GetConst(COMMON_SPECIAL, &special);
	GetConst(COMMON_SETF, &setf);
	GetConst(COMMON_SYMBOL_VALUE, &symbolv);
	GetConst(COMMON_QUOTE, &quote);
	list_heap(&special, special, symbol, NULL);
	list_heap(&declaim, declaim, special, NULL);
	list_heap(&symbol, quote, symbol, NULL);
	list_heap(&symbolv, symbolv, symbol, NULL);
	list_heap(&setf, setf, symbolv, value, NULL);
	if (doc == Nil) {
		list_heap(&progn, progn, declaim, setf, symbol, NULL);
	}
	else {
		GetConst(SYSTEM_SETDOC_VARIABLE, &setdoc);
		list_heap(&doc, quote, doc, NULL);
		list_heap(&setdoc, setdoc, symbol, doc, NULL);
		list_heap(&progn, progn, declaim, setf, setdoc, symbol, NULL);
	}
	*ret = progn;
}

_g void defparameter_common(addr form, addr env, addr *ret)
{
	/* (lisp-system::defparameter symbol value doc) */
	addr args, symbol, value, doc;

	getcdr(form, &args);
	if (! consp(args)) goto error;
	GetCons(args, &symbol, &args);
	if (! symbolp(symbol))
		fmte("The defparameter argument ~S must be a symbol.", symbol, NULL);
	if (! consp(args)) goto error;
	GetCons(args, &value, &args);
	if (args == Nil) {
		doc = Nil;
		goto expand;
	}
	if (! consp(args)) goto error;
	GetCons(args, &doc, &args);
	if (! stringp(doc))
		fmte("The defparameter argument ~S must be a string.", doc, NULL);
	if (args != Nil) goto error;
expand:
	expand_defparameter(symbol, value, doc, ret);
	return;

error:
	fmte("The defparameter argument ~S must be a "
			"(symbol value &optional documentation) form.", form, NULL);
}


/*
 *  defvar
 */
static void expand_defvar_novalue(addr symbol, addr *ret)
{
	/* `(progn (declaim (special ,symbol))
	 *         ',symbol)
	 */
	addr progn, declaim, special, quote;

	GetConst(COMMON_PROGN, &progn);
	GetConst(COMMON_DECLAIM, &declaim);
	GetConst(COMMON_SPECIAL, &special);
	GetConst(COMMON_QUOTE, &quote);
	list_heap(&special, special, symbol, NULL);
	list_heap(&declaim, declaim, special, NULL);
	list_heap(&symbol, quote, symbol, NULL);
	list_heap(&progn, progn, declaim, symbol, NULL);
	*ret = progn;
}

static void expand_defvar(addr symbol, addr value, addr doc, addr *ret)
{
	/* `(progn (declaim (special ,symbol))
	 *         `(unless (boundp ',symbol)
	 *           (setf (symbol-value ',symbol) ,value))
	 *         ,(when doc
	 *           `(lisp-system::setdoc-variable ',symbol ',doc))
	 *         ',symbol)
	 */
	addr progn, declaim, special, unless, boundp, setf, symbolv, setdoc, quote;

	GetConst(COMMON_PROGN, &progn);
	GetConst(COMMON_DECLAIM, &declaim);
	GetConst(COMMON_SPECIAL, &special);
	GetConst(COMMON_UNLESS, &unless);
	GetConst(COMMON_BOUNDP, &boundp);
	GetConst(COMMON_SETF, &setf);
	GetConst(COMMON_SYMBOL_VALUE, &symbolv);
	GetConst(COMMON_QUOTE, &quote);
	list_heap(&special, special, symbol, NULL);
	list_heap(&declaim, declaim, special, NULL);
	list_heap(&symbol, quote, symbol, NULL);
	list_heap(&symbolv, symbolv, symbol, NULL);
	list_heap(&setf, setf, symbolv, value, NULL);
	list_heap(&boundp, boundp, symbol, NULL);
	list_heap(&unless, unless, boundp, setf, NULL);
	if (doc == Nil) {
		list_heap(&progn, progn, declaim, unless, symbol, NULL);
	}
	else {
		GetConst(SYSTEM_SETDOC_VARIABLE, &setdoc);
		list_heap(&doc, quote, doc, NULL);
		list_heap(&setdoc, setdoc, symbol, doc, NULL);
		list_heap(&progn, progn, declaim, unless, setdoc, symbol, NULL);
	}
	*ret = progn;
}

_g void defvar_common(addr form, addr env, addr *ret)
{
	addr args, symbol, value, doc;

	getcdr(form, &args);
	if (! consp(args)) goto error;
	GetCons(args, &symbol, &args);
	if (args == Nil) {
		value = Unbound;
		doc = Nil;
		goto expand;
	}
	if (! consp(args)) goto error;
	GetCons(args, &value, &args);
	if (args == Nil) {
		doc = Nil;
		goto expand;
	}
	if (! consp(args)) goto error;
	GetCons(args, &doc, &args);
	if (args != Nil) goto error;
expand:
	if (value == Unbound)
		expand_defvar_novalue(symbol, ret);
	else
		expand_defvar(symbol, value, doc, ret);
	return;

error:
	fmte("The defvar argument ~S must be a "
			"(symbol &optional value documentation) form.", form, NULL);
}


/*
 *  destructuring-bind
 */
static void check_destructuring_bind(addr pos)
{
	getenvironment_macro_lambda(pos, &pos);
	if (pos != Nil)
		fmte("destructuring-bind don't accept &environment parameter ~S.", pos, NULL);
}

_g int destructuring_bind_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr args, lambda, expr, decl, eval;
	LocalHold hold;

	getcdr(form, &args);
	if (! consp(args)) goto error;
	GetCons(args, &lambda, &args);
	if (! consp(args)) goto error;
	GetCons(args, &expr, &args);
	/* parse */
	if (! listp(lambda))
		fmte("destructuring-bind argument ~S must be a list type.", lambda, NULL);

	lambda_macro(ptr->local, &lambda, lambda, Nil);
	check_destructuring_bind(lambda);
	hold = LocalHold_local_push(ptr, lambda);
	if (declare_body(ptr, env, args, &decl, &args))
		return 1;
	localhold_end(hold);
	/* (eval::destructuring-bind lambda expr decl args) */
	GetConst(SYSTEM_DESTRUCTURING_BIND, &eval);
	list_heap(ret, eval, lambda, expr, decl, args, NULL);
	return 0;

error:
	fmte("destructuring-bind argument ~S must be a "
			"(lambda-list expr &body body) form.", form, NULL);
	return 0;
}


/*
 *  psetq
 */
static void psetq_common_constant(Execute ptr, addr form, addr env, addr *ret,
		constindex setq_constant,
		constindex psetq_constant)
{
	addr args, root, var, value, gensym, cons, setq, let;

	getcdr(form, &form);
	GetConstant(setq_constant, &setq);
	args = root = Nil;
	while (form != Nil) {
		if (! consp(form)) {
			GetConstant(psetq_constant, &setq);
			fmte("~A argument ~S don't allow dotted list.", setq, form, NULL);
		}
		GetCons(form, &var, &form);
		if (! consp(form))
			fmte("After variable ~S must be a cons, but ~S.", var, form, NULL);
		GetCons(form, &value, &form);
		make_gensym(ptr, &gensym);
		/* let argument */
		list_heap(&cons, gensym, value, NULL);
		cons_heap(&args, cons, args);
		/* body */
		list_heap(&cons, setq, var, gensym, NULL);
		cons_heap(&root, cons, root);
	}
	nreverse_list_unsafe(&args, args);
	nreverse_list_unsafe(&root, root);
	/* let form */
	GetConst(COMMON_LET, &let);
	lista_heap(ret, let, args, root, NULL);
}

_g void psetq_common(Execute ptr, addr form, addr env, addr *ret)
{
	psetq_common_constant(ptr, form, env, ret,
			CONSTANT_COMMON_SETQ,
			CONSTANT_COMMON_PSETQ);
}


/*
 *  psetf
 */
_g void psetf_common(Execute ptr, addr form, addr env, addr *ret)
{
	psetq_common_constant(ptr, form, env, ret,
			CONSTANT_COMMON_SETF,
			CONSTANT_COMMON_PSETF);
}


/*
 *  return
 */
_g void return_common(addr form, addr env, addr *ret)
{
	addr args, value, return_from;

	getcdr(form, &args);
	if (args == Nil) {
		value = Nil;
		goto expand;
	}
	if (! consp(args)) goto error;
	GetCons(args, &value, &args);
	if (args != Nil) goto error;
expand:
	GetConst(COMMON_RETURN_FROM, &return_from);
	list_heap(ret, return_from, Nil, value, NULL);
	return;

error:
	fmte("RETURN argument ~S must be a (&optional value) form.", form, NULL);
}


/*
 *  complement
 */
_g void complement_common(addr var, addr *ret)
{
	addr pos;

	compiled_heap(&pos, Nil);
	setcompiled_dynamic(pos, p_defun_lambda_complement);
	SetDataFunction(pos, var);
	*ret = pos;
}


/*
 *  constantly
 */
_g void constantly_common(addr var, addr *ret)
{
	addr pos;

	compiled_heap(&pos, Nil);
	setcompiled_any(pos, p_defun_lambda_constantly);
	SetDataFunction(pos, var);
	*ret = pos;
}


/*
 *  every
 */
_g int every_common(Execute ptr, addr call, addr rest, addr *ret)
{
	addr pos, car, cdr, args, next, temp1, temp2;
	LocalRoot local;
	LocalStack stack;
	size_t size;

	local = ptr->local;
	push_local(local, &stack);

	/* first */
	if (rest == Nil) goto result_true;
	args = next = Nil;
	size = 0;
	while (rest != Nil) {
		GetCons(rest, &pos, &rest);
		if (pos == Nil) goto result_true;
		if (consp(pos)) {
			getcons(pos, &car, &cdr);
			cons_local(local, &args, car, args);
			cons_local(local, &next, cdr, next);
		}
		else {
			if (length_sequence(pos, 1) <= size) goto result_true;
			getelt_sequence(NULL, pos, size, &car);
			cons_local(local, &args, car, args);
			cons_local(local, &next, pos, next);
		}
	}
	nreverse_list_unsafe(&args, args);
	nreverse_list_unsafe(&rest, next);
	if (callclang_apply(ptr, &pos, call, args))
		return 1;
	if (pos == Nil) goto result_false;

	/* second */
	for (size = 1; ; size++) {
		temp1 = args;
		temp2 = rest;
		while (temp1 != Nil) {
			GetCar(temp2, &cdr);
			if (cdr == Nil) goto result_true;
			if (consp(cdr)) {
				getcons(cdr, &car, &cdr);
				SetCar(temp1, car);
				SetCar(temp2, cdr);
			}
			else {
				if (length_sequence(cdr, 1) <= size) goto result_true;
				getelt_sequence(NULL, cdr, size, &car);
				SetCar(temp1, car);
			}
			GetCdr(temp1, &temp1);
			GetCdr(temp2, &temp2);
		}
		if (callclang_apply(ptr, &pos, call, args))
			return 1;
		if (pos == Nil) goto result_false;
	}

result_true:
	rollback_local(local, stack);
	*ret = T;
	return 0;

result_false:
	rollback_local(local, stack);
	*ret = Nil;
	return 0;
}


/*
 *  notevery
 */
_g int notevery_common(Execute ptr, addr call, addr rest, addr *ret)
{
	/* (notevery predicate sequence*) ==  (not (every predicate sequence*)) */
	if (every_common(ptr, call, rest, &rest)) return 1;
	*ret = (rest == Nil)? T: Nil;
	return 0;
}


/*
 *  some
 */
_g int some_common(Execute ptr, addr call, addr rest, addr *ret)
{
	addr pos, car, cdr, args, next, temp1, temp2;
	LocalRoot local;
	LocalStack stack;
	size_t size;

	local = ptr->local;
	push_local(local, &stack);

	/* first */
	if (rest == Nil) goto result_false;
	args = next = Nil;
	size = 0;
	while (rest != Nil) {
		GetCons(rest, &pos, &rest);
		if (pos == Nil) goto result_false;
		if (consp(pos)) {
			getcons(pos, &car, &cdr);
			cons_local(local, &args, car, args);
			cons_local(local, &next, cdr, next);
		}
		else {
			if (length_sequence(pos, 1) <= size) goto result_false;
			getelt_sequence(NULL, pos, size, &car);
			cons_local(local, &args, car, args);
			cons_local(local, &next, pos, next);
		}
	}
	nreverse_list_unsafe(&args, args);
	nreverse_list_unsafe(&rest, next);
	if (callclang_apply(ptr, &pos, call, args))
		return 1;
	if (pos != Nil) goto result;

	/* second */
	for (size = 1; ; size++) {
		temp1 = args;
		temp2 = rest;
		while (temp1 != Nil) {
			GetCar(temp2, &cdr);
			if (cdr == Nil) goto result_false;
			if (consp(cdr)) {
				getcons(cdr, &car, &cdr);
				SetCar(temp1, car);
				SetCar(temp2, cdr);
			}
			else {
				if (length_sequence(cdr, 1) <= size) goto result_false;
				getelt_sequence(NULL, cdr, size, &car);
				SetCar(temp1, car);
			}
			GetCdr(temp1, &temp1);
			GetCdr(temp2, &temp2);
		}
		if (callclang_apply(ptr, &pos, call, args))
			return 1;
		if (pos != Nil) goto result;
	}

result:
	rollback_local(local, stack);
	*ret = pos;
	return 0;

result_false:
	rollback_local(local, stack);
	*ret = Nil;
	return 0;
}


/*
 *  notany
 */
_g int notany_common(Execute ptr, addr call, addr rest, addr *ret)
{
	/* (notany predicate sequence*) ==  (not (some predicate sequence*)) */
	if (some_common(ptr, call, rest, &rest)) return 1;
	*ret = (rest == Nil)? T: Nil;
	return 0;
}


/*
 *  and
 */
_g void and_common(addr form, addr env, addr *ret)
{
	addr expr, when, andv;

	getcdr(form, &form);

	/* (and) */
	if (form == Nil) {
		*ret = T;
		return;
	}

	/* (and expr) */
	if (singlep(form)) {
		GetCar(form, ret);
		return;
	}

	/* (and expr . tail) -> (when expr (and . tail)) */
	GetCons(form, &expr, &form);
	GetConst(COMMON_WHEN, &when);
	GetConst(COMMON_AND, &andv);
	cons_heap(&andv, andv, form);
	list_heap(ret, when, expr, andv, NULL);
}


/*
 *  cond
 */
_g void cond_common(addr form, addr env, addr *ret)
{
	addr expr, tail, ifsym, progn, cond;

	getcdr(form, &form);

	/* (cond) */
	if (form == Nil) {
		*ret = Nil;
		return;
	}

	/* (cond clause ...) */
	/* (cond (expr . tail) . form)
	 *   `(if ,expr (progn ,@tail) (cond ,@form))
	 */
	if (! consp(form))
		fmte("The cond form ~S must be a cons.", form, NULL);
	GetCons(form, &expr, &form);
	if (! consp(expr))
		fmte("The cond clause ~S must be a cons.", expr, NULL);
	GetCons(expr, &expr, &tail);
	GetConst(COMMON_IF, &ifsym);
	GetConst(COMMON_PROGN, &progn);
	GetConst(COMMON_COND, &cond);
	cons_heap(&form, cond, form);
	cons_heap(&tail, progn, tail);
	list_heap(ret, ifsym, expr, tail, form, NULL);
}


/*
 *  or
 */
_g void or_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr gensym, let, ifsym, orv, expr;

	getcdr(form, &form);

	/* (or) */
	if (form == Nil) {
		*ret = Nil;
		return;
	}

	/* (or expr) */
	if (singlep(form)) {
		GetCar(form, ret);
		return;
	}

	/* (or expr . form) ->
	 *   (let ((#:g expr))
	 *     (if #:g #:g (or . form))) */
	if (! consp(form))
		fmte("The or form ~S must be a cons.", NULL);
	GetCons(form, &expr, &form);
	make_gensym(ptr, &gensym);
	GetConst(COMMON_LET, &let);
	GetConst(COMMON_IF, &ifsym);
	GetConst(COMMON_OR, &orv);
	list_heap(&expr, gensym, expr, NULL);
	conscar_heap(&expr, expr);
	cons_heap(&form, orv, form);
	list_heap(&form, ifsym, gensym, gensym, form, NULL);
	list_heap(ret, let, expr, form, NULL);
}


/*
 *  when
 */
_g void when_common(addr form, addr env, addr *ret)
{
	addr args, expr, ifsym, cons;

	getcdr(form, &args);
	if (! consp(args))
		fmte("The when ~S must be a (when test . body) form.", form, NULL);
	GetCons(args, &expr, &args);
	/* `(if ,expr (progn ,@body)) */
	GetConst(COMMON_PROGN, &cons);
	cons_heap(&cons, cons, args);
	GetConst(COMMON_IF, &ifsym);
	list_heap(ret, ifsym, expr, cons, NULL);
}


/*
 *  unless
 */
_g void unless_common(addr form, addr env, addr *ret)
{
	addr args, notv, expr, ifsym, cons;

	getcdr(form, &args);
	if (! consp(args))
		fmte("The unless ~S must be a (unless test . body) form.", form, NULL);
	GetCons(args, &expr, &args);
	/* `(if (not ,expr) (progn ,@body)) */
	GetConst(COMMON_PROGN, &cons);
	cons_heap(&cons, cons, args);
	GetConst(COMMON_NOT, &notv);
	list_heap(&expr, notv, expr, NULL);
	GetConst(COMMON_IF, &ifsym);
	list_heap(ret, ifsym, expr, cons, NULL);
}


/*
 *  case
 */
_g void case_common(Execute ptr, addr form, addr env, addr *ret)
{
	int lastp;
	addr key, args, list, test, body, g, root;
	addr let, cond, eql, member, quote, otherwise, declare, ignorable;

	/* (let ((g key))
	 *   (declare (ignorable g))
	 *   (cond ((eql g 'test1) . body1)
	 *         ((member g '(test2)) . body2)
	 *         (t . otherwise)))
	 */
	getcdr(form, &form);
	if (! consp(form))
		fmte("CASE argument must be (key &rest clauses) form.", form, NULL);
	GetCons(form, &key, &args);
	GetConst(COMMON_LET, &let);
	GetConst(COMMON_DECLARE, &declare);
	GetConst(COMMON_IGNORABLE, &ignorable);
	GetConst(COMMON_COND, &cond);
	GetConst(COMMON_EQL, &eql);
	GetConst(COMMON_MEMBER, &member);
	GetConst(COMMON_QUOTE, &quote);
	GetConst(COMMON_OTHERWISE, &otherwise);
	make_gensym(ptr, &g);

	lastp = 0;
	for (root = Nil; args != Nil; ) {
		if (! consp(args))
			fmte("CASE clauses ~S must be list type.", args, NULL);
		if (lastp)
			fmte("CASE clauses ~S don't appear after otherwise clause.", args, NULL);
		GetCons(args, &test, &args);
		if (! consp(test))
			fmte("CASE clauses ~S must be list type.", test, NULL);
		GetCons(test, &test, &body);
		if (test == T || test == otherwise) {
			cons_heap(&list, T, body);
			lastp = 1;
		}
		else {
			list_heap(&list, quote, test, NULL);
			list_heap(&list, consp(test)? member: eql, g, list, NULL);
			cons_heap(&list, list, body);
		}
		cons_heap(&root, list, root);
	}
	/* otherwise */
	if (lastp == 0) {
		list_heap(&list, T, Nil, NULL);
		cons_heap(&root, list, root);
	}
	/* cond form */
	nreverse_list_unsafe(&root, root);
	cons_heap(&root, cond, root);
	list_heap(&list, g, key, NULL);
	conscar_heap(&list, list);
	list_heap(&ignorable, ignorable, g, NULL);
	list_heap(&declare, declare, ignorable, NULL);
	list_heap(ret, let, list, declare, root, NULL);
}


/*
 *  ecase
 */
_g void ecase_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr key, args, list, test, body, g, root;
	addr let, cond, eql, member, quote, error, type, a;

	/* (let ((g key))
	 *   (cond ((eql g 'test1) . body1)
	 *         ((member g '(test2)) . body2)
	 *         (t . (type-error g (member ...)))))
	 */
	getcdr(form, &form);
	if (! consp(form))
		fmte("ECASE argument must be (key &rest clauses) form.", form, NULL);
	GetCons(form, &key, &args);
	GetConst(COMMON_LET, &let);
	GetConst(COMMON_COND, &cond);
	GetConst(COMMON_EQL, &eql);
	GetConst(COMMON_MEMBER, &member);
	GetConst(COMMON_QUOTE, &quote);
	GetConst(SYSTEM_ECASE_ERROR, &error);
	make_gensym(ptr, &g);

	type = Nil;
	for (root = Nil; args != Nil; ) {
		if (! consp(args))
			fmte("ECASE clauses ~S must be list type.", args, NULL);
		GetCons(args, &test, &args);
		if (! consp(test))
			fmte("ECASE clauses ~S must be list type.", test, NULL);
		GetCons(test, &test, &body);
		list_heap(&list, quote, test, NULL);
		if (consp(test)) {
			list_heap(&list, member, g, list, NULL);
			while (test != Nil) {
				getcons(test, &a, &test);
				cons_heap(&type, a, type);
			}
		}
		else {
			list_heap(&list, eql, g, list, NULL);
			cons_heap(&type, test, type);
		}
		cons_heap(&list, list, body);
		cons_heap(&root, list, root);
	}
	/* error */
	nreverse_list_unsafe(&type, type);
	list_heap(&type, quote, type, NULL);
	list_heap(&list, error, g, type, NULL);
	list_heap(&list, T, list, NULL);
	cons_heap(&root, list, root);
	/* cond form */
	nreverse_list_unsafe(&root, root);
	cons_heap(&root, cond, root);
	list_heap(&list, g, key, NULL);
	conscar_heap(&list, list);
	list_heap(ret, let, list, root, NULL);
}


/*
 *  ccase
 */
static int function_ccase_comma(Execute ptr, addr stream, addr x, int *first)
{
	if (*first)
		*first = 0;
	else
		print_ascii_stream(stream, ", ");
	return princ_print(ptr, stream, x);
}

static int function_ccase_string(Execute ptr,
		addr *ret, addr *rtype, addr place, addr args)
{
	int first;
	addr stream, pos, x, list;
	LocalHold hold;

	/* member */
	GetConst(COMMON_MEMBER, &list);
	conscar_heap(&list, list);
	/* stream */
	open_output_string_stream(&stream, 0);
	hold = LocalHold_array(ptr, 2);
	localhold_set(hold, 0, stream);
	localhold_set(hold, 1, list);
	if (format_stream(ptr, stream, "The value of ~A, ~~A, is not ", place, NULL))
		goto throw;
	/* loop */
	for (first = 1; args != Nil; ) {
		getcons(args, &pos, &args);
		getcar(pos, &pos);
		if (consp(pos)) {
			while (pos != Nil) {
				getcons(pos, &x, &pos);
				if (function_ccase_comma(ptr, stream, x, &first))
					goto throw;
				cons_heap(&list, x, list);
				localhold_set(hold, 1, list);
			}
		}
		else {
			if (function_ccase_comma(ptr, stream, pos, &first))
				goto throw;
			cons_heap(&list, pos, list);
			localhold_set(hold, 1, list);
		}
	}
	localhold_end(hold);

	write_char_stream(stream, '.');
	string_stream_heap(stream, ret);
	close_stream(stream);
	nreverse_list_unsafe(rtype, list);
	return 0;

throw:
	close_stream(stream);
	return 1;
}

static int function_ccase_expand(Execute ptr,
		addr env, addr *ret, addr place, addr args)
{
	/* (let* ((a1 b1) (a2 b2) ... (value r) g)
	 *   (declare (ignorable a1 a2 ...))
	 *   (block result
	 *     (tagbody
	 *       loop
	 *       (restart-bind
	 *         ((store-value
	 *            (lambda (v) (setq g v value v) w (go loop))
	 *            :report-function
	 *              (lambda (s)
	 *                (princ "Retry ccase with new value xx." s))
	 *            :interactive-function
	 *              (lambda ()
	 *                (list (eval (prompt-for t "Input xx> "))))))
	 *         (return-from result
	 *           (case value
	 *             (...)
	 *             (t (error
	 *                  (make-condition 'simple-type-error
	 *                    :datum value
	 *                    :expected-type '(member ...)
	 *                    :format-control "The value of xx, ~A, is not xx"
	 *                    :format-arguments (list value))))))))))
	 */
	addr a, b, g, r, w, v, s, str1, str2, str3;
	addr leta, declare, ignorable, tagbody, loop, restart, store, lambda, setq;
	addr value, go, report, inter, princ, list, eval, prompt, case_;
	addr quote, invoke, make, simple, datum, expect, control, arguments;
	addr x, y, root, type, block, retfrom, result;
	LocalHold hold;

	if (get_setf_expansion(ptr, place, env, &a, &b, &g, &w, &r))
		return 1;
	hold = LocalHold_local(ptr);
	localhold_pushva(hold, a, b, g, w, r, NULL);

	getcar(g, &g);
	Return(format_string(ptr, &str1,
				"Retry ccase with new value ~A.", place, NULL));
	localhold_push(hold, str1);
	Return(format_string(ptr, &str2,
				"Input ~A> ", place, NULL));
	localhold_push(hold, str2);
	if (function_ccase_string(ptr, &str3, &type, place, args))
		return 1;
	localhold_end(hold);

	make_symbolchar(&v, "V");
	make_symbolchar(&s, "STREAM");
	make_symbolchar(&loop, "LOOP");
	make_symbolchar(&value, "VALUE");
	make_symbolchar(&result, "RESULT");
	GetConst(COMMON_LETA, &leta);
	GetConst(COMMON_DECLARE, &declare);
	GetConst(COMMON_IGNORABLE, &ignorable);
	GetConst(COMMON_TAGBODY, &tagbody);
	GetConst(COMMON_RESTART_BIND, &restart);
	GetConst(COMMON_STORE_VALUE, &store);
	GetConst(COMMON_LAMBDA, &lambda);
	GetConst(COMMON_SETQ, &setq);
	GetConst(COMMON_GO, &go);
	GetConst(KEYWORD_REPORT_FUNCTION, &report);
	GetConst(KEYWORD_INTERACTIVE_FUNCTION, &inter);
	GetConst(COMMON_PRINC, &princ);
	GetConst(COMMON_LIST, &list);
	GetConst(COMMON_EVAL, &eval);
	GetConst(SYSTEM_PROMPT_FOR, &prompt);
	GetConst(COMMON_CASE, &case_);
	GetConst(COMMON_QUOTE, &quote);
	GetConst(COMMON_ERROR, &invoke);
	GetConst(COMMON_MAKE_CONDITION, &make);
	GetConst(COMMON_SIMPLE_TYPE_ERROR, &simple);
	GetConst(KEYWORD_DATUM, &datum);
	GetConst(KEYWORD_EXPECTED_TYPE, &expect);
	GetConst(KEYWORD_FORMAT_CONTROL, &control);
	GetConst(KEYWORD_FORMAT_ARGUMENTS, &arguments);
	GetConst(COMMON_BLOCK, &block);
	GetConst(COMMON_RETURN_FROM, &retfrom);
	/* expand */
	list_heap(&x, list, value, NULL);
	list_heap(&type, quote, type, NULL);
	list_heap(&simple, quote, simple, NULL);
	list_heap(&make, make, simple,
			datum, value, expect, type, control, str3, arguments, x, NULL);
	list_heap(&invoke, invoke, make, NULL);
	list_heap(&invoke, T, invoke, NULL);
	conscar_heap(&case_, case_);
	cons_heap(&case_, value, case_);
	while (args != Nil) {
		getcons(args, &x, &args);
		cons_heap(&case_, x, case_);
	}
	cons_heap(&case_, invoke, case_);
	nreverse_list_unsafe(&case_, case_);
	list_heap(&case_, retfrom, result, case_, NULL);
	list_heap(&prompt, prompt, T, str2, NULL);
	list_heap(&eval, eval, prompt, NULL);
	list_heap(&list, list, eval, NULL);
	list_heap(&x, lambda, Nil, list, NULL);
	list_heap(&princ, princ, str1, s, NULL);
	list_heap(&s, s, NULL);
	list_heap(&y, lambda, s, princ, NULL);
	list_heap(&go, go, loop, NULL);
	list_heap(&setq, setq, g, v, value, v, NULL);
	list_heap(&v, v, NULL);
	list_heap(&lambda, lambda, v, setq, w, go, NULL);
	list_heap(&store, store, lambda, report, y, inter, x, NULL);
	list_heap(&store, store, NULL);
	list_heap(&restart, restart, store, case_, NULL);
	list_heap(&tagbody, tagbody, loop, restart, NULL);
	list_heap(&tagbody, block, result, tagbody, NULL);
	/* let* */
	lista_heap(&ignorable, ignorable, a, NULL);
	list_heap(&declare, declare, ignorable, NULL);
	for (root = Nil; a != Nil; ) {
		getcons(a, &x, &a);
		getcons(b, &y, &b);
		list_heap(&x, x, y, NULL);
		cons_heap(&root, x, root);
	}
	list_heap(&value, value, r, NULL);
	cons_heap(&root, value, root);
	cons_heap(&root, g, root);
	nreverse_list_unsafe(&root, root);
	list_heap(ret, leta, root, declare, tagbody, NULL);

	return 0;
}

_g int ccase_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr args, x;
	LocalHold hold;

	hold = LocalHold_local(ptr);
	localhold_pushva_force(hold, form, env, NULL);
	getcdr(form, &form);
	if (! consp(form))
		goto error;
	GetCons(form, &x, &args);
	if (function_ccase_expand(ptr, env, ret, x, args))
		return 1;
	localhold_end(hold);
	return 0;

error:
	fmte("CCASE arguments ~S must be (place &rest args) form.", form, NULL);
	return 0;
}


/*
 *  typecase
 */
_g void typecase_common(Execute ptr, addr form, addr env, addr *ret)
{
	int lastp;
	addr key, args, list, test, body, g, root;
	addr let, cond, typep, quote, otherwise, declare, ignorable;

	/* (let ((g key))
	 *   (declare (ignorable g))
	 *   (cond ((typep g 'test1) . body1)
	 *         (t . otherwise)))
	 */
	getcdr(form, &form);
	if (! consp(form))
		fmte("TYPECASE argument must be (key &rest clauses) form.", form, NULL);
	GetCons(form, &key, &args);
	GetConst(COMMON_LET, &let);
	GetConst(COMMON_DECLARE, &declare);
	GetConst(COMMON_IGNORABLE, &ignorable);
	GetConst(COMMON_COND, &cond);
	GetConst(COMMON_TYPEP, &typep);
	GetConst(COMMON_QUOTE, &quote);
	GetConst(COMMON_OTHERWISE, &otherwise);
	make_gensym(ptr, &g);

	lastp = 0;
	for (root = Nil; args != Nil; ) {
		if (! consp(args))
			fmte("TYPECASE clauses ~S must be list type.", args, NULL);
		if (lastp) {
			fmte("TYPECASE clauses ~S don't "
					"appear after otherwise clause.", args, NULL);
		}
		GetCons(args, &test, &args);
		if (! consp(test))
			fmte("TYPECASE clauses ~S must be list type.", test, NULL);
		GetCons(test, &test, &body);
		if (test == T || test == otherwise) {
			cons_heap(&list, T, body);
			lastp = 1;
		}
		else {
			list_heap(&list, quote, test, NULL);
			list_heap(&list, typep, g, list, NULL);
			cons_heap(&list, list, body);
		}
		cons_heap(&root, list, root);
	}
	/* otherwise */
	if (lastp == 0) {
		list_heap(&list, T, Nil, NULL);
		cons_heap(&root, list, root);
	}
	/* cond form */
	nreverse_list_unsafe(&root, root);
	cons_heap(&root, cond, root);
	list_heap(&list, g, key, NULL);
	conscar_heap(&list, list);
	list_heap(&ignorable, ignorable, g, NULL);
	list_heap(&declare, declare, ignorable, NULL);
	list_heap(ret, let, list, declare, root, NULL);
}


/*
 *  etypecase
 */
_g void etypecase_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr key, args, list, test, body, g, root;
	addr let, cond, typep, quote, error, type;

	/* (let ((g key))
	 *   (cond ((typep g 'test1) . body1)
	 *         (t . (type-error g (or ...)))))
	 */
	getcdr(form, &form);
	if (! consp(form))
		fmte("ETYPECASE argument must be (key &rest clauses) form.", form, NULL);
	GetCons(form, &key, &args);
	GetConst(COMMON_LET, &let);
	GetConst(COMMON_COND, &cond);
	GetConst(COMMON_TYPEP, &typep);
	GetConst(COMMON_QUOTE, &quote);
	GetConst(SYSTEM_ETYPECASE_ERROR, &error);
	make_gensym(ptr, &g);

	type = Nil;
	for (root = Nil; args != Nil; ) {
		if (! consp(args))
			fmte("ETYPECASE clauses ~S must be list type.", args, NULL);
		GetCons(args, &test, &args);
		if (! consp(test))
			fmte("ETYPECASE clauses ~S must be list type.", test, NULL);
		GetCons(test, &test, &body);
		list_heap(&list, quote, test, NULL);
		list_heap(&list, typep, g, list, NULL);
		cons_heap(&type, test, type);
		cons_heap(&list, list, body);
		cons_heap(&root, list, root);
	}
	/* error */
	nreverse_list_unsafe(&type, type);
	list_heap(&type, quote, type, NULL);
	list_heap(&list, error, g, type, NULL);
	list_heap(&list, T, list, NULL);
	cons_heap(&root, list, root);
	/* cond form */
	nreverse_list_unsafe(&root, root);
	cons_heap(&root, cond, root);
	list_heap(&list, g, key, NULL);
	conscar_heap(&list, list);
	list_heap(ret, let, list, root, NULL);
}


/*
 *  ctypecase
 */
static void function_ctypecase_string(Execute ptr, addr *ret, addr args)
{
	addr list, pos;

	GetConst(COMMON_OR, &list);
	conscar_heap(&list, list);
	while (args != Nil) {
		getcons(args, &pos, &args);
		getcar(pos, &pos);
		cons_heap(&list, pos, list);
	}
	nreverse_list_unsafe(ret, list);
}

static int function_ctypecase_expand(Execute ptr,
		addr env, addr *ret, addr place, addr args)
{
	/* (let* ((a1 b1) (a2 b2) ... (value r) g)
	 *   (declare (ignorable a1 a2 ...))
	 *   (block result
	 *     (tagbody
	 *       loop
	 *       (restart-bind
	 *         ((store-value
	 *            (lambda (v) (setq g v value v) w (go loop))
	 *            :report-function
	 *              (lambda (s)
	 *                (princ "Retry ctypecase with new value xx." s))
	 *            :interactive-function
	 *              (lambda ()
	 *                (list (eval (prompt-for t "Input xx> "))))))
	 *         (return-from result
	 *           (typecase value
	 *             (...)
	 *             (t (error
	 *                  (make-condition 'type-error
	 *                    :datum value
	 *                    :expected-type '(or ...))))))))))
	 */
	addr a, b, g, r, w, v, s, str1, str2;
	addr leta, declare, ignorable, tagbody, loop, restart, store, lambda, setq;
	addr value, go, report, inter, princ, list, eval, prompt, case_;
	addr quote, invoke, make, simple, datum, expect;
	addr x, y, root, type, block, retfrom, result;
	LocalHold hold;

	if (get_setf_expansion(ptr, place, env, &a, &b, &g, &w, &r))
		return 1;
	hold = LocalHold_local(ptr);
	localhold_pushva(hold, a, b, g, w, r, NULL);

	getcar(g, &g);
	Return(format_string(ptr, &str1,
				"Retry ctypecase with new value ~A.", place, NULL));
	localhold_push(hold, str1);
	Return(format_string(ptr, &str2,
				"Input ~A> ", place, NULL));
	localhold_push(hold, str2);
	function_ctypecase_string(ptr, &type, args);
	localhold_end(hold);

	make_symbolchar(&v, "V");
	make_symbolchar(&s, "STREAM");
	make_symbolchar(&loop, "LOOP");
	make_symbolchar(&value, "VALUE");
	make_symbolchar(&result, "RESULT");
	GetConst(COMMON_LETA, &leta);
	GetConst(COMMON_DECLARE, &declare);
	GetConst(COMMON_IGNORABLE, &ignorable);
	GetConst(COMMON_TAGBODY, &tagbody);
	GetConst(COMMON_RESTART_BIND, &restart);
	GetConst(COMMON_STORE_VALUE, &store);
	GetConst(COMMON_LAMBDA, &lambda);
	GetConst(COMMON_SETQ, &setq);
	GetConst(COMMON_GO, &go);
	GetConst(KEYWORD_REPORT_FUNCTION, &report);
	GetConst(KEYWORD_INTERACTIVE_FUNCTION, &inter);
	GetConst(COMMON_PRINC, &princ);
	GetConst(COMMON_LIST, &list);
	GetConst(COMMON_EVAL, &eval);
	GetConst(SYSTEM_PROMPT_FOR, &prompt);
	GetConst(COMMON_TYPECASE, &case_);
	GetConst(COMMON_QUOTE, &quote);
	GetConst(COMMON_ERROR, &invoke);
	GetConst(COMMON_MAKE_CONDITION, &make);
	GetConst(COMMON_TYPE_ERROR, &simple);
	GetConst(KEYWORD_DATUM, &datum);
	GetConst(KEYWORD_EXPECTED_TYPE, &expect);
	GetConst(COMMON_BLOCK, &block);
	GetConst(COMMON_RETURN_FROM, &retfrom);
	/* expand */
	list_heap(&x, list, value, NULL);
	list_heap(&type, quote, type, NULL);
	list_heap(&simple, quote, simple, NULL);
	list_heap(&make, make, simple, datum, value, expect, type, NULL);
	list_heap(&invoke, invoke, make, NULL);
	list_heap(&invoke, T, invoke, NULL);
	conscar_heap(&case_, case_);
	cons_heap(&case_, value, case_);
	while (args != Nil) {
		getcons(args, &x, &args);
		cons_heap(&case_, x, case_);
	}
	cons_heap(&case_, invoke, case_);
	nreverse_list_unsafe(&case_, case_);
	list_heap(&case_, retfrom, result, case_, NULL);
	list_heap(&prompt, prompt, T, str2, NULL);
	list_heap(&eval, eval, prompt, NULL);
	list_heap(&list, list, eval, NULL);
	list_heap(&x, lambda, Nil, list, NULL);
	list_heap(&princ, princ, str1, s, NULL);
	list_heap(&s, s, NULL);
	list_heap(&y, lambda, s, princ, NULL);
	list_heap(&go, go, loop, NULL);
	list_heap(&setq, setq, g, v, value, v, NULL);
	list_heap(&v, v, NULL);
	list_heap(&lambda, lambda, v, setq, w, go, NULL);
	list_heap(&store, store, lambda, report, y, inter, x, NULL);
	list_heap(&store, store, NULL);
	list_heap(&restart, restart, store, case_, NULL);
	list_heap(&tagbody, tagbody, loop, restart, NULL);
	list_heap(&tagbody, block, result, tagbody, NULL);
	/* let* */
	lista_heap(&ignorable, ignorable, a, NULL);
	list_heap(&declare, declare, ignorable, NULL);
	for (root = Nil; a != Nil; ) {
		getcons(a, &x, &a);
		getcons(b, &y, &b);
		list_heap(&x, x, y, NULL);
		cons_heap(&root, x, root);
	}
	list_heap(&value, value, r, NULL);
	cons_heap(&root, value, root);
	cons_heap(&root, g, root);
	nreverse_list_unsafe(&root, root);
	list_heap(ret, leta, root, declare, tagbody, NULL);

	return 0;
}

_g int ctypecase_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr args, x;
	LocalHold hold;

	hold = LocalHold_local(ptr);
	localhold_pushva_force(hold, form, env, NULL);
	getcdr(form, &form);
	if (! consp(form))
		goto error;
	GetCons(form, &x, &args);
	if (function_ctypecase_expand(ptr, env, ret, x, args))
		return 1;
	localhold_end(hold);
	return 0;

error:
	fmte("CTYPECASE arguments ~S must be (place &rest args) form.", form, NULL);
	return 0;
}


/*
 *  multiple-value-bind
 */
_g int multiple_value_bind_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr list, pos, vars, expr, doc, decl;
	LocalHold hold;

	hold = LocalHold_local(ptr);
	localhold_pushva_force(hold, form, env, NULL);
	/* argument */
	getcdr(form, &form);
	if (! consp(form)) goto error;
	getcons(form, &vars, &form);
	for (list = vars; list != Nil; ) {
		getcons(list, &pos, &list);
		check_variable(pos);
	}
	if (! consp(form)) goto error;
	getcons(form, &expr, &form);
	/* extract */
	if (declare_body_documentation(ptr, env, form, &doc, &decl, &form))
		return 1;
	localhold_end(hold);
	GetConst(SYSTEM_MULTIPLE_VALUE_BIND, &pos);
	list_heap(ret, pos, vars, expr, decl, doc, form, NULL);
	return 0;

error:
	fmte("The multiple-value-bind argument must be a "
			"((vars*) expr &body body) form.", NULL);
	return 0;
}


/*
 *  multiple-value-list
 */
_g void multiple_value_list_common(addr form, addr env, addr *ret)
{
	addr args, expr, symbol, func, list;

	getcdr(form, &args);
	if (! consp(args)) goto error;
	GetCons(args, &expr, &args);
	if (args != Nil) goto error;

	/* `(multiple-value-call #'list ,expr) */
	GetConst(COMMON_MULTIPLE_VALUE_CALL, &symbol);
	GetConst(COMMON_FUNCTION, &func);
	GetConst(COMMON_LIST, &list);
	list_heap(&list, func, list, NULL);
	list_heap(ret, symbol, list, expr, NULL);
	return;

error:
	fmte("The multiple-value-list argument ~S must be a single list.", form, NULL);
}


/*
 *  multiple-value-setq
 */
_g void multiple_value_setq_common(addr form, addr env, addr *ret)
{
	addr args, vars, expr, values, setf;

	getcdr(form, &args);
	if (! consp(args)) goto error;
	GetCons(args, &vars, &args);
	if (! consp(args)) goto error;
	GetCons(args, &expr, &args);
	if (args != Nil) goto error;

	/* `(values (setf (values ,@vars) ,expr)) */
	GetConst(COMMON_VALUES, &values);
	GetConst(COMMON_SETF, &setf);
	cons_heap(&vars, values, vars);
	list_heap(&vars, setf, vars, expr, NULL);
	list_heap(ret, values, vars, NULL);
	return;

error:
	fmte("The multiple-value-setq arguments ~S must be a (vars form).", form, NULL);
}


/*
 *  nth-value
 */
_g void nth_value_common(addr form, addr env, addr *ret)
{
	addr args, nth, expr, nth_value;

	getcdr(form, &form);
	if (! consp(form)) goto error;
	GetCons(form, &nth, &args);
	if (! consp(args)) goto error;
	GetCons(args, &expr, &args);
	if (args != Nil) goto error;
	GetConst(SYSTEM_NTH_VALUE, &nth_value);
	list_heap(ret, nth_value, nth, expr, NULL);
	return;

error:
	fmte("NTH-VALUE argument ~S must be (nth expr) form.", form, NULL);
}


/*
 *  prog
 */
static void function_prog_constant(addr form, addr *ret,
		constindex prog_constant,
		constindex let_constant)
{
	/*  `(block nil
	 *    (let ,var
	 *      ,@decl
	 *      (tagbody ,@body)))
	 */
	addr var, decl, root, pos;
	addr let, block, tagbody;

	/* argument */
	getcdr(form, &form);
	if (! consp(form)) {
		GetConstant(prog_constant, &var);
		fmte("~A argument ~S must be ([var] &rest body) form.", var, form, NULL);
	}
	GetCons(form, &var, &form);
	declare_body_form(form, &decl, &form);

	/* expand */
	GetConstant(let_constant, &let);
	GetConst(COMMON_BLOCK, &block);
	GetConst(COMMON_TAGBODY, &tagbody);
	/* (tagbody ...) */
	cons_heap(&form, tagbody, form);
	/* (let ...) */
	conscar_heap(&root, let);
	cons_heap(&root, var, root);
	while (decl != Nil) {
		GetCons(decl, &pos, &decl);
		cons_heap(&root, pos, root);
	}
	cons_heap(&root, form, root);
	nreverse_list_unsafe(&root, root);
	/* (block ...) */
	list_heap(ret, block, Nil, root, NULL);
}

_g void prog_common(addr form, addr env, addr *ret)
{
	function_prog_constant(form, ret,
			CONSTANT_COMMON_PROG,
			CONSTANT_COMMON_LET);
}


/*
 *  prog*
 */
_g void proga_common(addr form, addr env, addr *ret)
{
	function_prog_constant(form, ret,
			CONSTANT_COMMON_PROGA,
			CONSTANT_COMMON_LETA);
}


/*
 *  prog1
 */
_g void prog1_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr expr, g, let, root;

	getcdr(form, &form);
	if (! consp(form))
		fmte("PROG1 arguemnt ~S must be (form1 &body body) form.", form, NULL);
	GetCons(form, &expr, &form);
	if (form == Nil) {
		*ret = expr;
		return;
	}
	/* `(let ((,g ,expr)) ,@form ,g) */
	make_gensym(ptr, &g);
	GetConst(COMMON_LET, &let);
	list_heap(&expr, g, expr, NULL);
	conscar_heap(&expr, expr);
	conscar_heap(&root, let);
	cons_heap(&root, expr, root);
	while (form != Nil) {
		if (! consp(form))
			fmte("PROG1 argument ~S don't accept a dotted list.", form, NULL);
		GetCons(form, &expr, &form);
		cons_heap(&root, expr, root);
	}
	cons_heap(&root, g, root);
	nreverse_list_unsafe(ret, root);
}


/*
 *  prog2
 */
_g void prog2_common(addr form, addr env, addr *ret)
{
	addr expr, progn, prog1;

	getcdr(form, &form);
	if (! consp(form)) goto error;
	GetCons(form, &expr, &form);
	if (! consp(form)) goto error;
	/* `(progn ,expr (prog1 ,@form)) */
	GetConst(COMMON_PROGN, &progn);
	GetConst(COMMON_PROG1, &prog1);
	cons_heap(&prog1, prog1, form);
	list_heap(ret, progn, expr, prog1, NULL);
	return;

error:
	fmte("PROG2 arguemnt ~S must be (form1 form2 &body body) form.", form, NULL);
}


/*
 *  define-modify-macro
 */
static int define_modify_macro_find(addr key, addr lambda, addr *ret)
{
	addr args, check;

	for (args = lambda; args != Nil; ) {
		if (! consp(args)) goto error;
		GetCons(args, &check, &args);
		if (check == key) {
			if (! consp(args)) goto error;
			GetCar(args, ret);
			return 1;
		}
	}
	return 0;

error:
	fmte("Invaild macro-lambda-list ~S.", lambda, NULL);
	return 0;
}

static void define_modify_macro_expand(LocalRoot local, addr *ret,
		addr name, addr args, addr call, addr doc)
{
	addr place, env, whole, key;
	addr defmacro, mvbind, expansion, quote, let, mapcar, function;
	addr list, lista, cddr, cons, values, declare, ignorable;
	addr a, b, g, w, r, qmvbind, list1, list2, list3;

	/* place */
	make_symbolchar(&place, "PLACE");
	cons_heap(&args, place, args);
	/* &environment */
	GetConst(AMPERSAND_ENVIRONMENT, &key);
	if (! define_modify_macro_find(key, args, &env)) {
		make_symbolchar(&env, "ENV");
		lista_heap(&args, key, env, args, NULL);
	}
	/* &whole */
	GetConst(AMPERSAND_WHOLE, &key);
	if (! define_modify_macro_find(key, args, &whole)) {
		make_symbolchar(&whole, "WHOLE");
		lista_heap(&args, key, whole, args, NULL);
	}

	/* expand
	 *
	 *  (defmacro name (&whole whole &environment env var &rest args)
	 *    (declare (ignorable ...))
	 *    (multiple-value-bind (a b g w r) (get-setf-expansion var env)
	 *      `(let ,(mapcar #'list a b)
	 *        (multiple-value-bind ,g (call ,r ,@(cddr whole))
	 *          ,w
	 *          (values ,@g)))))
	 *
	 *  (defmacro name (&whole whole &environment env var &rest args)
	 *    (declare (ignorable ...))
	 *    (multiple-value-bind (a b g w r) (get-setf-expansion var env)
	 *      (list (quote let) (mapcar (function list) a b)
	 *        (list (quote multiple-value-bind) g
	 *          (list* (quote call) r (cddr whole)) w
	 *          (cons (quote values) g)))))
	 */
	GetConst(COMMON_DEFMACRO, &defmacro);
	GetConst(COMMON_MULTIPLE_VALUE_BIND, &mvbind);
	GetConst(COMMON_GET_SETF_EXPANSION, &expansion);
	GetConst(COMMON_QUOTE, &quote);
	GetConst(COMMON_LET, &let);
	GetConst(COMMON_MAPCAR, &mapcar);
	GetConst(COMMON_FUNCTION, &function);
	GetConst(COMMON_LIST, &list);
	GetConst(COMMON_LISTA, &lista);
	GetConst(COMMON_CDDR, &cddr);
	GetConst(COMMON_CONS, &cons);
	GetConst(COMMON_VALUES, &values);
	GetConst(COMMON_DECLARE, &declare);
	GetConst(COMMON_IGNORABLE, &ignorable);
	make_symbolchar(&a, "A");
	make_symbolchar(&b, "B");
	make_symbolchar(&g, "G");
	make_symbolchar(&w, "W");
	make_symbolchar(&r, "R");
	allsymbol_macro_lambda_heap(local, &list1, args);
	cons_heap(&ignorable, ignorable, list1);
	list_heap(&declare, declare, ignorable, NULL);
	list_heap(&values, quote, values, NULL);
	list_heap(&cons, cons, values, g, NULL);
	list_heap(&cddr, cddr, whole, NULL);
	list_heap(&call, quote, call, NULL);
	list_heap(&lista, lista, call, r, cddr, NULL);
	list_heap(&qmvbind, quote, mvbind, NULL);
	list_heap(&list2, list, qmvbind, g, lista, w, cons, NULL);
	list_heap(&function, function, list, NULL);
	list_heap(&mapcar, mapcar, function, a, b, NULL);
	list_heap(&let, quote, let, NULL);
	list_heap(&list1, list, let, mapcar, list2, NULL);
	list_heap(&expansion, expansion, place, env, NULL);
	list_heap(&list3, a, b, g, w, r, NULL);
	list_heap(&mvbind, mvbind, list3, expansion, list1, NULL);
	list_heap(ret, defmacro, name, args, declare, mvbind, NULL);
}

_g void define_modify_macro_common(LocalRoot local, addr form, addr env, addr *ret)
{
	addr args, name, lambda, call, doc;

	getcdr(form, &form);
	if (! consp(form)) goto error;
	GetCons(form, &name, &args);
	if (! consp(args)) goto error;
	GetCons(args, &lambda, &args);
	if (! consp(args)) goto error;
	GetCons(args, &call, &args);
	if (args == Nil) {
		doc = Nil;
		goto expand;
	}
	GetCons(args, &doc, &args);
	if (args != Nil) goto error;
	if (! stringp(doc)) {
		fmte("DEFINE-MODIFY-MACRO documentation ~S "
				"must be a string type.", doc, NULL);
	}
expand:
	define_modify_macro_expand(local, ret, name, lambda, call, doc);
	return;

error:
	fmte("DEFINE-MODIFY-MACRO argument ~S must be "
			"(name lambda-list functionn &optional documentation) "
			"form.", form, NULL);
}


/*
 *  defsetf
 */
static void defsetf_short_common(addr access, addr update, addr doc, addr *ret)
{
	/* `(define-setf-expander ,access (&environment env &rest args)
	 *   (system::defsetf-short ',access ',update, args env))
	 */
	addr define, defsetf, rest, args, envi, env, quote, value;

	GetConst(COMMON_DEFINE_SETF_EXPANDER, &define);
	GetConst(SYSTEM_DEFSETF_SHORT, &defsetf);
	GetConst(AMPERSAND_REST, &rest);
	GetConst(AMPERSAND_ENVIRONMENT, &envi);
	GetConst(COMMON_QUOTE, &quote);
	make_symbolchar(&args, "ARGS");
	make_symbolchar(&env, "ENV");

	list_heap(&update, quote, update, NULL);
	list_heap(&value, quote, access, NULL);
	list_heap(&defsetf, defsetf, value, update, args, env, NULL);
	list_heap(&rest, envi, env, rest, args, NULL);
	if (doc == Nil)
		list_heap(&define, define, access, rest, defsetf, NULL);
	else
		list_heap(&define, define, access, rest, doc, defsetf, NULL);
	*ret = define;
}

static void defsetf_long_common(addr access,
		addr lambda, addr store, addr body, addr *ret)
{
	/* `(define-setf-expander ,access (&environment env &rest args)
	 *   (system::defsetf-long ,access ,lambda ,store ,body args env))
	 */
	addr define, defsetf, rest, args, envi, env, quote, value;

	GetConst(COMMON_DEFINE_SETF_EXPANDER, &define);
	GetConst(SYSTEM_DEFSETF_LONG, &defsetf);
	GetConst(AMPERSAND_REST, &rest);
	GetConst(AMPERSAND_ENVIRONMENT, &envi);
	GetConst(COMMON_QUOTE, &quote);
	make_symbolchar(&args, "ARGS");
	make_symbolchar(&env, "ENV");

	list_heap(&lambda, quote, lambda, NULL);
	list_heap(&store, quote, store, NULL);
	list_heap(&body, quote, body, NULL);
	list_heap(&value, quote, access, NULL);
	list_heap(&defsetf, defsetf, value, lambda, store, body, args, env, NULL);
	list_heap(&rest, envi, env, rest, args, NULL);
	list_heap(ret, define, access, rest, defsetf, NULL);
}

_g void defsetf_common(addr form, addr env, addr *ret)
{
	addr args, arg1, arg2, arg3;

	getcdr(form, &form);
	if (! consp(form)) goto error;
	GetCons(form, &arg1, &args);
	if (! consp(args)) goto error;
	GetCons(args, &arg2, &args);
	if (listp(arg2)) {
		/* long form */
		if (! consp(args))
			fmte("Invalid defsetf long form ~S.", form, NULL);
		GetCons(args, &arg3, &args);
		if (! listp(arg3))
			fmte("defsetf argument ~S must be a list type.", arg3, NULL);
		defsetf_long_common(arg1, arg2, arg3, args, ret);
	}
	else if (args == Nil) {
		/* short form */
		defsetf_short_common(arg1, arg2, Nil, ret);
	}
	else {
		/* short form, documentation */
		if (! consp(args))
			fmte("Invalid defsetf short form ~S.", form, NULL);
		GetCons(args, &arg3, &args);
		if (args != Nil)
			fmte("Invalid defsetf short form ~S.", form, NULL);
		if (! stringp(arg3))
			fmte("defsetf documentation ~S must be a string type.", arg3, NULL);
		defsetf_short_common(arg1, arg2, arg3, ret);
	}
	return;

error:
	fmte("Invalid defsetf form ~S.", form, NULL);
}


/*
 *  define-setf-expander
 */
_g void define_setf_expander_common(addr form, addr env, addr *ret)
{
	/* `(system::define-setf-expander
	 *     ',access
	 *     (system::macro-lambda ,@args))
	 */
	addr access, args, define, lambda, quote;

	getcdr(form, &form);
	if (! consp(form)) goto error;
	GetCons(form, &access, &args);
	if (! consp(args)) goto error;
	/* expand */
	GetConst(SYSTEM_DEFINE_SETF_EXPANDER, &define);
	GetConst(SYSTEM_MACRO_LAMBDA, &lambda);
	GetConst(COMMON_QUOTE, &quote);
	cons_heap(&lambda, lambda, args);
	list_heap(&access, quote, access, NULL);
	list_heap(ret, define, access, lambda, NULL);
	return;

error:
	fmte("DEFINE-SETF-EXPANDER argument ~S "
			"must be (access lambda-list &rest body) form.", form, NULL);
}


/*
 *  setf
 */
static void setf_single_common(addr *ret, addr value,
		addr var1, addr var2, addr store, addr writer, addr reader)
{
	/* (let* ((g1 a1)
	 *        (g2 a2)
	 *        (g value))
	 *   (declare (ignorable g1 g2))
	 *     writer
	 *     g)
	 */
	addr list1, list2, args, a, b, leta, declare, ignorable;

	list1 = var1;
	list2 = var2;
	args = Nil;
	while (list1 != Nil) {
		getcons(list1, &a, &list1);
		getcons(list2, &b, &list2);
		list_heap(&a, a, b, NULL);
		cons_heap(&args, a, args);
	}
	GetCar(store, &store);
	list_heap(&a, store, value, NULL);
	cons_heap(&args, a, args);
	nreverse_list_unsafe(&args, args);
	/* declare */
	GetConst(COMMON_IGNORABLE, &ignorable);
	cons_heap(&ignorable, ignorable, var1);
	GetConst(COMMON_DECLARE, &declare);
	list_heap(&declare, declare, ignorable, NULL);
	/* let* */
	GetConst(COMMON_LETA, &leta);
	list_heap(ret, leta, args, declare, writer, store, NULL);
}

static void setf_multiple_common(addr *ret, addr value,
		addr var1, addr var2, addr store, addr writer, addr reader)
{
	/* (let* ((g1 a1)
	 *        (g2 a2))
	 *   (declare (ignorable g1 g2))
	 *   (multiple-value-bind (g ...) value
	 *     writer
	 *     (values g1 ...)))
	 */
	addr list1, list2, args, a, b, leta, declare, ignorable, bind, values;

	list1 = var1;
	list2 = var2;
	args = Nil;
	while (list1 != Nil) {
		getcons(list1, &a, &list1);
		getcons(list2, &b, &list2);
		list_heap(&a, a, b, NULL);
		cons_heap(&args, a, args);
	}
	nreverse_list_unsafe(&args, args);
	/* declare */
	GetConst(COMMON_IGNORABLE, &ignorable);
	cons_heap(&ignorable, ignorable, var1);
	GetConst(COMMON_DECLARE, &declare);
	list_heap(&declare, declare, ignorable, NULL);
	/* values */
	GetConst(COMMON_VALUES, &values);
	cons_heap(&values, values, store);
	/* multiple-value-bind */
	GetConst(COMMON_MULTIPLE_VALUE_BIND, &bind);
	list_heap(&bind, bind, store, value, writer, values, NULL);
	/* let* */
	GetConst(COMMON_LETA, &leta);
	list_heap(ret, leta, args, declare, bind, NULL);
}

static int setf_expr_common(Execute ptr, addr *ret, addr key, addr value, addr env)
{
	addr a, b, g, w, r;

	if (get_setf_expansion(ptr, key, env, &a, &b, &g, &w, &r))
		return 1;
	if (singlep(g))
		setf_single_common(ret, value, a, b, g, w, r);
	else
		setf_multiple_common(ret, value, a, b, g, w, r);

	return 0;
}

_g int setf_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr key, value, root, progn;
	LocalHold hold;

	getcdr(form, &form);
	if (form == Nil) {
		*ret = Nil;
		return 0;
	}

	hold = LocalHold_array(ptr, 3);
	localhold_set(hold, 0, form);
	localhold_set(hold, 1, env);
	for (root = Nil; form != Nil; ) {
		if (! consp(form)) goto error;
		GetCons(form, &key, &form);
		if (! consp(form)) goto error;
		GetCons(form, &value, &form);
		if (setf_expr_common(ptr, &key, key, value, env))
			return 1;
		cons_heap(&root, key, root);
		localhold_set(hold, 2, root);
	}
	localhold_end(hold);
	nreverse_list_unsafe(&root, root);

	/* (progn ...) */
	GetConst(COMMON_PROGN, &progn);
	cons_heap(ret, progn, root);
	return 0;

error:
	fmte("The setf form ~S must be a place value form.", form, NULL);
	return 0;
}


/*
 *  shiftf
 */
static void shiftf_list2_common(addr *ret, addr a, addr b, addr root)
{
	addr x, y;

	while (a != Nil) {
		getcons(a, &x, &a);
		getcons(b, &y, &b);
		list_heap(&x, x, y, NULL);
		cons_heap(&root, x, root);
	}
	*ret = root;
}

static void shiftf_ignorable_common(addr *ret, addr list)
{
	addr declare, ignorable;

	GetConst(COMMON_DECLARE, &declare);
	GetConst(COMMON_IGNORABLE, &ignorable);
	cons_heap(&ignorable, ignorable, list);
	list_heap(ret, declare, ignorable, NULL);
}

static void shiftf_mvbind_common(addr *ret, addr g, addr r, addr body)
{
	addr mvbind, declare;

	GetConst(COMMON_MULTIPLE_VALUE_BIND, &mvbind);
	shiftf_ignorable_common(&declare, g);
	lista_heap(ret, mvbind, g, r, declare, body, NULL);
}

_g int shiftf_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr args, root, pos, let, declare, prog1;
	addr a, b, g, w, r, r0, alist, glist, wlist, rlist;
	LocalHold hold;

	/* (shiftf x0 x1 x2 value)
	 * (let* (...)
	 *   (multiple-value-prog1 r0
	 *     (multiple-value-bind (g0) r1
	 *     (declare (ignorable g0))
	 *       (multiple-value-bind (g1) r2
	 *       (declare (ignorable g1))
	 *         (multiple-value-bind (g2) value
	 *         (declare (ignorable g2))
	 *           w0 w1 w2)))))
	 */
	hold = LocalHold_array(ptr, 4);
	localhold_pushva_force(hold, form, env, NULL);

	getcdr(form, &form);
	if (! consp(form)) goto error;
	getcons(form, &pos, &args);
	if (! consp(args)) goto error;

	/* push */
	if (get_setf_expansion(ptr, pos, env, &a, &b, &g, &w, &r0))
		return 1;
	alist = glist = wlist = rlist = Nil;
	shiftf_list2_common(&alist, a, b, alist);
	cons_heap(&glist, g, glist);
	cons_heap(&wlist, w, wlist);
	localhold_set(hold, 0, alist);
	localhold_set(hold, 1, glist);
	localhold_set(hold, 2, wlist);
	for (;;) {
		getcons(args, &pos, &args);
		if (args == Nil)
			break;
		if (get_setf_expansion(ptr, pos, env, &a, &b, &g, &w, &r))
			return 1;
		shiftf_list2_common(&alist, a, b, alist);
		cons_heap(&glist, g, glist);
		cons_heap(&wlist, w, wlist);
		cons_heap(&rlist, r, rlist);
		localhold_set(hold, 0, alist);
		localhold_set(hold, 1, glist);
		localhold_set(hold, 2, wlist);
		localhold_set(hold, 3, rlist);
	}
	localhold_end(hold);

	/* last expand */
	nreverse_list_unsafe(&wlist, wlist);
	GetCons(glist, &g, &glist);
	shiftf_mvbind_common(&root, g, pos, wlist);

	/* loop expand */
	while (glist != Nil) {
		GetCons(glist, &g, &glist);
		GetCons(rlist, &r, &rlist);
		list_heap(&root, root, NULL);
		shiftf_mvbind_common(&root, g, r, root);
	}

	/* multiple-value-prog1 */
	GetConst(COMMON_MULTIPLE_VALUE_PROG1, &prog1);
	list_heap(&root, prog1, r0, root, NULL);

	/* let expand */
	if (alist != Nil) {
		nreverse_list_unsafe(&alist, alist);
		GetConst(COMMON_LETA, &let);
		shiftf_ignorable_common(&declare, a);
		list_heap(&root, let, alist, declare, root, NULL);
	}

	/* result */
	*ret = root;
	return 0;

error:
	fmte("SHIFT argument ~S must be (place ... value) form.", form, NULL);
	return 0;
}


/*
 *  rotatef
 */
_g int rotatef_common(Execute ptr, addr form, addr env, addr *ret)
{
	addr args, root, pos, let, declare;
	addr a, b, g, w, r, r0, alist, glist, wlist, rlist;
	LocalHold hold;

	hold = LocalHold_array(ptr, 4);
	localhold_pushva_force(hold, form, env, NULL);

	getcdr(form, &form);
	if (form == Nil) {
		*ret = Nil;
		localhold_end(hold);
		return 0;
	}
	if (! consp(form)) goto error;
	getcons(form, &pos, &args);
	if (args == Nil) {
		/* (progn pos nil) */
		GetConst(COMMON_PROGN, &root);
		list_heap(ret, root, pos, Nil, NULL);
		localhold_end(hold);
		return 0;
	}
	if (! consp(args)) goto error;

	/* push */
	if (get_setf_expansion(ptr, pos, env, &a, &b, &g, &w, &r0))
		return 1;
	alist = glist = wlist = rlist = Nil;
	shiftf_list2_common(&alist, a, b, alist);
	cons_heap(&glist, g, glist);
	cons_heap(&wlist, w, wlist);
	localhold_set(hold, 0, alist);
	localhold_set(hold, 1, glist);
	localhold_set(hold, 2, wlist);
	while (args != Nil) {
		getcons(args, &pos, &args);
		if (get_setf_expansion(ptr, pos, env, &a, &b, &g, &w, &r))
			return 1;
		shiftf_list2_common(&alist, a, b, alist);
		cons_heap(&glist, g, glist);
		cons_heap(&wlist, w, wlist);
		cons_heap(&rlist, r, rlist);
		localhold_set(hold, 0, alist);
		localhold_set(hold, 1, glist);
		localhold_set(hold, 2, wlist);
		localhold_set(hold, 3, rlist);
	}
	localhold_end(hold);

	/* last expand */
	cons_heap(&wlist, Nil, wlist);
	nreverse_list_unsafe(&wlist, wlist);
	GetCons(glist, &g, &glist);
	shiftf_mvbind_common(&root, g, r0, wlist);

	/* loop expand */
	while (glist != Nil) {
		GetCons(glist, &g, &glist);
		GetCons(rlist, &r, &rlist);
		list_heap(&root, root, NULL);
		shiftf_mvbind_common(&root, g, r, root);
	}

	/* let expand */
	if (alist != Nil) {
		nreverse_list_unsafe(&alist, alist);
		GetConst(COMMON_LETA, &let);
		shiftf_ignorable_common(&declare, a);
		list_heap(&root, let, alist, declare, root, NULL);
	}

	/* result */
	*ret = root;
	return 0;

error:
	fmte("ROTATEF argument ~S don't accept a dotted list.", form, NULL);
	return 0;
}

