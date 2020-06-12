#ifndef __POINTER_TABLE_HEADER__
#define __POINTER_TABLE_HEADER__

#include "define.h"

enum pointer_index {
	p_empty,
	p_user,
#ifdef LISP_DEGRADE
	/* debug */
	p_debug1,
	p_debug2,
	p_debug3,
#endif
	/* code_function */
	p_nop_code,
	p_execute_control_set_code,
	p_execute_control_push_code,
	p_execute_switch_set_code,
	p_execute_switch_push_code,

	p_set_code,
	p_push_code,
	p_push_result_code,
	p_push_values_code,
	p_nil_set_code,
	p_nil_push_code,
	p_t_set_code,
	p_t_push_code,

	p_lexical_code,
	p_lexical_set_code,
	p_lexical_push_code,
	p_lexical_rem_code,
	p_special_set_code,
	p_special_push_code,
	p_special_rem_code,

	p_declaim_special_code,
	p_declaim_type_value_code,
	p_declaim_type_function_code,
	p_declaim_inline_code,
	p_declaim_notinline_code,
	p_declaim_compilation_code,
	p_declaim_debug_code,
	p_declaim_safety_code,
	p_declaim_space_code,
	p_declaim_speed_code,
	p_declaim_declaration_code,

	p_type_result_code,
	p_type_lexical_code,
	p_type_special_code,
	p_type_global_code,
	p_type_function_code,
	p_type_setf_code,
	p_let_lexical_code,
	p_let_special_code,
	p_leta_special_code,

	p_setq_lexical_code,
	p_setq_special_code,
	p_setq_global_code,

	p_function_set_code,
	p_function_push_code,
	p_setf_set_code,
	p_setf_push_code,

	p_defmacro_code,
	p_deftype_code,
	p_define_compiler_macro_code,
	p_define_symbol_macro_code,
	p_defun_code,

	p_call_code,
	p_call_result_code,
	p_call_type_code,
	p_call_function_code,
	p_call_setf_code,
	p_call_lexical_code,

	p_values_nil_code,
	p_values_set_code,
	p_the_set_code,
	p_the_push_code,

	p_if_unbound_code,
	p_if_nil_code,
	p_if_t_code,
	p_goto_code,
	p_go_code,
	p_return_from_code,
	p_catch_code,
	p_throw_operator_code,
	p_taginfo_code,
	p_blockinfo_code,
	p_unwind_protect_code,

	p_handler_bind_code,
	p_handler_case_code,
	p_restart_bind_code,
	p_restart_case_code,

	p_prog1_set_code,
	p_prog1_push_code,
	p_funcall_code,
	p_nth_value_code,
	p_progv_code,

	/* code_lambda */
	p_pop_code,
	p_pop_unbound_code,
	p_getf_code,
	p_rest_code,
	p_allow_other_keys_code,
	p_rest_null_code,
	p_whole_code,

	p_lambda_code,
	p_lambda_name_code,
	p_lambda_type_code,
	p_lambda_doc_code,
	p_lambda_form_code,
	p_lambda_defun_code,
	p_lambda_closure_code,
	p_lambda_allocate_code,
	p_lambda_lexical_code,
	p_macro_code,
	p_macro_special_code,
	p_macro_env_code,
	p_macro_whole_code,

	p_bind1_type_code,
	p_bind1_special_code,
	p_bind1_lexical_code,
	p_bind2_type_code,
	p_bind2_special_code,
	p_bind2_lexical_code,

	p_load_time_value_alloc_code,
	p_load_time_value_value_code,
	p_load_time_value_set_code,
	p_load_time_value_push_code,

	/* optimize */
	p_optcode_result_type,
	p_optcode_car0_set,
	p_optcode_car0_push,
	p_optcode_car1_set,
	p_optcode_car1_push,
	p_optcode_cdr0_set,
	p_optcode_cdr0_push,
	p_optcode_cdr1_set,
	p_optcode_cdr1_push,
	p_optcode_cons,

	/* end of code */
	p_size_code,

