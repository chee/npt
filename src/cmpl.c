#include "bignum.h"
#include "cmpl.h"
#include "cmpl_math.h"
#include "condition.h"
#include "equal.h"
#include "heap.h"
#include "integer.h"
#include "lisp.h"
#include "number.h"
#include "ratio.h"
#include "rational.h"
#include "real.h"
#include "real_float.h"
#include "type.h"

int complexp(addr pos)
{
	return GetType(pos) == LISPTYPE_COMPLEX;
}

void setreal_complex(addr pos, addr value)
{
	CheckType(pos, LISPTYPE_COMPLEX);
	Check(GetStatusReadOnly(pos), "readonly error");
	SetRealComplex_Low(pos, value);
}

void getreal_complex(addr pos, addr *ret)
{
	CheckType(pos, LISPTYPE_COMPLEX);
	GetRealComplex_Low(pos, ret);
}

void setimag_complex(addr pos, addr value)
{
	CheckType(pos, LISPTYPE_COMPLEX);
	Check(GetStatusReadOnly(pos), "readonly error");
	SetImagComplex_Low(pos, value);
}

void getimag_complex(addr pos, addr *ret)
{
	CheckType(pos, LISPTYPE_COMPLEX);
	GetImagComplex_Low(pos, ret);
}

void settype_complex(addr pos, int value)
{
	CheckType(pos, LISPTYPE_COMPLEX);
	Check(GetStatusReadOnly(pos), "readonly error");
	SetTypeComplex_Low(pos, value);
}

enum ComplexType gettype_complex(addr pos)
{
	CheckType(pos, LISPTYPE_COMPLEX);
	return GetTypeComplex_Low(pos);
}

enum ComplexType getcomplex(addr pos, enum ComplexType *type, addr *real, addr *imag)
{
	CheckType(pos, LISPTYPE_COMPLEX);
	*type = GetTypeComplex_Low(pos);
	GetRealComplex_Low(pos, real);
	GetImagComplex_Low(pos, imag);
	return *type;
}

enum ComplexType getcomplexr(addr pos, addr *real, addr *imag)
{
	CheckType(pos, LISPTYPE_COMPLEX);
	GetRealComplex_Low(pos, real);
	GetImagComplex_Low(pos, imag);
	return GetTypeComplex_Low(pos);
}


/*
 *  rational complex  -> #c(1/2 3)
 *  float complex     -> #c(0.5 3.0)
 */
void make_complex_unsafe(LocalRoot local, addr *ret, enum ComplexType type)
{
	alloc_array2(local, ret, LISPTYPE_COMPLEX, 2);
	SetTypeComplex(*ret, type);
}

void complex_unsafe_alloc(LocalRoot local, addr *ret,
		addr real, addr imag, enum ComplexType type)
{
	addr pos;

	if (zerop_real(imag)) {
		real_throw_alloc(local, real, ret);
	}
	else {
		real_throw_alloc(local, real, &real);
		real_throw_alloc(local, imag, &imag);
		make_complex_unsafe(local, &pos, type);
		SetRealComplex(pos, real);
		SetImagComplex(pos, imag);
		*ret = pos;
	}
}

void complex_unsafe_local(LocalRoot local, addr *ret,
		addr real, addr imag, enum ComplexType type)
{
	Check(local == NULL, "local error");
	complex_unsafe_alloc(local, ret, real, imag, type);
}

void complex_unsafe_heap(addr *ret,
		addr real, addr imag, enum ComplexType type)
{
	complex_unsafe_alloc(NULL, ret, real, imag, type);
}

static void complex_fixnum(LocalRoot local, addr *ret, addr real, addr imag)
{
	switch (GetType(imag)) {
		case LISPTYPE_SINGLE_FLOAT:
			single_float_alloc(local, &real, (single_float)RefFixnum(real));
			single_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_single);
			break;

		case LISPTYPE_DOUBLE_FLOAT:
			double_float_fixnum_alloc(local, &real, real);
			double_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_double);
			break;

		case LISPTYPE_LONG_FLOAT:
			long_float_fixnum_alloc(local, &real, real);
			long_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_long);
			break;

		case LISPTYPE_FIXNUM:
			fixnum_throw_alloc(local, real, &real);
			fixnum_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_rational);
			break;

		case LISPTYPE_BIGNUM:
			fixnum_throw_alloc(local, real, &real);
			bignum_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_rational);
			break;

		case LISPTYPE_RATIO:
			fixnum_throw_alloc(local, real, &real);
			ratio_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_rational);
			break;

		default:
			TypeError(imag, REAL);
			break;
	}
}

