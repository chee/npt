#ifndef __PARSE_TYPEDEF_HEADER__
#define __PARSE_TYPEDEF_HEADER__

enum EVAL_PARSE {
	EVAL_PARSE_EMPTY,
	/* constant */
	EVAL_PARSE_NIL,
	EVAL_PARSE_T,
	EVAL_PARSE_TYPE,
	EVAL_PARSE_CLOS,
	EVAL_PARSE_INTEGER,
	EVAL_PARSE_RATIONAL,
	EVAL_PARSE_COMPLEX,
	EVAL_PARSE_CHARACTER,
	EVAL_PARSE_ARRAY,
	EVAL_PARSE_VECTOR,
	EVAL_PARSE_BITVECTOR,
	EVAL_PARSE_STRING,
	EVAL_PARSE_SYMBOL,
	EVAL_PARSE_FLOAT,
	EVAL_PARSE_DECLAIM,
	EVAL_PARSE_RANDOM_STATE,
	EVAL_PARSE_PATHNAME,
	EVAL_PARSE_ENVIRONMENT,
	EVAL_PARSE_LEXICAL,
	/* cons */
	EVAL_PARSE_PROGN,
	EVAL_PARSE_LET,
	EVAL_PARSE_LETA,
	EVAL_PARSE_SETQ,
	EVAL_PARSE_DEFUN,
	EVAL_PARSE_DEFMACRO,
	EVAL_PARSE_MACRO_LAMBDA,
	EVAL_PARSE_DEFTYPE,
	EVAL_PARSE_DEFINE_COMPILER_MACRO,
	EVAL_PARSE_DESTRUCTURING_BIND,
	EVAL_PARSE_DEFINE_SYMBOL_MACRO,
	EVAL_PARSE_SYMBOL_MACROLET,
	EVAL_PARSE_QUOTE,
	EVAL_PARSE_FUNCTION,
	EVAL_PARSE_LAMBDA,
	EVAL_PARSE_IF,
	EVAL_PARSE_UNWIND_PROTECT,
	EVAL_PARSE_TAGBODY,
	EVAL_PARSE_TAG,
	EVAL_PARSE_GO,
	EVAL_PARSE_BLOCK,
	EVAL_PARSE_RETURN_FROM,
	EVAL_PARSE_CATCH,
	EVAL_PARSE_THROW,
	EVAL_PARSE_FLET,
	EVAL_PARSE_LABELS,
	EVAL_PARSE_THE,
	EVAL_PARSE_EVAL_WHEN,
	EVAL_PARSE_VALUES,
	EVAL_PARSE_LOCALLY,
	EVAL_PARSE_CALL,
	EVAL_PARSE_MULTIPLE_VALUE_BIND,
	EVAL_PARSE_MULTIPLE_VALUE_CALL,
	EVAL_PARSE_MULTIPLE_VALUE_PROG1,
	EVAL_PARSE_NTH_VALUE,
	EVAL_PARSE_PROGV,
	EVAL_PARSE_LOAD_TIME_VALUE,
	/* size */
	EVAL_PARSE_SIZE
};

typedef enum EVAL_PARSE EvalParse;

#endif