	/* common-eval */
	p_defmacro_lambda,
	p_defun_compile,
	p_defun_eval,
	p_defun_compiler_macro_function,
	p_defun_setf_compiler_macro_function,
	p_defmacro_define_compiler_macro,
	p_defmacro_defmacro,
	p_defun_macro_function,
	p_defun_setf_macro_function,
	p_defun_macroexpand,
	p_defun_macroexpand_1,
	p_defmacro_define_symbol_macro,
	p_defun_proclaim,
	p_defmacro_declaim,
	p_defun_special_operator_p,
	p_defun_constantp,
	/* common-types */
	p_defun_coerce,
	p_defmacro_deftype,
	p_defun_subtypep,
	p_defun_type_of,
	p_defun_typep,
	p_defun_type_error_datum,
	p_defun_type_error_expected_type,
	/* common-data */
	p_defun_apply,
	p_defmacro_defun,
	p_defun_fdefinition,
	p_defun_setf_fdefinition,
	p_defun_fboundp,
	p_defun_fmakunbound,
	p_defun_funcall,
	p_defun_function_lambda_expression,
	p_defun_functionp,
	p_defun_compiled_function_p,
	p_defmacro_defconstant,
	p_defmacro_defparameter,
	p_defmacro_defvar,
	p_defmacro_destructuring_bind,
	p_defmacro_psetq,
	p_defmacro_return,
	p_defun_not,
	p_defun_eq,
	p_defun_eql,
	p_defun_equal,
	p_defun_equalp,
	p_defun_identity,
	p_defun_lambda_complement,
	p_defun_complement,
	p_defun_lambda_constantly,
	p_defun_constantly,
	p_defun_every,
	p_defun_some,
	p_defun_notany,
	p_defun_notevery,
	p_defmacro_and,
	p_defmacro_cond,
	p_defmacro_or,
	p_defmacro_when,
	p_defmacro_unless,
	p_defmacro_case,
	p_defmacro_ecase,
	p_defmacro_ccase,
	p_defmacro_typecase,
	p_defmacro_etypecase,
	p_defmacro_ctypecase,
	p_defmacro_multiple_value_bind,
	p_defmacro_multiple_value_list,
	p_defmacro_multiple_value_setq,
	p_defun_values,
	p_defmacro_setf_values,
	p_defun_values_list,
	p_defmacro_nth_value,
	p_defmacro_prog,
	p_defmacro_proga,
	p_defmacro_prog1,
	p_defmacro_prog2,
	p_defmacro_define_modify_macro,
	p_defmacro_defsetf,
	p_defmacro_define_setf_expander,
	p_defun_get_setf_expansion,
	p_defmacro_setf,
	p_defmacro_psetf,
	p_defmacro_shiftf,
	p_defmacro_rotatef,
	/* common-iteration */
	p_defmacro_do,
	p_defmacro_doa,
	p_defmacro_dotimes,
	p_defmacro_dolist,
	p_defmacro_loop,
	/* common-objects */
	p_defun_ensure_generic_function,
	p_defun_slot_boundp,
	p_defun_slot_exists_p,
	p_defun_slot_makunbound,
	p_defun_slot_value,
	p_defun_setf_slot_value,
	p_defmacro_with_accessors,
	p_defmacro_with_slots,
	p_defmacro_defclass,
	p_defmacro_defgeneric,
	p_defmacro_defmethod,
	p_defun_find_class,
	p_defun_setf_find_class,
	p_defmacro_define_method_combination,
	p_defun_class_of,
	p_defun_unbound_slot_instance,
	/* Metaobject Protocol class */
	p_defun_referenced_class,
	p_defun_ensure_class,
	p_method_ensure_class_using_class_null,
	p_method_ensure_class_using_class_class,
	p_method_allocate_instance_stdclass,
	p_method_initialize_instance_stdobject,
	p_method_reinitialize_instance_stdobject,
	p_method_shared_initialize_stdobject,
	p_method_make_instance_symbol,
	p_method_make_instance_stdclass,
	p_method_make_instance_structure,
	p_method_make_instances_obsolete_symbol,
	p_method_make_instances_obsolete_stdclass,
	p_method_make_load_form_object,
	p_method_make_load_form_class,
	p_defun_make_load_form_saving_slots,
	p_method_slot_missing,
	p_method_slot_unbound,
	p_method_update_instance_for_different_class,
	p_method_update_instance_for_redefined_class,
	p_method_slot_boundp_using_class,
	p_method_slot_exists_p_using_class,
	p_method_slot_makunbound_using_class,
	p_method_slot_value_using_class,
	p_method_setf_slot_value_using_class,
	p_method_change_class_stdclass,
	p_method_change_class_symbol,
	/* Metaobject Protocol reader */
	p_method_class_name,
	p_method_setf_class_name,
	p_make_slot_definition_call,
	p_method_class_slots,
	p_method_class_direct_slots,
	p_method_class_default_initargs,
	p_method_class_direct_default_initargs,
	p_method_class_precedence_list,
	p_method_class_direct_superclasses,
	p_method_class_direct_subclasses,
	p_method_class_finalized_p,
	p_method_class_prototype,
	p_method_slot_definition_name,
	p_method_slot_definition_type,
	p_method_slot_definition_allocation,
	p_method_slot_definition_initargs,
	p_method_slot_definition_initform,
	p_method_slot_definition_initfunction,
	p_method_generic_function_name,
	p_method_setf_generic_function_name,
	p_method_generic_function_methods,
	p_method_generic_function_lambda_list,
	p_method_generic_function_argument_precedence_order,
	p_method_generic_function_declarations,
	p_method_generic_function_method_class,
	p_method_generic_function_method_combination,
	p_method_method_function,
	p_method_method_generic_function,
	p_method_method_lambda_list,
	p_method_method_specializers,
	p_method_method_qualifiers,
	p_method_accessor_method_slot_definition,