static void complex_bignum(LocalRoot local, addr *ret, addr real, addr imag)
{
	switch (GetType(imag)) {
		case LISPTYPE_SINGLE_FLOAT:
			single_float_bignum_alloc(local, &real, real);
			single_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_single);
			break;

		case LISPTYPE_DOUBLE_FLOAT:
			double_float_bignum_alloc(local, &real, real);
			double_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_double);
			break;

		case LISPTYPE_LONG_FLOAT:
			long_float_bignum_alloc(local, &real, real);
			long_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_long);
			break;

		case LISPTYPE_FIXNUM:
			bignum_throw_alloc(local, real, &real);
			fixnum_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_rational);
			break;

		case LISPTYPE_BIGNUM:
			bignum_throw_alloc(local, real, &real);
			bignum_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_rational);
			break;

		case LISPTYPE_RATIO:
			bignum_throw_alloc(local, real, &real);
			ratio_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_rational);
			break;

		default:
			TypeError(imag, REAL);
			break;
	}
}

static void complex_ratio(LocalRoot local, addr *ret, addr real, addr imag)
{
	switch (GetType(imag)) {
		case LISPTYPE_SINGLE_FLOAT:
			single_float_alloc(local, &real, single_float_ratio(real));
			single_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_single);
			break;

		case LISPTYPE_DOUBLE_FLOAT:
			double_float_alloc(local, &real, double_float_ratio(real));
			double_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_double);
			break;

		case LISPTYPE_LONG_FLOAT:
			long_float_alloc(local, &real, long_float_ratio(real));
			long_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_long);
			break;

		case LISPTYPE_FIXNUM:
			ratio_throw_alloc(local, real, &real);
			fixnum_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_rational);
			break;

		case LISPTYPE_BIGNUM:
			ratio_throw_alloc(local, real, &real);
			bignum_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_rational);
			break;

		case LISPTYPE_RATIO:
			ratio_throw_alloc(local, real, &real);
			ratio_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_rational);
			break;

		default:
			TypeError(imag, REAL);
			break;
	}
}

static void complex_single(LocalRoot local, addr *ret, addr real, addr imag)
{
	switch (GetType(imag)) {
		case LISPTYPE_SINGLE_FLOAT:
			single_float_throw_alloc(local, real, &real);
			single_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_single);
			break;

		case LISPTYPE_DOUBLE_FLOAT:
			double_float_alloc(local, &real, (double_float)RefSingleFloat(real));
			double_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_double);
			break;

		case LISPTYPE_LONG_FLOAT:
			long_float_alloc(local, &real, (long_float)RefSingleFloat(real));
			long_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_long);
			break;

		case LISPTYPE_FIXNUM:
			single_float_throw_alloc(local, real, &real);
			single_float_alloc(local, &imag, (single_float)RefFixnum(imag));
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_single);
			break;

		case LISPTYPE_BIGNUM:
			single_float_throw_alloc(local, real, &real);
			single_float_bignum_alloc(local, &imag, imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_single);
			break;

		case LISPTYPE_RATIO:
			single_float_throw_alloc(local, real, &real);
			single_float_alloc(local, &imag, single_float_ratio(imag));
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_single);
			break;

		default:
			TypeError(imag, REAL);
			break;
	}
}

static void complex_double(LocalRoot local, addr *ret, addr real, addr imag)
{
	switch (GetType(imag)) {
		case LISPTYPE_SINGLE_FLOAT:
			double_float_throw_alloc(local, real, &real);
			double_float_alloc(local, &imag, (double_float)RefSingleFloat(imag));
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_double);
			break;

		case LISPTYPE_DOUBLE_FLOAT:
			double_float_throw_alloc(local, real, &real);
			double_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_double);
			break;

		case LISPTYPE_LONG_FLOAT:
			long_float_alloc(local, &real, (long_float)RefDoubleFloat(real));
			long_float_throw_alloc(local, imag, &imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_long);
			break;

		case LISPTYPE_FIXNUM:
			double_float_throw_alloc(local, real, &real);
			double_float_fixnum_alloc(local, &imag, imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_double);
			break;

		case LISPTYPE_BIGNUM:
			double_float_throw_alloc(local, real, &real);
			double_float_bignum_alloc(local, &imag, imag);
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_double);
			break;

		case LISPTYPE_RATIO:
			double_float_throw_alloc(local, real, &real);
			double_float_alloc(local, &imag, double_float_ratio(imag));
			complex_unsafe_alloc(local, ret, real, imag, ComplexType_double);
			break;

		default:
			TypeError(imag, REAL);
			break;
	}
}

