#ifndef __OPTIMIZE_HEADER__
#define __OPTIMIZE_HEADER__

#include "define.h"
#include "eval.h"
#include "eval_declare.h"
#include "eval_parse.h"
#include "local.h"
#include "typedef.h"

struct optimize_value {
	OptimizeType declaim[EVAL_OPTIMIZE_SIZE];
	OptimizeType local[EVAL_OPTIMIZE_SIZE];
};

struct optimize_struct {
	LocalRoot local;
	int update;
	addr pos;
	struct optimize_value value;
};

typedef int (*optimize_call)(struct optimize_struct *);

_g void save_optimize_value(const struct optimize_struct *str,
		struct optimize_value *save);
_g void rollback_optimize_value(struct optimize_struct *str,
		const struct optimize_value *save);

_g int optimize_speed_on(struct optimize_struct *str);
_g int optimize_evaltype(addr pos, enum EVAL_PARSE type);
_g int optimize_evaltype_on(struct optimize_struct *str, enum EVAL_PARSE type);
_g void optimize_initialize(struct optimize_struct *str, LocalRoot local, addr pos);
_g int optimize_extract(struct optimize_struct *str, optimize_call call);

#endif

