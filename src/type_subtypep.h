#ifndef __TYPE_SUBTYPEP_HEADER__
#define __TYPE_SUBTYPEP_HEADER__

#include "type.h"
#include "typedef.h"

enum SUBTYPEP_RESULT {
	SUBTYPEP_INCLUDE,
	SUBTYPEP_EXCLUDE,
	SUBTYPEP_FALSE,
	SUBTYPEP_INVALID,
	SUBTYPEP_SIZE
};
typedef enum SUBTYPEP_RESULT SubtypepResult;

_g void init_type_subtypep(void);
_g int subtypep_result_(addr left, addr right, int aster, SubtypepResult *ret);
_g int subtypep_asterisk_clang_(addr left, addr right, int *ret, int *validp);
_g int subtypep_clang_(addr left, addr right, int *ret, int *validp);
_g int subtypep_common(Execute ptr, addr x, addr y, addr env, addr *v1, addr *v2);
_g int subtypep_result_syscall(Execute ptr, addr left, addr right, addr *ret);

#endif