static void complex_long(LocalRoot local, addr *ret, addr real, addr imag)
{
	switch (GetType(imag)) {
		case LISPTYPE_SINGLE_FLOAT:
			long_float_throw_alloc(local, real, &real);
			long_float_alloc(local, &imag, (long_float)RefSingleFloat(imag));
			break;

		case LISPTYPE_DOUBLE_FLOAT:
			long_float_throw_alloc(local, real, &real);
			long_float_alloc(local, &imag, (long_float)RefDoubleFloat(imag));
			break;

		case LISPTYPE_LONG_FLOAT:
			long_float_throw_alloc(local, real, &real);
			long_float_throw_alloc(local, imag, &imag);
			break;

		case LISPTYPE_FIXNUM:
			long_float_throw_alloc(local, real, &real);
			long_float_fixnum_alloc(local, &imag, imag);
			break;

		case LISPTYPE_BIGNUM:
			long_float_throw_alloc(local, real, &real);
			long_float_bignum_alloc(local, &imag, imag);
			break;

		case LISPTYPE_RATIO:
			long_float_throw_alloc(local, real, &real);
			long_float_alloc(local, &imag, long_float_ratio(imag));
			break;

		default:
			TypeError(imag, REAL);
			break;
	}
	complex_unsafe_alloc(local, ret, real, imag, ComplexType_long);
}

void complex_alloc(LocalRoot local, addr *ret, addr real, addr imag)
{
	switch (GetType(real)) {
		case LISPTYPE_SINGLE_FLOAT:
			complex_single(local, ret, real, imag);
			break;

		case LISPTYPE_DOUBLE_FLOAT:
			complex_double(local, ret, real, imag);
			break;

		case LISPTYPE_LONG_FLOAT:
			complex_long(local, ret, real, imag);
			break;

		case LISPTYPE_FIXNUM:
			complex_fixnum(local, ret, real, imag);
			break;

		case LISPTYPE_BIGNUM:
			complex_bignum(local, ret, real, imag);
			break;

		case LISPTYPE_RATIO:
			complex_ratio(local, ret, real, imag);
			break;

		default:
			TypeError(real, REAL);
			break;
	}
}
void complex_local(LocalRoot local, addr *ret, addr real, addr imag)
{
	Check(local == NULL, "local error");
	complex_alloc(local, ret, real, imag);
}
void complex_heap(addr *ret, addr real, addr imag)
{
	complex_alloc(NULL, ret, real, imag);
}

void complex_single_alloc(LocalRoot local,
		addr *ret, single_float real, single_float imag)
{
	addr pos1, pos2;

	single_float_check_alloc(local, &pos1, real);
	single_float_check_alloc(local, &pos2, imag);
	complex_unsafe_alloc(local, ret, pos1, pos2, ComplexType_single);
}
void complex_single_local(LocalRoot local,
		addr *ret, single_float real, single_float imag)
{
	Check(local == NULL, "local error");
	complex_single_alloc(local, ret, real, imag);
}
void complex_single_heap(addr *ret,
		single_float real, single_float imag)
{
	complex_single_alloc(NULL, ret, real, imag);
}

void complex_double_alloc(LocalRoot local,
		addr *ret, double_float real, double_float imag)
{
	addr pos1, pos2;

	double_float_check_alloc(local, &pos1, real);
	double_float_check_alloc(local, &pos2, imag);
	complex_unsafe_alloc(local, ret, pos1, pos2, ComplexType_double);
}
void complex_double_local(LocalRoot local,
		addr *ret, double_float real, double_float imag)
{
	Check(local == NULL, "local error");
	complex_double_alloc(local, ret, real, imag);
}
void complex_double_heap(addr *ret,
		double_float real, double_float imag)
{
	complex_double_alloc(NULL, ret, real, imag);
}

