#include "environment.h"
#include "env_code.h"
#include "env_describe.h"
#include "env_time.h"

void init_environment(void)
{
	init_environment_code();
	init_environment_describe();
	init_environemnt_time();
}

void build_environment(Execute ptr)
{
	build_environment_code();
	build_environment_describe(ptr);
}

