#ifndef __CONTROL_EXECUTE_HEADER__
#define __CONTROL_EXECUTE_HEADER__

#include "execute.h"
#include "typedef.h"

_g int runcode_control_(Execute ptr, addr code);

_g int execute_control(Execute ptr, addr call);
_g int apply_control(Execute ptr, addr call, addr args);
_g int applya_control(Execute ptr, addr call, ...);
_g int funcall_control(Execute ptr, addr call, ...);
_g int call_control(Execute ptr, addr args);

_g int callclang_apply(Execute ptr, addr *ret, addr call, addr cons);
_g int callclang_applya(Execute ptr, addr *ret, addr call, ...);
_g int callclang_funcall(Execute ptr, addr *ret, addr call, ...);

#endif

