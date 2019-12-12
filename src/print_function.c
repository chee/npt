#include "array_vector.h"
#include "build.h"
#include "code.h"
#include "control.h"
#include "constant.h"
#include "function.h"
#include "gc.h"
#include "pointer.h"
#include "print_function.h"
#include "print_pretty.h"
#include "print_write.h"
#include "sequence.h"
#include "stream.h"
#include "stream_pretty.h"
#include "symbol.h"
#include "unicode.h"

/*
 *  unwind-protect
 */
static void pprint_logical_block_close(Execute ptr)
{
	addr stream;

	getdata_control(ptr, &stream);
	Check(! pretty_stream_p(stream), "type error");
	close_pretty_stream(ptr, stream);
	setresult_control(ptr, Nil);
}


/*
 *  pprint-fill
 */
static void pprint_logical_block_type_form(Execute ptr, enum pprint_newline type)
{
	addr stream, pos;

	getdata_control(ptr, &stream);
	Check(! pretty_stream_p(stream), "type error");
	Return0(check_pretty_stream(ptr, stream));
	/* body */
	Return0(pprint_exit_common(ptr, stream));
	for (;;) {
		Return0(pprint_pop_common(ptr, stream, &pos));
		Return0(write_print(ptr, stream, pos));
		Return0(pprint_exit_common(ptr, stream));
		write_char_stream(stream, ' ');
		pprint_newline_common(ptr, type, stream);
	}
}

static void pprint_logical_block_type(Execute ptr, pointer type)
{
	int check;
	addr stream, control, code, gensym;

	/* stream */
	getdata_control(ptr, &stream);
	Check(! pretty_stream_p(stream), "type error");
	/* unwind-protect */
	push_finalize_control(ptr, &control);
	syscall_code(ptr->local, &code, p_pprint_logical_block_close, stream);
	setfinalize_control(ptr, control, code);
	/* code */
	gensym_pretty_stream(stream, &gensym);
	catch_syscall_code(&code, type, gensym, stream);
	check = callclang_funcall(ptr, &code, code, NULL);
	free_check_control(ptr, control, check);
}

static int pprint_type_common(Execute ptr,
		addr stream, addr list, int colon, pointer type)
{
	addr prefix, suffix, lambda;

	/* make-pprint-stream */
	prefix = suffix = Nil;
	if (colon) {
		strvect_char_heap(&prefix, "(");
		strvect_char_heap(&suffix, ")");
	}
	open_pretty_stream(ptr, &stream, stream, list, prefix, Nil, suffix);

	/* function */
	compiled_heap(&lambda, Nil);
	setcompiled_empty(lambda, type);
	SetDataFunction(lambda, stream);

	/* call */
	gchold_pushva_local(ptr->local, stream, lambda, NULL);
	return call_pretty_stream(ptr, stream, lambda);
}

static int pprint_list_common(Execute ptr, addr stream, addr list, pointer type)
{
	return pprint_type_common(ptr, stream, list, 1, type);
}

static void pprint_logical_block_fill_form(Execute ptr)
{
	pprint_logical_block_type_form(ptr, pprint_newline_fill);
}

static void pprint_logical_block_fill(Execute ptr)
{
	pprint_logical_block_type(ptr, p_pprint_logical_block_fill_form);
}

_g int pprint_fill_common(Execute ptr, addr stream, addr list, int colon)
{
	/* (defun pprint-fill (*standard-output* list &optional (colon t) atsign)
	 *   (declare (ignore atsign))
	 *   (pprint-logical-block
	 *     (nil list :prefix (if colon "(" "") :suffix (if colon ")" ""))
	 *     (pprint-exit-if-list-exhausted)
	 *     (loop (write (pprint-pop))
	 *           (pprint-exit-if-list-exhausted)
	 *           (write-char #\Space)
	 *           (pprint-newline :fill))))
	 */
	return pprint_type_common(ptr, stream, list, colon, p_pprint_logical_block_fill);
}


/*
 *  pprint-linaer
 */
static void pprint_logical_block_linear_form(Execute ptr)
{
	pprint_logical_block_type_form(ptr, pprint_newline_linear);
}

static void pprint_logical_block_linear(Execute ptr)
{
	pprint_logical_block_type(ptr, p_pprint_logical_block_linear_form);
}