	/* Metaobject Protocol generic */
	p_method_ensure_generic_function_class,
	p_method_ensure_generic_function_null,
	p_function_ensure_method,
	p_method_function_keywords,
	p_defun_flet_method_p,
	p_defun_flet_next_method,
	p_defun_method_combination_instance,
	p_defun_ensure_method_combination_short,
	p_defun_ensure_method_combination_long,
	p_defun_qualifiers_elt,
	p_defun_combination_binding,
	p_defun_macro_make_method,
	p_defun_macro_call_method,
	p_defun_macro_method_lambda,
	p_method_compute_applicable_methods_std,
	p_method_find_method_std,
	p_method_add_method_std,
	p_method_remove_method_std,
	/* Metaobject Protocol protocols */
	p_method_make_method_lambda_std,
	/* common-structures */
	p_defmacro_defstruct,
	p_defun_copy_structure,
	/* common-condition */
	p_defun_cell_error_name,
	p_defmacro_assert,
	p_defun_error,
	p_defun_cerror,
	p_defun_cerror_continue,
	p_defmacro_check_type,
	p_defun_invalid_method_error,
	p_defun_method_combination_error,
	p_defun_signal,
	p_defun_simple_condition_format_control,
	p_defun_simple_condition_format_arguments,
	p_defun_warn,
	p_defun_warn_restart,
	p_defun_invoke_debugger,
	p_defun_break,
	p_defun_break_continue,
	p_defmacro_handler_bind,
	p_defmacro_handler_case,
	p_defmacro_ignore_errors,
	p_defmacro_define_condition,
	p_defun_make_condition,
	p_defun_compute_restarts,
	p_defun_find_restart,
	p_defun_invoke_restart,
	p_defun_invoke_restart_interactively,
	p_defmacro_restart_bind,
	p_defmacro_restart_case,
	p_defun_restart_name,
	p_defmacro_with_condition_restarts,
	p_defmacro_with_simple_restart,
	p_defun_abort,
	p_defun_continue,
	p_defun_muffle_warning,
	p_defun_store_value,
	p_defun_use_value,
	/* common-symbols */
	p_defun_symbolp,
	p_defun_keywordp,
	p_defun_make_symbol,
	p_defun_copy_symbol,
	p_defun_gensym,
	p_defun_gentemp,
	p_defun_symbol_function,
	p_defun_setf_symbol_function,
	p_defun_symbol_value,
	p_defun_setf_symbol_value,
	p_defun_symbol_plist,
	p_defun_setf_symbol_plist,
	p_defun_symbol_name,
	p_defun_symbol_package,
	p_defun_get,
	p_defun_setf_get,
	p_defun_remprop,
	p_defun_boundp,
	p_defun_makunbound,
	p_defun_set,
	/* common-packages */
	p_defun_export,
	p_defun_find_symbol,
	p_defun_find_package,
	p_defun_find_all_symbols,
	p_defun_import,
	p_defun_list_all_packages,
	p_defun_rename_package,
	p_defun_shadow,
	p_defun_shadowing_import,
	p_defun_delete_package,
	p_defun_make_package,
	p_defmacro_with_package_iterator,
	p_defun_unexport,
	p_defun_unintern,
	p_defmacro_in_package,
	p_defun_unuse_package,
	p_defun_use_package,
	p_defmacro_defpackage,
	p_defmacro_do_symbols,
	p_defmacro_do_external_symbols,
	p_defmacro_do_all_symbols,
	p_defun_intern,
	p_defun_package_name,
	p_defun_package_nicknames,
	p_defun_package_shadowing_symbols,
	p_defun_package_use_list,
	p_defun_package_used_by_list,
	p_defun_packagep,
	p_defun_package_error_package,
	/* common-numbers */
	p_defun_number_equal,
	p_defun_number_not_equal,
	p_defun_number_less,
	p_defun_number_greater,
	p_defun_number_less_equal,
	p_defun_number_greater_equal,
	p_defun_max,
	p_defun_min,
	p_defun_minusp,
	p_defun_plusp,
	p_defun_zerop,
	p_defun_floor,
	p_defun_ffloor,
	p_defun_ceiling,
	p_defun_fceiling,
	p_defun_truncate,
	p_defun_ftruncate,
	p_defun_round,
	p_defun_fround,
	p_defun_cis,
	p_defun_sin,
	p_defun_cos,
	p_defun_tan,
	p_defun_sinh,
	p_defun_cosh,
	p_defun_tanh,
	p_defun_asin,
	p_defun_acos,
	p_defun_atan,
	p_defun_asinh,
	p_defun_acosh,
	p_defun_atanh,
	p_defun_exp,
	p_defun_expt,
	p_defun_plus,
	p_defun_minus,
	p_defun_asterisk,
	p_defun_slash,
	p_defun_oneplus,
	p_defun_oneminus,
	p_defun_abs,
	p_defun_evenp,
	p_defun_oddp,
	p_defun_gcd,
	p_defun_lcm,
	p_defmacro_incf,
	p_defmacro_decf,
	p_defun_log,
	p_defun_mod,
	p_defun_rem,
	p_defun_signum,
	p_defun_sqrt,
	p_defun_isqrt,
	p_defun_make_random_state,
	p_defun_random,
	p_defun_random_state_p,
	p_defun_numberp,
	p_defun_complex,
	p_defun_complexp,
	p_defun_conjugate,
	p_defun_phase,
	p_defun_realpart,
	p_defun_imagpart,
	p_defun_upgraded_complex_part_type,
	p_defun_realp,
	p_defun_numerator,
	p_defun_denominator,
	p_defun_rational,
	p_defun_rationalize,
	p_defun_rationalp,
	p_defun_ash,
	p_defun_integer_length,
	p_defun_integerp,
	p_defun_parse_integer,
	p_defun_boole,
	p_defun_logand,
	p_defun_logandc1,
	p_defun_logandc2,
	p_defun_logeqv,
	p_defun_logior,
	p_defun_lognand,
	p_defun_lognor,
	p_defun_lognot,
	p_defun_logorc1,
	p_defun_logorc2,
	p_defun_logxor,
	p_defun_logbitp,
	p_defun_logcount,
	p_defun_logtest,
	p_defun_byte,
	p_defun_byte_size,
	p_defun_byte_position,
	p_defun_deposit_field,
	p_defun_dpb,
	p_defun_ldb,
	p_defmacro_setf_ldb,
	p_defun_ldb_test,
	p_defun_mask_field,
	p_defmacro_setf_mask_field,
	p_defun_decode_float,
	p_defun_scale_float,
	p_defun_float_radix,
	p_defun_float_sign,
	p_defun_float_digits,
	p_defun_float_precision,
	p_defun_integer_decode_float,
	p_defun_float,
	p_defun_floatp,
	p_defun_arithmetic_error_operands,
	p_defun_arithmetic_error_operation,
	/* common-characters */
	p_defun_char_eql,
	p_defun_char_less,
	p_defun_char_greater,
	p_defun_char_less_equal,
	p_defun_char_greater_equal,
	p_defun_char_equal,
	p_defun_char_lessp,
	p_defun_char_greaterp,
	p_defun_char_not_lessp,
	p_defun_char_not_greaterp,
	p_defun_char_not_eql,
	p_defun_char_not_equal,
	p_defun_character,
	p_defun_characterp,
	p_defun_alpha_char_p,
	p_defun_alphanumericp,
	p_defun_digit_char,
	p_defun_digit_char_p,
	p_defun_graphic_char_p,
	p_defun_standard_char_p,
	p_defun_char_upcase,
	p_defun_char_downcase,
	p_defun_upper_case_p,
	p_defun_lower_case_p,
	p_defun_both_case_p,
	p_defun_char_code,
	p_defun_code_char,
	p_defun_char_name,
	p_defun_name_char,
	/* common-conses */
	p_defun_car,
	p_defun_cdr,
	p_defun_caar,
	p_defun_cadr,
	p_defun_cdar,
	p_defun_cddr,
	p_defun_caaar,
	p_defun_caadr,
	p_defun_cadar,
	p_defun_caddr,
	p_defun_cdaar,
	p_defun_cdadr,
	p_defun_cddar,
	p_defun_cdddr,
	p_defun_caaaar,
	p_defun_caaadr,
	p_defun_caadar,
	p_defun_caaddr,
	p_defun_cadaar,
	p_defun_cadadr,
	p_defun_caddar,
	p_defun_cadddr,
	p_defun_cdaaar,
	p_defun_cdaadr,
	p_defun_cdadar,
	p_defun_cdaddr,
	p_defun_cddaar,
	p_defun_cddadr,
	p_defun_cdddar,
	p_defun_cddddr,
	p_defun_fifth,
	p_defun_sixth,
	p_defun_seventh,
	p_defun_eighth,
	p_defun_ninth,
	p_defun_tenth,