void complex_long_alloc(LocalRoot local,
		addr *ret, long_float real, long_float imag)
{
	addr pos1, pos2;

	long_float_check_alloc(local, &pos1, real);
	long_float_check_alloc(local, &pos2, imag);
	complex_unsafe_alloc(local, ret, pos1, pos2, ComplexType_long);
}
void complex_long_local(LocalRoot local,
		addr *ret, long_float real, long_float imag)
{
	Check(local == NULL, "local error");
	complex_long_alloc(local, ret, real, imag);
}
void complex_long_heap(addr *ret,
		long_float real, long_float imag)
{
	complex_long_alloc(NULL, ret, real, imag);
}

void complex_copy_alloc(LocalRoot local, addr pos, addr *ret)
{
	enum ComplexType type;
	addr real, imag;

	Check(GetType(pos) != LISPTYPE_COMPLEX, "type error");
	getcomplex(pos, &type, &real, &imag);
	real_copy_alloc(local, real, &real);
	real_copy_alloc(local, imag, &imag);
	complex_unsafe_alloc(local, ret, real, imag, type);
}
void complex_copy_local(LocalRoot local, addr pos, addr *ret)
{
	Check(local == NULL, "local error");
	complex_copy_alloc(local, pos, ret);
}
void complex_copy_heap(addr pos, addr *ret)
{
	complex_copy_alloc(NULL, pos, ret);
}

void complex_result_local(LocalRoot local, addr pos, addr *ret)
{
	addr real, imag;

	Check(local == NULL, "local error");
	CheckType(pos, LISPTYPE_COMPLEX);
	GetRealComplex(pos, &real);
	GetImagComplex(pos, &imag);
	if (zerop_real(imag)) {
		real_result_local(local, real, ret);
	}
	else {
		real_result_local(local, real, &real);
		real_result_local(local, imag, &imag);
		complex_local(local, ret, real, imag);
	}
}
void complex_result_heap(LocalRoot local, addr pos, addr *ret)
{
	addr real, imag;

	Check(local == NULL, "local error");
	CheckType(pos, LISPTYPE_COMPLEX);
	GetRealComplex(pos, &real);
	GetImagComplex(pos, &imag);
	if (zerop_real(imag)) {
		real_result_heap(local, real, ret);
	}
	else {
		real_result_heap(local, real, &real);
		real_result_heap(local, imag, &imag);
		complex_heap(ret, real, imag);
	}
}

void complex_throw_alloc(LocalRoot local, addr pos, addr *ret)
{
	Check(GetType(pos) != LISPTYPE_COMPLEX, "type error");
	if (GetStatusDynamic(pos))
		complex_copy_heap(pos, ret);
	else
		*ret = pos;
}
void complex_throw_local(LocalRoot local, addr pos, addr *ret)
{
	Check(local == NULL, "local error");
	complex_throw_alloc(local, pos, ret);
}

void complex_throw_heap(addr pos, addr *ret)
{
	complex_throw_alloc(NULL, pos, ret);
}

void single_float_complex(addr pos, single_float *re, single_float *im)
{
	addr real, imag;

	CheckType(pos, LISPTYPE_COMPLEX);
	switch (getcomplexr(pos, &real, &imag)) {
		case ComplexType_single:
			*re = cast_ss_value(real);
			*im = cast_ss_value(imag);
			break;

		case ComplexType_double:
			*re = cast_ds_value(real);
			*im = cast_ds_value(imag);
			break;

		case ComplexType_long:
			*re = cast_ls_value(real);
			*im = cast_ls_value(imag);
			break;

		case ComplexType_rational:
			*re = single_float_rational(real);
			*im = single_float_rational(imag);
			break;

		case ComplexType_error:
		default:
			TypeError(pos, COMPLEX);
			return;
	}
}

void double_float_complex(addr pos, double_float *re, double_float *im)
{
	addr real, imag;

	CheckType(pos, LISPTYPE_COMPLEX);
	switch (getcomplexr(pos, &real, &imag)) {
		case ComplexType_single:
			*re = cast_sd_value(real);
			*im = cast_sd_value(imag);
			break;

		case ComplexType_double:
			*re = cast_dd_value(real);
			*im = cast_dd_value(imag);
			break;

		case ComplexType_long:
			*re = cast_dl_value(real);
			*im = cast_dl_value(imag);
			break;

		case ComplexType_rational:
			*re = double_float_rational(real);
			*im = double_float_rational(imag);
			break;

		case ComplexType_error:
		default:
			TypeError(pos, COMPLEX);
			return;
	}
}