_g int pprint_linear_common(Execute ptr, addr stream, addr list, int colon)
{
	/* (defun pprint-linear (*standard-output* list &optional (colon t) atsign)
	 *   (declare (ignore atsign))
	 *   (pprint-logical-block
	 *     (nil list :prefix (if colon "(" "") :suffix (if colon ")" ""))
	 *     (pprint-exit-if-list-exhausted)
	 *     (loop (write (pprint-pop))
	 *           (pprint-exit-if-list-exhausted)
	 *           (write-char #\Space)
	 *           (pprint-newline :linear))))
	 */
	return pprint_type_common(ptr, stream, list, colon, p_pprint_logical_block_linear);
}


/*
 *  pprint-tabular
 */
static void pprint_logical_block_tabular_form(Execute ptr)
{
	addr cons, stream, pos;
	fixnum colinc;

	getdata_control(ptr, &cons);
	GetCons(cons, &stream, &pos);
	Check(! pretty_stream_p(stream), "type error");
	GetFixnum(pos, &colinc);
	Return0(check_pretty_stream(ptr, stream));
	/* body */
	Return0(pprint_exit_common(ptr, stream));
	for (;;) {
		Return0(pprint_pop_common(ptr, stream, &pos));
		Return0(write_print(ptr, stream, pos));
		Return0(pprint_exit_common(ptr, stream));
		write_char_stream(stream, ' ');
		pprint_tab_section_relative(ptr, stream, 0, colinc);
		pprint_newline_common(ptr, pprint_newline_fill, stream);
	}
}

static void pprint_logical_block_tabular(Execute ptr)
{
	int check;
	addr cons, stream, control, code, gensym;

	/* stream */
	getdata_control(ptr, &cons);
	GetCar(cons, &stream);
	Check(! pretty_stream_p(stream), "type error");
	/* unwind-protect */
	push_finalize_control(ptr, &control);
	syscall_code(ptr->local, &code, p_pprint_logical_block_close, stream);
	setfinalize_control(ptr, control, code);
	/* code */
	gensym_pretty_stream(stream, &gensym);
	catch_syscall_code(&code, p_pprint_logical_block_tabular_form, gensym, cons);
	check = callclang_funcall(ptr, &code, code, NULL);
	free_check_control(ptr, control, check);
}

_g int pprint_tabular_common(Execute ptr,
		addr stream, addr list, int colon, fixnum size)
{
	/* (defun pprint-tabular
	 *   (*standard-output* list &optional (colon t) atsign (tabsize 16))
	 *   (declare (ignore atsign))
	 *   (pprint-logical-block
	 *     (nil list :prefix (if colon "(" "") :suffix (if colon ")" ""))
	 *     (pprint-exit-if-list-exhausted)
	 *     (loop (write (pprint-pop))
	 *           (pprint-exit-if-list-exhausted)
	 *           (write-char #\Space)
	 *           (pprint-tab :section-relative 0 tabsize)
	 *           (pprint-newline :fill))))
	 */
	addr prefix, suffix, lambda, cons;

	/* make-pprint-stream */
	prefix = suffix = Nil;
	if (colon) {
		strvect_char_heap(&prefix, "(");
		strvect_char_heap(&suffix, ")");
	}
	open_pretty_stream(ptr, &stream, stream, list, prefix, Nil, suffix);

	/* closure */
	fixnum_heap(&cons, size);
	cons_heap(&cons, stream, cons);

	/* function */
	compiled_heap(&lambda, Nil);
	setcompiled_empty(lambda, p_pprint_logical_block_tabular);
	SetDataFunction(lambda, cons);

	/* call */
	gchold_pushva_local(ptr->local, stream, lambda, NULL);
	return call_pretty_stream(ptr, stream, lambda);
}


/*
 *  dispatch-vector
 */
static void pprint_dispatch_vector2(Execute ptr)
{
	addr cons, stream, pos, vector;
	size_t size, i;

	getdata_control(ptr, &cons);
	GetCons(cons, &stream, &vector);
	Check(! pretty_stream_p(stream), "type error");
	Check(! vectorp_sequence(vector), "type error");
	Return0(check_pretty_stream(ptr, stream));
	/* body */
	size = length_sequence(vector, 1);
	Return0(size == 0);
	i = 0;
	for (;;) {
		Return0(pprint_pop_common(ptr, stream, &pos));
		getelt_sequence(NULL, vector, i, &pos);
		Return0(write_print(ptr, stream, pos));
		i++;
		Return0(size <= i);
		write_char_stream(stream, ' ');
		pprint_newline_common(ptr, pprint_newline_fill, stream);
	}
}

