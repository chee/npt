/*
 *  ANSI COMMON LISP: 25. Environment
 */
#include "common_header.h"
#include "cons.h"
#include "env_code.h"
#include "env_describe.h"
#include "env_function.h"
#include "env_time.h"
#include "env_version.h"
#include "stream.h"
#include "type_parse.h"

/* (defun decode-universal-time (time &optional zone) ...)
 *     -> second, minute, hour, day, month, year, day, daylight-p, zone
 *   time        integer
 *   zone        (or null (rational -24 24))
 *   second      (integer 0 59)  ;; ignoring leap seconds
 *   minute      (integer 0 59)
 *   hour        (integer 0 23)
 *   day         (integer 1 31)
 *   month       (integer 1 12)
 *   year        integer
 *   week        (integer 0 6)  ;; 0 means Monday.
 *   daylight-p  boolean
 */
static void function_decode_universal_time(Execute ptr, addr pos, addr zone)
{
	struct universal_time_struct u;

	if (zone == Unbound) zone = Nil;
	decode_universal_time_common(ptr->local, &u, pos, zone);
	setvalues_control(ptr,
			u.second, u.minute, u.hour,
			u.date, u.month, u.year,
			u.week, u.daylight_p, u.zone, NULL);
}

static void type_decode_universal_time(addr *ret)
{
	addr args, values;

	GetTypeTable(&args, Intplus);
	GetTypeTable(&values, TimeZone);
	typeargs_var1opt1(&args, args, values);
	GetTypeValues(&values, DecodeUniversalTime);
	type_compiled_heap(args, values, ret);
}

static void defun_decode_universal_time(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_DECODE_UNIVERSAL_TIME, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_var1opt1(pos, p_defun_decode_universal_time);
	SetFunctionCommon(symbol, pos);
	/* type */
	type_decode_universal_time(&type);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun encode-universal-time
 *     (second minute hour day month year &optional zone) ...)
 *     -> universal-time
 *   second          (integer 0 59)
 *   minute          (integer 0 59)
 *   hour            (integer 0 23)
 *   day             (integer 1 31)
 *   month           (integer 1 12)
 *   year            (integer 0 *)
 *   zone            (rational -24 24)
 *   universal-time  (integer 0 *)
 */
static void function_encode_universal_time(Execute ptr, addr rest)
{
	addr s, mi, h, d, m, y, z;

	if (! consp_getcons(rest, &s, &rest)) goto error;
	if (! consp_getcons(rest, &mi, &rest)) goto error;
	if (! consp_getcons(rest, &h, &rest)) goto error;
	if (! consp_getcons(rest, &d, &rest)) goto error;
	if (! consp_getcons(rest, &m, &rest)) goto error;
	if (! consp_getcons(rest, &y, &rest)) goto error;
	if (! consp_getcons(rest, &z, &rest)) z = Unbound;
	if (consp(rest)) goto error;
	encode_universal_time_common(ptr->local, &s, s, mi, h, d, m, y, z);
	setresult_control(ptr, s);
	return;

error:
	fmte("Invalid argument ENCODE-UNIVERSAL-TIME.", NULL);
}

static void type_encode_universal_time(addr *ret)
{
	addr args, values, plus, sec, hour, day, month, zone;

	GetTypeTable(&sec, TimeSecond);
	GetTypeTable(&hour, TimeHour);
	GetTypeTable(&day, TimeDay);
	GetTypeTable(&month, TimeMonth);
	GetTypeTable(&plus, Intplus);
	GetTypeTable(&zone, TimeZone);
	list_heap(&args, sec, sec, hour, day, month, plus, NULL);
	list_heap(&values, zone, NULL);
	typeargs_full(&args, args, values, Nil, Nil);
	GetTypeValues(&values, Intplus);
	type_compiled_heap(args, values, ret);
}

static void defun_encode_universal_time(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_ENCODE_UNIVERSAL_TIME, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_dynamic(pos, p_defun_encode_universal_time);
	SetFunctionCommon(symbol, pos);
	/* type */
	type_encode_universal_time(&type);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun get-universal-time () ...) -> universal-time */