void long_float_complex(addr pos, long_float *re, long_float *im)
{
	addr real, imag;

	CheckType(pos, LISPTYPE_COMPLEX);
	switch (getcomplexr(pos, &real, &imag)) {
		case ComplexType_single:
			*re = cast_sl_value(real);
			*im = cast_sl_value(imag);
			break;

		case ComplexType_double:
			*re = cast_dl_value(real);
			*im = cast_dl_value(imag);
			break;

		case ComplexType_long:
			*re = cast_ll_value(real);
			*im = cast_ll_value(imag);
			break;

		case ComplexType_rational:
			*re = long_float_rational(real);
			*im = long_float_rational(imag);
			break;

		case ComplexType_error:
		default:
			TypeError(pos, COMPLEX);
			return;
	}
}

static int zerop_call_complex(addr pos, int (*call)(addr))
{
	addr check;
	GetRealComplex(pos, &check);
	if (! call(check)) return 0;
	GetImagComplex(pos, &check);
	return call(check);
}

int zerop_complex(addr pos)
{
	Check(GetType(pos) != LISPTYPE_COMPLEX, "type error");
	switch (GetTypeComplex(pos)) {
		case ComplexType_single:
			return zerop_call_complex(pos, zerop_single_float);

		case ComplexType_double:
			return zerop_call_complex(pos, zerop_double_float);

		case ComplexType_long:
			return zerop_call_complex(pos, zerop_long_float);

		case ComplexType_rational:
			return zerop_call_complex(pos, zerop_rational);

		default:
			TypeError(pos, COMPLEX);
			return 0;
	}
}

int eql_complex(addr left, addr right)
{
	addr check1, check2;

	Check(GetType(left) != LISPTYPE_COMPLEX, "type left error");
	Check(GetType(right) != LISPTYPE_COMPLEX, "type right error");
	GetRealComplex(left, &check1);
	GetRealComplex(right, &check2);
	if (! eql_function(check1, check2)) return 0;
	GetImagComplex(left, &check1);
	GetImagComplex(right, &check2);

	return eql_function(check1, check2);
}

int equal_complex(LocalRoot local, addr left, addr right)
{
	addr check1, check2;

	Check(GetType(left) != LISPTYPE_COMPLEX, "type left error");
	Check(GetType(right) != LISPTYPE_COMPLEX, "type right error");
	GetRealComplex(left, &check1);
	GetRealComplex(right, &check2);
	if (! equal_real(local, check1, check2)) return 0;
	GetImagComplex(left, &check1);
	GetImagComplex(right, &check2);

	return equal_real(local, check1, check2);
}

int equal_fc_number(addr left, addr right)
{
	addr check;

	Check(GetType(left) != LISPTYPE_FIXNUM, "type left error");
	Check(GetType(right) != LISPTYPE_COMPLEX, "type right error");
	GetImagComplex(right, &check);
	if (! zerop_real(check)) return 0;
	GetRealComplex(right, &check);

	return equal_fixnum_real(left, right);
}

int equal_bc_number(addr left, addr right)
{
	addr check;

	Check(GetType(left) != LISPTYPE_BIGNUM, "type left error");
	Check(GetType(right) != LISPTYPE_COMPLEX, "type right error");
	GetImagComplex(right, &check);
	if (! zerop_real(check)) return 0;
	GetRealComplex(right, &check);

	return equal_bignum_real(left, right);
}

int equal_rc_number(LocalRoot local, addr left, addr right)
{
	addr check;

	Check(GetType(left) != LISPTYPE_RATIO, "type left error");
	Check(GetType(right) != LISPTYPE_COMPLEX, "type right error");
	GetImagComplex(right, &check);
	if (! zerop_real(check)) return 0;
	GetRealComplex(right, &check);

	return equal_ratio_real(local, left, right);
}