static void pprint_dispatch_vector1(Execute ptr)
{
	int check;
	addr cons, stream, control, code, gensym;

	/* stream */
	getdata_control(ptr, &cons);
	GetCar(cons, &stream);
	Check(! pretty_stream_p(stream), "type error");
	/* unwind-protect */
	push_finalize_control(ptr, &control);
	syscall_code(ptr->local, &code, p_pprint_logical_block_close, stream);
	setfinalize_control(ptr, control, code);
	/* code */
	gensym_pretty_stream(stream, &gensym);
	catch_syscall_code(&code, p_pprint_dispatch_vector2, gensym, cons);
	check = callclang_funcall(ptr, &code, code, NULL);
	free_check_control(ptr, control, check);
}

static void pprint_dispatch_vector(Execute ptr, addr stream, addr pos)
{
	/* (defun dispatch-vector (*standard-output* v)
	 *   (pprint-logical-block (nil nil :prefix "#(" :suffix ")")
	 *     (let ((end (length v)) (i 0))
	 *       (when (plusp end)
	 *         (loop (pprint-pop)
	 *               (write (aref v i))
	 *               (if (= (incf i) end) (return nil))
	 *               (write-char #\Space)
	 *               (pprint-newline :fill))))))
	 */
	addr prefix, suffix, lambda, cons;

	/* make-pprint-stream */
	strvect_char_heap(&prefix, "#(");
	strvect_char_heap(&suffix, ")");
	open_pretty_stream(ptr, &stream, stream, Nil, prefix, Nil, suffix);

	/* closure */
	cons_heap(&cons, stream, pos);

	/* function */
	compiled_heap(&lambda, Nil);
	setcompiled_empty(lambda, p_pprint_dispatch_vector1);
	SetDataFunction(lambda, cons);

	/* call */
	gchold_pushva_local(ptr->local, stream, lambda, NULL);
	(void)call_pretty_stream(ptr, stream, lambda);
}


/*
 *  dispatch-call
 */
static void pprint_dispatch_call2(Execute ptr)
{
	addr stream, pos;

	getdata_control(ptr, &stream);
	Check(! pretty_stream_p(stream), "type error");
	Return0(check_pretty_stream(ptr, stream));
	/* body */
	Return0(pprint_pop_common(ptr, stream, &pos));
	Return0(write_print(ptr, stream, pos));
	Return0(pprint_exit_common(ptr, stream));
	write_char_stream(stream, ' ');
	pprint_newline_common(ptr, pprint_newline_miser, stream);
	pprint_indent_common(ptr, 0, 0, stream);
	for (;;) {
		Return0(pprint_pop_common(ptr, stream, &pos));
		Return0(write_print(ptr, stream, pos));
		Return0(pprint_exit_common(ptr, stream));
		write_char_stream(stream, ' ');
		pprint_newline_common(ptr, pprint_newline_linear, stream);
	}
}

static void pprint_dispatch_call1(Execute ptr)
{
	pprint_logical_block_type(ptr, p_pprint_dispatch_call2);
}

static void pprint_dispatch_call(Execute ptr, addr stream, addr list)
{
	/* (defun dispatch-call (*standard-outupt* list)
	 *   (pprint-logical-block (nil list :prefix "(" :suffix ")")
	 *     ;; name
	 *     (write (pprint-pop))
	 *     (pprint-exit-if-list-exhausted)
	 *     (write-char #\Space)
	 *     (pprint-newline :miser)
	 *     (pprint-indent :current 0)
	 *     ;; args
	 *     (loop (write (pprint-pop))
	 *           (pprint-exit-if-list-exhausted)
	 *           (write-char #\Space)
	 *           (pprint-newline :linear))))
	 */
	pprint_list_common(ptr, stream, list, p_pprint_dispatch_call1);
}


/*
 *  dispatch-defun
 */
static void pprint_dispatch_defun6(Execute ptr)
{
	addr stream, pos;

	getdata_control(ptr, &stream);
	Check(! pretty_stream_p(stream), "type error");
	Return0(check_pretty_stream(ptr, stream));
	/* body */
	Return0(pprint_exit_common(ptr, stream));
	for (;;) {
		Return0(pprint_pop_common(ptr, stream, &pos));
		Return0(write_print(ptr, stream, pos));
		Return0(pprint_exit_common(ptr, stream));
		write_char_stream(stream, ' ');
		pprint_newline_common(ptr, pprint_newline_linear, stream);
	}
}