static void function_get_universal_time(Execute ptr)
{
	addr pos;
	get_universal_time_common(ptr->local, &pos);
	setresult_control(ptr, pos);
}

static void type_get_universal_time(addr *ret)
{
	addr args, values;

	GetTypeArgs(&args, Empty);
	GetTypeValues(&values, Intplus);
	type_compiled_heap(args, values, ret);
}

static void defun_get_universal_time(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_GET_UNIVERSAL_TIME, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_get_universal_time);
	SetFunctionCommon(symbol, pos);
	/* type */
	type_get_universal_time(&type);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun get-decoded-time () ...)
 *   -> second, minute, hour, date, month, year, week, daylight-p, zone
 * (get-decoded-time) == (decode-universal-time (get-universal-time))
 */
static void function_get_decoded_time(Execute ptr)
{
	struct universal_time_struct u;

	get_decoded_time_common(ptr->local, &u);
	setvalues_control(ptr,
			u.second, u.minute, u.hour,
			u.date, u.month, u.year,
			u.week, u.daylight_p, u.zone, NULL);
}

static void type_get_decoded_time(addr *ret)
{
	addr args, values;

	GetTypeArgs(&args, Empty);
	GetTypeValues(&values, DecodeUniversalTime);
	type_compiled_heap(args, values, ret);
}

static void defun_get_decoded_time(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_GET_DECODED_TIME, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_get_decoded_time);
	SetFunctionCommon(symbol, pos);
	/* type */
	type_get_decoded_time(&type);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun apropos (string-designer) ...) -> (values) */
static void function_apropos(Execute ptr, addr var, addr opt)
{
	Return0(apropos_common(ptr, var, (opt == Unbound)? Nil: opt));
	setvalues_nil_control(ptr);
}

static void type_apropos(addr *ret)
{
	addr args, values;

	GetTypeTable(&args, StringDesigner);
	GetTypeTable(&values, PackageDesignerNull);
	typeargs_var1opt1(&args, args, values);
	GetTypeValues(&values, Empty);
	type_compiled_heap(args, values, ret);
}

static void defun_apropos(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_APROPOS, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_var1opt1(pos, p_defun_apropos);
	SetFunctionCommon(symbol, pos);
	/* type */
	type_apropos(&type);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun apropos-list (string-designer) ...) -> (values) */
static void function_apropos_list(Execute ptr, addr var, addr opt)
{
	Return0(apropos_list_common(ptr, var, (opt == Unbound)? Nil: opt, &var));
	setresult_control(ptr, var);
}

static void type_apropos_list(addr *ret)
{
	addr args, values;

	GetTypeTable(&args, StringDesigner);
	GetTypeTable(&values, PackageDesignerNull);
	typeargs_var1opt1(&args, args, values);
	GetTypeValues(&values, List);
	type_compiled_heap(args, values, ret);
}

static void defun_apropos_list(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_APROPOS_LIST, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_var1opt1(pos, p_defun_apropos_list);
	SetFunctionCommon(symbol, pos);
	/* type */
	type_apropos_list(&type);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun describe (object &optional stream) ...) -> (values) */
static void function_describe(Execute ptr, addr var, addr opt)
{
	Return0(describe_common(ptr, var, opt));
	setvalues_nil_control(ptr);
}

static void type_describe(addr *ret)
{
	addr args, values;

	GetTypeTable(&args, T);
	GetTypeTable(&values, StreamDesigner);
	typeargs_var1opt1(&args, args, values);
	GetTypeValues(&values, Empty);
	type_compiled_heap(args, values, ret);
}

static void defun_describe(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_DESCRIBE, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_var1opt1(pos, p_defun_describe);
	SetFunctionCommon(symbol, pos);
	/* type */
	type_describe(&type);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defmacro time (form) ...) -> result) */
static void function_time(Execute ptr, addr form, addr env)
{
	time_common(ptr, form, env, &form);
	setresult_control(ptr, form);
}