int equal_sc_number(LocalRoot local, addr left, addr right)
{
	addr check;

	Check(GetType(left) != LISPTYPE_SINGLE_FLOAT, "type left error");
	Check(GetType(right) != LISPTYPE_COMPLEX, "type right error");
	GetImagComplex(right, &check);
	if (! zerop_real(check)) return 0;
	GetRealComplex(right, &check);

	return equal_single_float_real(local, left, right);
}

int equal_dc_number(LocalRoot local, addr left, addr right)
{
	addr check;

	Check(GetType(left) != LISPTYPE_DOUBLE_FLOAT, "type left error");
	Check(GetType(right) != LISPTYPE_COMPLEX, "type right error");
	GetImagComplex(right, &check);
	if (! zerop_real(check)) return 0;
	GetRealComplex(right, &check);

	return equal_double_float_real(local, left, right);
}

int equal_lc_number(LocalRoot local, addr left, addr right)
{
	addr check;

	Check(GetType(left) != LISPTYPE_LONG_FLOAT, "type left error");
	Check(GetType(right) != LISPTYPE_COMPLEX, "type right error");
	GetImagComplex(right, &check);
	if (! zerop_real(check)) return 0;
	GetRealComplex(right, &check);

	return equal_long_float_real(local, left, right);
}

void sign_reverse_complex_common(addr pos, addr *ret)
{
	enum ComplexType type;
	addr real, imag;

	Check(GetType(pos) != LISPTYPE_COMPLEX, "type error");
	switch (getcomplex(pos, &type, &real, &imag)) {
		case ComplexType_single:
			sign_reverse_floats_heap(real, &real);
			sign_reverse_floats_heap(imag, &imag);
			break;

		case ComplexType_double:
			sign_reverse_floatd_heap(real, &real);
			sign_reverse_floatd_heap(imag, &imag);
			break;

		case ComplexType_long:
			sign_reverse_floatl_heap(real, &real);
			sign_reverse_floatl_heap(imag, &imag);
			break;

		case ComplexType_rational:
			sign_reverse_rational_common(real, &real);
			sign_reverse_rational_common(imag, &imag);
			break;

		default:
			TypeError(pos, COMPLEX);
			return;
	}
	complex_unsafe_heap(ret, real, imag, type);
}

void sign_reverse_complex_local(LocalRoot local, addr pos, addr *ret)
{
	enum ComplexType type;
	addr real, imag;

	Check(local == NULL, "local error");
	Check(GetType(pos) != LISPTYPE_COMPLEX, "type error");
	switch (getcomplex(pos, &type, &real, &imag)) {
		case ComplexType_single:
			sign_reverse_floats_local(local, real, &real);
			sign_reverse_floats_local(local, imag, &imag);
			break;

		case ComplexType_double:
			sign_reverse_floatd_local(local, real, &real);
			sign_reverse_floatd_local(local, imag, &imag);
			break;

		case ComplexType_long:
			sign_reverse_floatl_local(local, real, &real);
			sign_reverse_floatl_local(local, imag, &imag);
			break;

		case ComplexType_rational:
			sign_reverse_rational_local(local, real, &real);
			sign_reverse_rational_local(local, imag, &imag);
			break;

		default:
			TypeError(pos, COMPLEX);
			return;
	}
	complex_unsafe_local(local, ret, real, imag, type);
}


/*
 *  abs
 */
void abs_complex_common(LocalRoot local, addr pos, addr *ret)
{
	addr real, imag;
	single_float vf, v2;
	double_float vd;
	long_float vl;

	Check(local == NULL, "local error");
	CheckType(pos, LISPTYPE_COMPLEX);
	switch (getcomplexr(pos, &real, &imag)) {
		case ComplexType_single:
			cabs_f(RefSingleFloat(real), RefSingleFloat(imag), &vf);
			single_float_check_heap(ret, vf);
			break;

		case ComplexType_double:
			cabs_d(RefDoubleFloat(real), RefDoubleFloat(imag), &vd);
			double_float_check_heap(ret, vd);
			break;

		case ComplexType_long:
			cabs_l(RefLongFloat(real), RefLongFloat(imag), &vl);
			long_float_check_heap(ret, vl);
			break;

		case ComplexType_rational:
			vf = single_float_rational(real);
			v2 = single_float_rational(imag);
			single_float_check_heap(ret, sqrtf(vf*vf + v2+v2));
			break;

		default:
			TypeError(pos, COMPLEX);
			return;
	}
}