static void pprint_dispatch_defun5(Execute ptr)
{
	pprint_logical_block_type(ptr, p_pprint_dispatch_defun6);
}

static void pprint_dispatch_defun4(Execute ptr)
{
	addr stream, pos;

	getdata_control(ptr, &stream);
	Check(! pretty_stream_p(stream), "type error");
	Return0(check_pretty_stream(ptr, stream));
	/* body */
	Return0(pprint_exit_common(ptr, stream));
	for (;;) {
		Return0(pprint_pop_common(ptr, stream, &pos));
		Return0(pprint_list_common(ptr, stream, pos, p_pprint_dispatch_defun5));
		Return0(pprint_exit_common(ptr, stream));
		write_char_stream(stream, ' ');
		pprint_newline_common(ptr, pprint_newline_fill, stream);
	}
}

static void pprint_dispatch_defun3(Execute ptr)
{
	pprint_logical_block_type(ptr, p_pprint_dispatch_defun4);
}

static void pprint_dispatch_defun2(Execute ptr)
{
	addr stream, pos;

	getdata_control(ptr, &stream);
	Check(! pretty_stream_p(stream), "type error");
	Return0(check_pretty_stream(ptr, stream));
	/* defun */
	Return0(pprint_pop_common(ptr, stream, &pos));
	Return0(write_print(ptr, stream, pos));
	Return0(pprint_exit_common(ptr, stream));
	write_char_stream(stream, ' ');
	pprint_newline_common(ptr, pprint_newline_miser, stream);
	/* name */
	Return0(pprint_pop_common(ptr, stream, &pos));
	Return0(write_print(ptr, stream, pos));
	Return0(pprint_exit_common(ptr, stream));
	write_char_stream(stream, ' ');
	pprint_newline_common(ptr, pprint_newline_miser, stream);
	/* args */
	Return0(pprint_pop_common(ptr, stream, &pos));
	Return0(pprint_list_common(ptr, stream, pos, p_pprint_dispatch_defun3));
	/* body */
	pprint_indent_common(ptr, 1, 1, stream);
	for (;;) {
		Return0(pprint_exit_common(ptr, stream));
		write_char_stream(stream, ' ');
		pprint_newline_common(ptr, pprint_newline_linear, stream);
		Return0(pprint_pop_common(ptr, stream, &pos));
		Return0(write_print(ptr, stream, pos));
	}
}

static void pprint_dispatch_defun1(Execute ptr)
{
	pprint_logical_block_type(ptr, p_pprint_dispatch_defun2);
}

static void pprint_dispatch_defun(Execute ptr, addr stream, addr list)
{
	/* (defun dispatch-defun (*standard-output* list)
	 *   (pprint-logical-block (nil list :prefix "(" :suffix ")")
	 *     ;; defun
	 *     (write (pprint-pop))
	 *     (pprint-exit-if-list-exhausted)
	 *     (write-char #\Space)
	 *     (pprint-newline :miser)
	 *     ;; name
	 *     (write (pprint-pop))
	 *     (pprint-exit-if-list-exhausted)
	 *     (write-char #\Space)
	 *     (pprint-newline :miser)
	 *     ;; args
	 *     (pprint-logical-block (nil (pprint-pop) :prefix "(" :suffix ")")
	 *       (pprint-exit-if-list-exhausted)
	 *       (loop (pprint-logical-block (nil (pprint-pop) :prefix "(" :suffix ")")
	 *               (pprint-exit-if-list-exhausted)
	 *               (loop (write (pprint-pop))
	 *                     (pprint-exit-if-list-exhausted)
	 *                     (write-char #\Space)
	 *                     (pprint-newline :linear)))
	 *             (pprint-exit-if-list-exhausted)
	 *             (write-char #\Space)
	 *             (pprint-newline :fill)))
	 *     ;; body
	 *     (pprint-indent :block 1)
	 *     (loop (pprint-exit-if-list-exhausted)
	 *           (write-char #\Space)
	 *           (pprint-newline :linear)
	 *           (write (pprint-pop)))))
	 */
	pprint_list_common(ptr, stream, list, p_pprint_dispatch_defun1);
}