static void defmacro_time(void)
{
	addr symbol, pos, type;

	GetConst(COMMON_TIME, &symbol);
	compiled_macro_heap(&pos, symbol);
	setcompiled_macro(pos, p_defmacro_time);
	SetMacroCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, MacroFunction);
	settype_function(pos, type);
}


/* (defconstant internal-time-units-per-second [imlementation-dependency]) */
static void defconstant_internal_time_units_per_second(void)
{
	addr symbol, value;
	fixnum units;

	GetConst(COMMON_INTERNAL_TIME_UNITS_PER_SECOND, &symbol);
	get_internal_time_units_per_second(&units);
	fixnum_heap(&value, units);
	SetValueSymbol(symbol, value);
}


/* (defun get-internal-real-time () ...) -> (integer 0 *) */
static void function_get_internal_real_time(Execute ptr)
{
	addr pos;
	get_internal_real_time_common(ptr->local, &pos);
	setresult_control(ptr, pos);
}

static void defun_get_internal_real_time(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_GET_INTERNAL_REAL_TIME, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_get_internal_real_time);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, GetInternalRealTime);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun get-internal-run-time () ...) -> (integer 0 *) */
static void function_get_internal_run_time(Execute ptr)
{
	addr pos;
	get_internal_run_time_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_get_internal_run_time(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_GET_INTERNAL_RUN_TIME, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_get_internal_run_time);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, GetInternalRealTime);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun disassemble (extended-function-designer) ...) -> nil */
static void function_disassemble(Execute ptr, addr var)
{
	addr stream, check;

	standard_output_stream(ptr, &stream);
	if (symbolp(var)) {
		getfunction_local(ptr, var, &check);
		if (check == Unbound) {
			getmacro_symbol(var, &check);
			if (check == Unbound)
				fmte("Invalid argument ~S.", var);
		}
		var = check;
	}
	Return0(disassemble_common(ptr, stream, var));
	setresult_control(ptr, Nil);
}

static void type_disassemble(addr *ret)
{
	addr args, values;

	GetTypeTable(&args, FunctionDesigner);
	typeargs_var1(&args, args);
	GetTypeValues(&values, Null);
	type_compiled_heap(args, values, ret);
}

static void defun_disassemble(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_DISASSEMBLE, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_var1(pos, p_defun_disassemble);
	SetFunctionCommon(symbol, pos);
	/* type */
	type_disassemble(&type);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun lisp-implementation-type () ...) -> (or string null)  */
static void function_lisp_implementation_type(Execute ptr)
{
	addr pos;
	implementation_type_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_lisp_implementation_type(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_LISP_IMPLEMENTATION_TYPE, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_lisp_implementation_type);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, EnvInfo);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun lisp-implementation-version () ...) -> (or string null)  */
static void function_lisp_implementation_version(Execute ptr)
{
	addr pos;
	implementation_version_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_lisp_implementation_version(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_LISP_IMPLEMENTATION_VERSION, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_lisp_implementation_version);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, EnvInfo);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun short-site-name () ...) -> (or string null) */
static void function_short_site_name(Execute ptr)
{
	addr pos;
	short_site_name_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_short_site_name(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_SHORT_SITE_NAME, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_short_site_name);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, EnvInfo);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun long-site-name () ...) -> (or string null) */
static void function_long_site_name(Execute ptr)
{
	addr pos;
	long_site_name_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_long_site_name(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_LONG_SITE_NAME, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_long_site_name);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, EnvInfo);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun machine-instance() ...) -> (or string null)  */
static void function_machine_instance(Execute ptr)
{
	addr pos;
	machine_instance_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_machine_instance(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_MACHINE_INSTANCE, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_machine_instance);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, EnvInfo);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun machine-type() ...) -> (or string null)  */
static void function_machine_type(Execute ptr)
{
	addr pos;
	machine_type_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_machine_type(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_MACHINE_TYPE, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_machine_type);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, EnvInfo);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun machine-version() ...) -> (or string null)  */
static void function_machine_version(Execute ptr)
{
	addr pos;
	machine_version_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_machine_version(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_MACHINE_VERSION, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_machine_version);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, EnvInfo);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun software-type() ...) -> (or string null)  */