	p_defun_setf_car,
	p_defun_setf_cdr,
	p_defun_setf_caar,
	p_defun_setf_cadr,
	p_defun_setf_cdar,
	p_defun_setf_cddr,
	p_defun_setf_caaar,
	p_defun_setf_caadr,
	p_defun_setf_cadar,
	p_defun_setf_caddr,
	p_defun_setf_cdaar,
	p_defun_setf_cdadr,
	p_defun_setf_cddar,
	p_defun_setf_cdddr,
	p_defun_setf_caaaar,
	p_defun_setf_caaadr,
	p_defun_setf_caadar,
	p_defun_setf_caaddr,
	p_defun_setf_cadaar,
	p_defun_setf_cadadr,
	p_defun_setf_caddar,
	p_defun_setf_cadddr,
	p_defun_setf_cdaaar,
	p_defun_setf_cdaadr,
	p_defun_setf_cdadar,
	p_defun_setf_cdaddr,
	p_defun_setf_cddaar,
	p_defun_setf_cddadr,
	p_defun_setf_cdddar,
	p_defun_setf_cddddr,
	p_defun_setf_fifth,
	p_defun_setf_sixth,
	p_defun_setf_seventh,
	p_defun_setf_eighth,
	p_defun_setf_ninth,
	p_defun_setf_tenth,

