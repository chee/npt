#ifndef __PROMPT_ARCH_HEADER__
#define __PROMPT_ARCH_HEADER__

#include "execute.h"
#include "typedef.h"

#define input_prompt_ _n(input_prompt_)

int input_prompt_(Execute ptr, addr *ret);

#endif

