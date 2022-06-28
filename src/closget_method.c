#include "clos_define.h"
#include "clos_object.h"
#include "closget.h"
#include "closget_method.h"
#include "constant.h"
#include "symbol.h"
#include "typedef.h"

/*
 *  access
 */
static int stdget_method_constant_(addr pos, addr *ret,
		enum Clos_method_Index index1, constindex index2)
{
	addr clos, check;

	CheckType(pos, LISPTYPE_CLOS);
	Check(Clos_method_size <= index1, "index error");
	GetClassOfClos(pos, &clos);
	Check(clos == Unbound, "unbound error");
	GetConst(CLOS_STANDARD_METHOD, &check);
	if (clos == check) {
		Check(clos_errorp(pos, (size_t)index1, index2), "index error");
		return clos_checkelt_(pos, (size_t)index1, ret);
	}
	else {
		GetConstant(index2, &check);
		return clos_check_(pos, check, ret);
	}
}

static int stdset_method_constant_(addr pos, addr value,
		enum Clos_method_Index index1, constindex index2)
{
	addr clos, check;

	CheckType(pos, LISPTYPE_CLOS);
	Check(Clos_method_size <= index1, "index error");
	GetClassOfClos(pos, &clos);
	Check(clos == Unbound, "unbound error");
	GetConst(CLOS_STANDARD_METHOD, &check);
	if (clos == check) {
		Check(clos_errorp(pos, (size_t)index1, index2), "index error");
		clos_setelt(pos, (size_t)index1, value);
		return 0;
	}
	else {
		GetConstant(index2, &check);
		return clos_set_(pos, check, value);
	}
}
#define StdGetMethod_(p,r,a,b) \
	stdget_method_constant_((p), (r), Clos_method_##a, CONSTANT_CLOSNAME_##b)
#define StdSetMethod_(p,r,a,b) \
	stdset_method_constant_((p), (r), Clos_method_##a, CONSTANT_CLOSNAME_##b)

int stdget_method_function_(addr pos, addr *ret)
{
	return StdGetMethod_(pos, ret, function, FUNCTION);
}
int stdset_method_function_(addr pos, addr value)
{
	return StdSetMethod_(pos, value, function, FUNCTION);
}

int stdget_method_generic_function_(addr pos, addr *ret)
{
	return StdGetMethod_(pos, ret, generic_function, GENERIC_FUNCTION);
}
int stdset_method_generic_function_(addr pos, addr value)
{
	return StdSetMethod_(pos, value, generic_function, GENERIC_FUNCTION);
}

int stdget_method_lambda_list_(addr pos, addr *ret)
{
	return StdGetMethod_(pos, ret, lambda_list, LAMBDA_LIST);
}
int stdset_method_lambda_list_(addr pos, addr value)
{
	return StdSetMethod_(pos, value, lambda_list, LAMBDA_LIST);
}

int stdget_method_qualifiers_(addr pos, addr *ret)
{
	return StdGetMethod_(pos, ret, qualifiers, QUALIFIERS);
}
int stdset_method_qualifiers_(addr pos, addr value)
{
	return StdSetMethod_(pos, value, qualifiers, QUALIFIERS);
}

int stdget_method_specializers_(addr pos, addr *ret)
{
	return StdGetMethod_(pos, ret, specializers, SPECIALIZERS);
}
int stdset_method_specializers_(addr pos, addr value)
{
	return StdSetMethod_(pos, value, specializers, SPECIALIZERS);
}

