#ifndef __PARSE_OBJECT_HEADER__
#define __PARSE_OBJECT_HEADER__

#include "eval_object.h"
#include "local.h"
#include "parse_typedef.h"
#include "typedef.h"

struct eval_parse {
	EvalParse type;
};

#define StructEvalParse_Low(x)		((struct eval_parse *)PtrEvalBodyAny(x))
#define RefEvalParse_Low(x,i)		RefEval((x),(i))
#define GetEvalParse_Low(x,i,v)		GetEval((x),(i),(v))
#define SetEvalParse_Low(x,i,v)		SetEval((x),(i),(v))
#define RefEvalParseType_Low(x)		(StructEvalParse_Low(x)->type)
#define GetEvalParseType_Low(x,v)	(*(v) = RefEvalParseType_Low(x))
#define SetEvalParseType_Low(x,v)	(RefEvalParseType_Low(x) = (v))

#ifdef LISP_DEBUG
#define StructEvalParse(x)			structevalparse(x)
#define RefEvalParse(x,i)			refevalparse(x,i)
#define GetEvalParse(x,i,v)			getevalparse(x,i,v)
#define SetEvalParse(x,i,v)			setevalparse(x,i,v)
#define RefEvalParseType(x)			refevalparsetype(x)
#define GetEvalParseType(x,v)		getevalparsetype(x,v)
#define SetEvalParseType(x,v)		setevalparsetype(x,v)
#else
#define StructEvalParse(x)			StructEvalParse_Low(x)
#define RefEvalParse(x,i)			RefEvalParse_Low(x,i)
#define GetEvalParse(x,i,v)			GetEvalParse_Low(x,i,v)
#define SetEvalParse(x,i,v)			SetEvalParse_Low(x,i,v)
#define RefEvalParseType(x)			RefEvalParseType_Low(x)
#define GetEvalParseType(x,v)		GetEvalParseType_Low(x,v)
#define SetEvalParseType(x,v)		SetEvalParseType_Low(x,v)
#endif

_g struct eval_parse *structevalparse(addr pos);
_g addr refevalparse(addr pos, size_t index);
_g void getevalparse(addr pos, size_t index, addr *ret);
_g void setevalparse(addr pos, size_t index, addr value);
_g EvalParse refevalparsetype(addr pos);
_g void getevalparsetype(addr pos, EvalParse *ret);
_g void setevalparsetype(addr pos, EvalParse value);

_g void eval_parse_alloc(LocalRoot local, addr *ret, EvalParse type, byte array);
_g void eval_parse_local(LocalRoot local, addr *ret, EvalParse type, byte array);
_g void eval_parse_heap(addr *ret, EvalParse type, byte array);

_g void eval_single_parse_alloc(LocalRoot local, addr *ret, EvalParse type, addr value);
_g void eval_single_parse_local(LocalRoot local, addr *ret, EvalParse type, addr value);
_g void eval_single_parse_heap(addr *ret, EvalParse type, addr value);

#endif