	p_defun_cons,
	p_defun_consp,
	p_defun_atom,
	p_defun_rplaca,
	p_defun_rplacd,
	p_defun_copy_list,
	p_defun_copy_tree,
	p_defun_sublis,
	p_defun_nsublis,
	p_defun_subst,
	p_defun_nsubst,
	p_defun_subst_if,
	p_defun_nsubst_if,
	p_defun_subst_if_not,
	p_defun_nsubst_if_not,
	p_defun_tree_equal,
	p_defun_list,
	p_defun_lista,
	p_defun_list_length,
	p_defun_listp,
	p_defun_make_list,
	p_defmacro_push,
	p_defmacro_pop,
	p_defun_nth,
	p_defun_setf_nth,
	p_defun_nthcdr,
	p_defun_member,
	p_defun_member_if,
	p_defun_member_if_not,
	p_defun_mapc,
	p_defun_mapcar,
	p_defun_mapcan,
	p_defun_mapl,
	p_defun_maplist,
	p_defun_mapcon,
	p_defun_endp,
	p_defun_null,
	p_defun_nconc,
	p_defun_append,
	p_defun_revappend,
	p_defun_nreconc,
	p_defun_butlast,
	p_defun_nbutlast,
	p_defun_last,
	p_defun_ldiff,
	p_defun_tailp,
	p_defun_acons,
	p_defun_assoc,
	p_defun_assoc_if,
	p_defun_assoc_if_not,
	p_defun_copy_alist,
	p_defun_pairlis,
	p_defun_rassoc,
	p_defun_rassoc_if,
	p_defun_rassoc_if_not,
	p_defun_get_properties,
	p_defun_getf,
	p_defmacro_setf_getf,
	p_defmacro_remf,
	p_defun_intersection,
	p_defun_nintersection,
	p_defun_adjoin,
	p_defmacro_pushnew,
	p_defun_set_difference,
	p_defun_nset_difference,
	p_defun_set_exclusive_or,
	p_defun_nset_exclusive_or,
	p_defun_subsetp,
	p_defun_union,
	p_defun_nunion,
	/* common-array */
	p_defun_make_array,
	p_defun_adjust_array,
	p_defun_adjustable_array_p,
	p_defun_aref,
	p_defun_setf_aref,
	p_defun_array_dimension,
	p_defun_array_dimensions,
	p_defun_array_element_type,
	p_defun_array_has_fill_pointer_p,
	p_defun_array_displacement,
	p_defun_array_in_bounds_p,
	p_defun_array_rank,
	p_defun_array_row_major_index,
	p_defun_array_total_size,
	p_defun_arrayp,
	p_defun_fill_pointer,
	p_defun_setf_fill_pointer,
	p_defun_row_major_aref,
	p_defun_setf_row_major_aref,
	p_defun_upgraded_array_element_type,
	p_defun_simple_vector_p,
	p_defun_svref,
	p_defun_setf_svref,
	p_defun_vector,
	p_defun_vector_pop,
	p_defun_vector_push,
	p_defun_vector_push_extend,
	p_defun_vectorp,
	p_defun_bit,
	p_defun_setf_bit,
	p_defun_bit_vector_p,
	p_defun_simple_bit_vector_p,
	p_defun_bit_and,
	p_defun_bit_andc1,
	p_defun_bit_andc2,
	p_defun_bit_eqv,
	p_defun_bit_ior,
	p_defun_bit_nand,
	p_defun_bit_nor,
	p_defun_bit_orc1,
	p_defun_bit_orc2,
	p_defun_bit_not,
	/* common-strings */
	p_defun_stringp,
	p_defun_simple_string_p,
	p_defun_char,
	p_defun_schar,
	p_defun_setf_char,
	p_defun_string,
	p_defun_string_upcase,
	p_defun_string_downcase,
	p_defun_string_capitalize,
	p_defun_nstring_upcase,
	p_defun_nstring_downcase,
	p_defun_nstring_capitalize,
	p_defun_string_trim,
	p_defun_string_left_trim,
	p_defun_string_right_trim,
	p_defun_string_eql,
	p_defun_string_not_eql,
	p_defun_string_less,
	p_defun_string_greater,
	p_defun_string_less_equal,
	p_defun_string_greater_equal,
	p_defun_string_equal,
	p_defun_string_not_equal,
	p_defun_string_lessp,
	p_defun_string_greaterp,
	p_defun_string_not_greaterp,
	p_defun_string_not_lessp,
	p_defun_make_string,
	/* common-sequences */
	p_defun_copy_seq,
	p_defun_elt,
	p_defun_setf_elt,
	p_defun_fill,
	p_defun_make_sequence,
	p_defun_subseq,
	p_defun_setf_subseq,
	p_defun_map,
	p_defun_map_into,
	p_defun_reduce,
	p_defun_count,
	p_defun_count_if,
	p_defun_count_if_not,
	p_defun_length,
	p_defun_reverse,
	p_defun_nreverse,
	p_defun_merge,
	p_defun_sort,
	p_defun_stable_sort,
	p_defun_find,
	p_defun_find_if,
	p_defun_find_if_not,
	p_defun_position,
	p_defun_position_if,
	p_defun_position_if_not,
	p_defun_search,
	p_defun_mismatch,
	p_defun_replace,
	p_defun_substitute,
	p_defun_substitute_if,
	p_defun_substitute_if_not,
	p_defun_nsubstitute,
	p_defun_nsubstitute_if,
	p_defun_nsubstitute_if_not,
	p_defun_concatenate,
	p_defun_remove,
	p_defun_remove_if,
	p_defun_remove_if_not,
	p_defun_delete,
	p_defun_delete_if,
	p_defun_delete_if_not,
	p_defun_remove_duplicates,
	p_defun_delete_duplicates,
	/* common-hashtables */
	p_defun_make_hash_table,
	p_defun_hash_table_p,
	p_defun_hash_table_count,
	p_defun_hash_table_rehash_size,
	p_defun_hash_table_rehash_threshold,
	p_defun_hash_table_size,
	p_defun_hash_table_test,
	p_defun_gethash,
	p_defun_setf_gethash,
	p_defun_remhash,
	p_defun_maphash,
	p_defmacro_with_hash_table_iterator,
	p_defun_clrhash,
	p_defun_sxhash,
	/* common-filenames */
	p_defun_pathname,
	p_defun_make_pathname,
	p_defun_pathnamep,
	p_defun_pathname_host,
	p_defun_pathname_device,
	p_defun_pathname_directory,
	p_defun_pathname_name,
	p_defun_pathname_type,
	p_defun_pathname_version,
	p_defun_load_logical_pathname_translations,
	p_defun_logical_pathname_translations,
	p_defun_setf_logical_pathname_translations,
	p_defun_logical_pathname,
	p_defun_namestring,
	p_defun_file_namestring,
	p_defun_directory_namestring,
	p_defun_host_namestring,
	p_defun_enough_namestring,
	p_defun_parse_namestring,
	p_defun_wild_pathname_p,
	p_defun_pathname_match_p,
	p_defun_translate_pathname,
	p_defun_translate_logical_pathname,
	p_defun_merge_pathnames,
	/* common-files */
	p_defun_directory,
	p_defun_probe_file,
	p_defun_ensure_directories_exist,
	p_defun_truename,
	p_defun_file_author,
	p_defun_file_write_date,
	p_defun_rename_file,
	p_defun_delete_file,
	p_defun_file_error_pathname,
	/* common-streams */
	p_defun_input_stream_p,
	p_defun_output_stream_p,
	p_defun_interactive_stream_p,
	p_defun_open_stream_p,
	p_defun_streamp,
	p_defun_stream_element_type,
	p_defun_read_byte,
	p_defun_write_byte,
	p_defun_peek_char,
	p_defun_read_char,
	p_defun_read_char_no_hang,
	p_defun_terpri,
	p_defun_fresh_line,
	p_defun_unread_char,
	p_defun_write_char,
	p_defun_read_line,
	p_defun_write_string,
	p_defun_write_line,
	p_defun_read_sequence,
	p_defun_write_sequence,
	p_defun_file_length,
	p_defun_file_position,
	p_defun_file_string_length,
	p_defun_open,
	p_defun_stream_external_format,
	p_defmacro_with_open_file,
	p_defun_close,
	p_defmacro_with_open_stream,
	p_defun_listen,
	p_defun_clear_input,
	p_defun_finish_output,
	p_defun_force_output,
	p_defun_clear_output,
	p_defun_y_or_n_p,
	p_defun_yes_or_no_p,
	p_defun_make_synonym_stream,
	p_defun_synonym_stream_symbol,
	p_defun_make_broadcast_stream,
	p_defun_broadcast_stream_streams,
	p_defun_make_two_way_stream,
	p_defun_two_way_stream_input_stream,
	p_defun_two_way_stream_output_stream,
	p_defun_make_echo_stream,
	p_defun_echo_stream_input_stream,
	p_defun_echo_stream_output_stream,
	p_defun_make_concatenated_stream,
	p_defun_concatenated_stream_streams,
	p_defun_make_string_input_stream,
	p_defun_make_string_output_stream,
	p_defun_get_output_stream_string,
	p_defmacro_with_input_from_string,
	p_defmacro_with_output_to_string,
	p_defun_stream_error_stream,
	/* common-printer */
	p_defun_copy_pprint_dispatch,
	p_defmacro_formatter,
	p_defun_pprint_dispatch,
	p_defun_pprint_fill,
	p_defun_pprint_linear,
	p_defun_pprint_tabular,
	p_defun_pprint_indent,
	p_defmacro_pprint_logical_block,
	p_defun_pprint_newline,
	p_defun_pprint_tab,
	p_defmacro_print_unreadable_object,
	p_defun_set_pprint_dispatch,
	p_defun_write,
	p_defun_prin1,
	p_defun_princ,
	p_defun_print,
	p_defun_pprint,
	p_defun_write_to_string,
	p_defun_prin1_to_string,
	p_defun_princ_to_string,
	p_defun_print_not_readable_object,
	p_defun_format,
	p_format_logicalblock2,
	p_format_logicalblock1,
	p_method_print_object_t,
	p_method_print_object_class,
	p_method_print_object_structure_object,
	p_formatter_call_common,
	/* common-reader */
	p_defun_copy_readtable,
	p_defun_make_dispatch_macro_character,
	p_defun_read,
	p_defun_read_preserving_whitespace,
	p_defun_read_delimited_list,
	p_defun_read_from_string,
	p_defun_readtable_case,
	p_defun_setf_readtable_case,
	p_defun_readtablep,
	p_defun_get_dispatch_macro_character,
	p_defun_set_dispatch_macro_character,
	p_defun_get_macro_character,
	p_defun_set_macro_character,
	p_defun_set_syntax_from_char,
	p_defmacro_with_standard_io_syntax,
	/* common-system */
	p_defun_compile_file,
	p_defun_compile_file_pathname,
	p_defun_load,
	p_defmacro_with_compilation_unit,
	p_defun_handler_delay_warning,
	p_defun_finalize_delay_warning,
	p_defun_provide,
	p_defun_require,
	/* common-environment */
	p_defun_decode_universal_time,
	p_defun_encode_universal_time,
	p_defun_get_universal_time,
	p_defun_get_decoded_time,
	p_defun_sleep,
	p_sleep_continue,
	p_sleep_close_object,
	p_defun_apropos,
	p_defun_apropos_list,
	p_defun_describe,
	p_method_describe_object_t,
	p_method_describe_object_class,
	p_method_describe_object_standard_object,
	p_method_describe_object_structure_object,
	p_defmacro_trace,
	p_defmacro_untrace,
	p_defun_trace,
	p_defun_trace_function,
	p_defun_untrace,
	p_defmacro_step,
	p_defmacro_time,
	p_defun_get_internal_real_time,
	p_defun_get_internal_run_time,
	p_defun_disassemble,
	p_defun_room,
	p_defun_ed,
	p_defun_inspect,
	p_defun_dribble,
	p_defun_lisp_implementation_type,
	p_defun_lisp_implementation_version,
	p_defun_short_site_name,
	p_defun_long_site_name,
	p_defun_machine_instance,
	p_defun_machine_type,
	p_defun_machine_version,
	p_defun_software_type,
	p_defun_software_version,
	p_defun_user_homedir_pathname,
	/* documentation */
	p_method_documentation_function_t,
	p_method_documentation_list_function,
	p_method_documentation_list_compiled_function,
	p_method_documentation_symbol_function,
	p_method_documentation_symbol_compiled_function,
	p_method_documentation_symbol_setf,
	p_method_documentation_method_combination_t,
	p_method_documentation_method_combination_method_combination,
	p_method_documentation_symbol_method_combination,
	p_method_documentation_standard_method_t,
	p_method_documentation_package_t,
	p_method_documentation_standard_class_t,
	p_method_documentation_structure_class_t,
	p_method_documentation_symbol_type,
	p_method_documentation_symbol_structure,
	p_method_documentation_symbol_variable,
	p_method_setf_documentation_function_t,
	p_method_setf_documentation_list_function,
	p_method_setf_documentation_list_compiled_function,
	p_method_setf_documentation_symbol_function,
	p_method_setf_documentation_symbol_compiled_function,
	p_method_setf_documentation_symbol_setf,
	p_method_setf_documentation_method_combination_t,
	p_method_setf_documentation_method_combination_method_combination,
	p_method_setf_documentation_symbol_method_combination,
	p_method_setf_documentation_standard_method_t,
	p_method_setf_documentation_package_t,
	p_method_setf_documentation_standard_class_t,
	p_method_setf_documentation_structure_class_t,
	p_method_setf_documentation_symbol_type,
	p_method_setf_documentation_symbol_structure,
	p_method_setf_documentation_symbol_variable,
	/* clos */
	p_defun_standard_lambda,
	p_defun_clos_ensure_reader,
	p_defun_clos_ensure_writer_instance,
	/* readtable */
	p_defun_reader_double_quote,
	p_defun_reader_single_quote,
	p_defun_reader_parensis_open,
	p_defun_reader_parensis_close,
	p_defun_reader_semicolon,
	p_defun_reader_backquote,
	p_defun_reader_comma,
	p_defun_reader_sharp,
	p_defun_dispatch_error,
	p_defun_dispatch_equal,
	p_defun_dispatch_sharp,
	p_defun_dispatch_single_quote,
	p_defun_dispatch_parensis_open,
	p_defun_dispatch_parensis_close,
	p_defun_dispatch_asterisk,
	p_defun_dispatch_colon,
	p_defun_dispatch_less,
	p_defun_dispatch_backslash,
	p_defun_dispatch_or,
	p_defun_dispatch_plus,
	p_defun_dispatch_minus,
	p_defun_dispatch_dot,
	p_defun_dispatch_radix,
	p_defun_dispatch_binary,
	p_defun_dispatch_octal,
	p_defun_dispatch_hexadecimal,
	p_defun_dispatch_complex,
	p_defun_dispatch_array,
	p_defun_dispatch_pathname,
	p_defun_dispatch_structure,
	p_equal_finalize_dispatch,
	/* package */
	p_defun_defpackage_make,
	/* pathname */
	p_defun_set_logical_pathname_translations,
	/* condition */
	p_defun_restart_warning,
	p_defun_handler_warning,
	p_defun_handler_savecore,
	/* eval */
	p_defun_eval_loop_abort_function,
	p_defun_eval_loop_abort_report,
	p_defun_eval_loop_abort_test,
	p_defun_handler_compile,
	p_defun_free_eval_stack,
	/* rt */
	p_defun_push_entries,
	p_defun_rem_all_tests,
	p_defmacro_deftest,
	p_defmacro_deftest_error,
	p_defun_do_tests,
	p_defun_equalrt,
	/* structure */
	p_defun_structure_reader_list,
	p_defun_structure_reader_vector,
	p_defun_structure_reader_clos,
	p_defun_structure_writer_list,
	p_defun_structure_writer_vector,
	p_defun_structure_writer_clos,
	p_defun_structure_constructor_list,
	p_defun_structure_constructor_vector,
	p_defun_structure_constructor_clos,
	p_defun_structure_copier_list,
	p_defun_structure_copier_vector,
	p_defun_structure_copier_clos,
	p_defun_structure_predicate_list,
	p_defun_structure_predicate_vector,
	p_defun_structure_predicate_clos,
	p_method_defstruct_default,
	p_method_defstruct_object,
	p_method_defstruct_function,
	/* input */
	p_readlist_finalize,
	/* extern */
	p_extern_unwind_protect,
	/* pprint */
	p_pprint_logical_block_close,
	p_pprint_logical_block_fill_form,
	p_pprint_logical_block_fill,
	p_pprint_logical_block_linear_form,
	p_pprint_logical_block_linear,
	p_pprint_logical_block_tabular_form,
	p_pprint_logical_block_tabular,
	p_pprint_dispatch_vector2,
	p_pprint_dispatch_vector1,
	p_pprint_dispatch_vector,
	p_pprint_dispatch_call2,
	p_pprint_dispatch_call1,
	p_pprint_dispatch_call,
	p_pprint_dispatch_defun6,
	p_pprint_dispatch_defun5,
	p_pprint_dispatch_defun4,
	p_pprint_dispatch_defun3,
	p_pprint_dispatch_defun2,
	p_pprint_dispatch_defun1,
	p_pprint_dispatch_defun,
	p_pprint_dispatch_let2,
	p_pprint_dispatch_let1,
	p_pprint_dispatch_let,
	/* restart */
	p_restart_symbol_use_function,
	p_restart_symbol_use_interactive,
	p_restart_symbol_use_report,
	p_restart_symbol_use_test,
	p_restart_symbol_store_global,
	p_restart_symbol_store_special,
	p_restart_symbol_store_interactive,
	p_restart_symbol_store_report,
	p_restart_function_use_function,
	/* stream */
	p_finalize_close_stream,
	/* syscall */
	p_defun_syscall_hello,
	p_defun_syscall_infobit,
	p_defun_syscall_infoprint,
	p_defun_syscall_gc,
	p_defun_syscall_savecore,
	p_defun_syscall_redirect_restart,
	p_defmacro_syscall_symbol_macro_expander,
	p_defun_syscall_defconstant,
	p_defun_syscall_in_package,
	p_defun_syscall_setplist,
	p_defun_syscall_remplist,
	p_defun_syscall_make_hash_iterator,
	p_defun_syscall_next_hash_iterator,
	p_defun_syscall_make_package_iterator,
	p_defun_syscall_next_package_iterator,
	p_defun_syscall_defpackage,
	p_defun_syscall_do_symbols,
	p_defun_syscall_do_external_symbols,
	p_defun_syscall_do_all_symbols,
	p_defun_syscall_getdoc_variable,
	p_defun_syscall_setdoc_variable,
	p_defun_syscall_specialp,
	p_defun_syscall_ecase_error,
	p_defun_syscall_etypecase_error,
	p_defun_syscall_define_setf_expander,
	p_defun_syscall_defsetf_short,
	p_defun_syscall_defsetf_long,
	p_defun_syscall_array_general_p,
	p_defun_syscall_array_specialized_p,
	p_defun_syscall_simple_sort,
	p_defun_syscall_bubble_sort,
	p_defun_syscall_quick_sort,
	p_defun_syscall_merge_sort,
	p_defun_syscall_exit,
	p_defun_syscall_end_input_stream,
	p_defun_syscall_make_extend_output_stream,
	p_defun_syscall_prompt_for,
	p_defun_syscall_closp,
	p_defun_syscall_fixnump,
	p_defun_syscall_bignump,
	p_defun_syscall_ratiop,
	p_defun_syscall_short_float_p,
	p_defun_syscall_single_float_p,
	p_defun_syscall_double_float_p,
	p_defun_syscall_long_float_p,
	p_defun_syscall_callnamep,
	p_defun_syscall_large_number,
	p_defun_syscall_print_unreadable_call,
	p_defun_syscall_write_default,
	p_defun_syscall_make_bignum,
	p_defun_syscall_make_ratio,
	p_defun_syscall_make_complex,
	p_defun_syscall_equal_random_state,
	p_defun_syscall_symbol_deftype,
	p_defun_syscall_delete_deftype,
	p_defun_syscall_subtypep_result,
	p_defun_syscall_ensure_structure,
	p_defun_syscall_structure_constructor,
	p_defun_syscall_loop_bind,
	p_defun_syscall_make_pprint_stream,
	p_defun_syscall_pprint_gensym,
	p_defun_syscall_pprint_exit,
	p_defun_syscall_pprint_pop,
	p_defun_syscall_pprint_close,
	p_defun_syscall_pprint_check,
	p_defun_syscall_pprint_pretty,
	p_defun_syscall_eastasian_set,
	p_defun_syscall_eastasian_get,
	p_defun_syscall_eastasian_width,
	p_defun_syscall_timeinfo,
	p_defun_syscall_ed_function,
	p_defun_syscall_run_program,
	p_defun_syscall_make_callname,
	p_defun_syscall_trace_add,
	p_defun_syscall_trace_del,
	p_defun_syscall_with_compilation_unit,
	p_defun_syscall_set_slots,
	p_defun_syscall_remove_file,
	p_defun_syscall_remove_directory,
	p_defmacro_syscall_declare_parse,

	/* size */
	p_size
};

typedef enum pointer_index pointer;

#endif