/*
 *  dispatch-let
 */
static void pprint_dispatch_let2(Execute ptr)
{
	addr stream, pos;

	getdata_control(ptr, &stream);
	Check(! pretty_stream_p(stream), "type error");
	Return0(check_pretty_stream(ptr, stream));
	/* let */
	Return0(pprint_pop_common(ptr, stream, &pos));
	Return0(write_print(ptr, stream, pos));
	Return0(pprint_exit_common(ptr, stream));
	write_char_stream(stream, ' ');
	pprint_newline_common(ptr, pprint_newline_miser, stream);
	/* args */
	Return0(pprint_pop_common(ptr, stream, &pos));
	Return0(pprint_list_common(ptr, stream, pos, p_pprint_dispatch_defun3));
	/* body */
	pprint_indent_common(ptr, 1, 1, stream);
	for (;;) {
		Return0(pprint_exit_common(ptr, stream));
		write_char_stream(stream, ' ');
		pprint_newline_common(ptr, pprint_newline_linear, stream);
		Return0(pprint_pop_common(ptr, stream, &pos));
		Return0(write_print(ptr, stream, pos));
	}
}

static void pprint_dispatch_let1(Execute ptr)
{
	pprint_logical_block_type(ptr, p_pprint_dispatch_let2);
}

static void pprint_dispatch_let(Execute ptr, addr stream, addr list)
{
	/* (defun dispatch-let (*standard-output* list)
	 *   (pprint-logical-block (nil list :prefix "(" :suffix ")")
	 *     ;; let
	 *     (write (pprint-pop))
	 *     (pprint-exit-if-list-exhausted)
	 *     (write-char #\Space)
	 *     (pprint-newline :miser)
	 *     ;; args
	 *     (pprint-logical-block (nil (pprint-pop) :prefix "(" :suffix ")")
	 *       (pprint-exit-if-list-exhausted)
	 *       (loop (pprint-logical-block (nil (pprint-pop) :prefix "(" :suffix ")")
	 *               (pprint-exit-if-list-exhausted)
	 *               (loop (write (pprint-pop))
	 *                     (pprint-exit-if-list-exhausted)
	 *                     (write-char #\Space)
	 *                     (pprint-newline :linear)))
	 *             (pprint-exit-if-list-exhausted)
	 *             (write-char #\Space)
	 *             (pprint-newline :fill)))
	 *     ;; body
	 *     (pprint-indent :block 1)
	 *     (loop (pprint-exit-if-list-exhausted)
	 *           (write-char #\Space)
	 *           (pprint-newline :linear)
	 *           (write (pprint-pop)))))
	 */
	pprint_list_common(ptr, stream, list, p_pprint_dispatch_let1);
}


/*
 *  initialize
 */
_g void init_print_function(void)
{
	SetPointerType(empty, pprint_logical_block_close);
	/* pprint-fill */
	SetPointerType(empty, pprint_logical_block_fill_form);
	SetPointerType(empty, pprint_logical_block_fill);
	/* pprint-linear */
	SetPointerType(empty, pprint_logical_block_linear_form);
	SetPointerType(empty, pprint_logical_block_linear);
	/* pprint-tabular */
	SetPointerType(empty, pprint_logical_block_tabular_form);
	SetPointerType(empty, pprint_logical_block_tabular);
	/* dispatch-vector */
	SetPointerType(empty, pprint_dispatch_vector2);
	SetPointerType(empty, pprint_dispatch_vector1);
	SetPointerType(var2, pprint_dispatch_vector);
	/* dispatch-call */
	SetPointerType(empty, pprint_dispatch_call2);
	SetPointerType(empty, pprint_dispatch_call1);
	SetPointerType(var2, pprint_dispatch_call);
	/* defun */
	SetPointerType(empty, pprint_dispatch_defun6);
	SetPointerType(empty, pprint_dispatch_defun5);
	SetPointerType(empty, pprint_dispatch_defun4);
	SetPointerType(empty, pprint_dispatch_defun3);
	SetPointerType(empty, pprint_dispatch_defun2);
	SetPointerType(empty, pprint_dispatch_defun1);
	SetPointerType(var2, pprint_dispatch_defun);
	/* let */
	SetPointerType(empty, pprint_dispatch_let2);
	SetPointerType(empty, pprint_dispatch_let1);
	SetPointerType(var2, pprint_dispatch_let);
}