static void function_software_type(Execute ptr)
{
	addr pos;
	software_type_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_software_type(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_SOFTWARE_TYPE, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_software_type);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, EnvInfo);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun software-version() ...) -> (or string null)  */
static void function_software_version(Execute ptr)
{
	addr pos;
	software_version_common(&pos);
	setresult_control(ptr, pos);
}

static void defun_software_version(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_SOFTWARE_VERSION, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_empty(pos, p_defun_software_version);
	SetFunctionCommon(symbol, pos);
	/* type */
	GetTypeCompiled(&type, EnvInfo);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/* (defun user-homedir-pathname (&optinal host) ...) -> result
 *   host    (or string list (eql :unspecific)
 *   result  (or pathname null)
 */
static void function_user_homedir_pathname(Execute ptr, addr host)
{
	/* (declare (ignore host)) */
	user_homedir_pathname_common(ptr, &host);
	setresult_control(ptr, host);
}

static void type_user_homedir_pathname(addr *ret)
{
	addr args, values, type1, type2, type3;

	GetTypeTable(&type1, String);
	GetTypeTable(&type2, List);
	GetConst(KEYWORD_UNSPECIFIC, &type3);
	type_eql_heap(type3, &type3);
	type3or_heap(type1, type2, type3, &args);
	typeargs_opt1(&args, args);
	GetTypeValues(&values, PathnameNull);
	type_compiled_heap(args, values, ret);
}

static void defun_user_homedir_pathname(void)
{
	addr symbol, pos, type;

	/* function */
	GetConst(COMMON_USER_HOMEDIR_PATHNAME, &symbol);
	compiled_heap(&pos, symbol);
	setcompiled_opt1(pos, p_defun_user_homedir_pathname);
	SetFunctionCommon(symbol, pos);
	/* type */
	type_user_homedir_pathname(&type);
	settype_function(pos, type);
	settype_function_symbol(symbol, type);
}


/*
 *  function
 */
_g void init_common_environment(void)
{
	SetPointerCall(defun, var1opt1, decode_universal_time);
	SetPointerCall(defun, dynamic, encode_universal_time);
	SetPointerCall(defun, empty, get_universal_time);
	SetPointerCall(defun, empty, get_decoded_time);
	SetPointerCall(defun, var1opt1, apropos);
	SetPointerCall(defun, var1opt1, apropos_list);
	SetPointerCall(defun, var1opt1, describe);
	SetPointerCall(defmacro, macro, time);
	SetPointerCall(defun, empty, get_internal_real_time);
	SetPointerCall(defun, empty, get_internal_run_time);
	SetPointerCall(defun, var1, disassemble);
	SetPointerCall(defun, empty, lisp_implementation_type);
	SetPointerCall(defun, empty, lisp_implementation_version);
	SetPointerCall(defun, empty, short_site_name);
	SetPointerCall(defun, empty, long_site_name);
	SetPointerCall(defun, empty, machine_instance);
	SetPointerCall(defun, empty, machine_type);
	SetPointerCall(defun, empty, machine_version);
	SetPointerCall(defun, empty, software_type);
	SetPointerCall(defun, empty, software_version);
	SetPointerCall(defun, opt1, user_homedir_pathname);
}

_g void build_common_environment(void)
{
	defun_decode_universal_time();
	defun_encode_universal_time();
	defun_get_universal_time();
	defun_get_decoded_time();
	/*sleep*/
	defun_apropos();
	defun_apropos_list();
	defun_describe();
	/*trace*/
	/*untrace*/
	/*step*/
	defmacro_time();
	defconstant_internal_time_units_per_second();
	defun_get_internal_real_time();
	defun_get_internal_run_time();
	defun_disassemble();
	/*documentation*/
	/*(setf documentation)*/
	/*room*/
	/*ed*/
	/*inspect*/
	/*dribble*/
	defun_lisp_implementation_type();
	defun_lisp_implementation_version();
	defun_short_site_name();
	defun_long_site_name();
	defun_machine_instance();
	defun_machine_type();
	defun_machine_version();
	defun_software_type();
	defun_software_version();
	defun_user_homedir_pathname();
}

