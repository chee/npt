#include <time.h>
#include "build.h"
#include "condition.h"
#include "constant.h"
#include "define.h"
#include "environment.h"
#include "format.h"
#include "function.h"
#include "hashtable.h"
#include "integer.h"
#include "object.h"
#include "rational.h"
#include "real_truncate.h"
#include "pathname.h"
#include "strtype.h"
#include "symbol.h"
#include "unicode.h"

/*
 *  decode-universal-time
 */
static void decode_universal_time_zone(LocalRoot local,
		struct universal_time_struct *u, addr pos, addr zone)
{
	addr temp;
	size_t value;

	minus_rational_common(local, pos, zone, &pos);
	if (rationalp(pos))
		truncate1_common(local, &pos, &temp, pos);
	if (! getindex_integer(pos, &value)) {
		u->second = intsizeh(value % 60);
		value /= 60;
		u->minute = intsizeh(value % 60);
		value /= 60;
		u->hour = intsizeh(value % 24);
		value /= 24;
		fmte("TODO", NULL);
	}
	u->zone = zone;
	fmte("TODO", NULL);
}

/* universal time: 1900/01/01
 *    struct time: 1970/01/01
 *  1900-1970 -> 17 leap-days
 *  universal-time(1900) -> 0
 *  universal-time(1970)
 *   -> (70[year]*365 + 17)[day] * 24[hour] * 60[min] * 60[sec]
 *      (* (+ (* 70 365) 17) 24 60 60)
 *      -> 2208988800
 */
static void decode_universal_time_nil(LocalRoot local,
		struct universal_time_struct *u, addr pos)
{
	fmte("TODO", NULL);
}

_g void decode_universal_time(LocalRoot local,
		struct universal_time_struct *u, addr pos, addr zone)
{
	if (zone == Nil)
		decode_universal_time_nil(local, u, pos);
	else
		decode_universal_time_zone(local, u, pos, zone);
}


/*
 *  disassemble
 */
static void disassemble_code(addr stream, addr pos)
{
	addr code;

	CheckType(pos, LISPTYPE_FUNCTION);
	GetFunction(pos, &code);
	if (code == Nil)
		return;
	CheckType(code, LISPTYPE_CODE);

}

_g void disassemble_common(addr stream, addr pos)
{
	CheckType(stream, LISPTYPE_STREAM);
	CheckType(pos, LISPTYPE_FUNCTION);
	if (compiled_function_p(pos)) {
		fmts(stream, "COMPILED-FUNCTION.~%");
	}
	else {
		fmts(stream, "INTERPRETED-FUNCTION.~%");
		disassemble_code(stream, pos);
	}
}


/*
 *  lisp-implementation-type
 */
_g void implementation_type_common(addr *ret)
{
	strvect_char_heap(ret, LISPNAME);
}


/*
 *  lisp-implementation-version
 */
_g void implementation_version_common(addr *ret)
{
	char buffer[256];

	snprintf(buffer, 256, "%d.%d.%d [%s]",
			LISP_VERSION_A, LISP_VERSION_B, LISP_VERSION_C, LISP_REVISION);
	strvect_char_heap(ret, buffer);
}


/*
 *  short-site-name
 */
#if defined(LISP_POSIX)
#include <sys/utsname.h>
_g void short_site_name_common(addr *ret)
{
	struct utsname data;

	if (uname(&data))
		*ret = Nil;
	else
		string8_null_heap(ret, data.nodename);
}
#elif defined(LISP_WINDOWS)
static void windows_site_name(addr *ret, COMPUTER_NAME_FORMAT format)
{
	BOOL check;
	WCHAR *data;
	DWORD size;
	LocalRoot local;
	LocalStack stack;

	size = 0;
	(void)GetComputerNameExW(format, NULL, &size);
	local = Local_Thread;
	push_local(local, &stack);
	data = (WCHAR *)lowlevel_local(local, size);
	check = GetComputerNameExW(format, NULL, &size);
	if (check == 0)
		*ret = Nil;
	else
		string16_size_heap(ret, (const byte16 *)data, (size_t)size);
	rollback_local(local, stack);
}
_g void short_site_name_common(addr *ret)
{
	windows_site_name(ret, ComputerNameNetBIOS);
}
#else
_g void short_site_name_common(addr *ret)
{
	*ret = Nil;
}
#endif


/*
 *  long-site-name
 */
#if defined(LISP_POSIX)
#include <sys/utsname.h>
_g void long_site_name_common(addr *ret)
{
	short_site_name_common(ret);
}
#elif defined(LISP_WINDOWS)
_g void long_site_name_common(addr *ret)
{
	windows_site_name(ret, ComputerNamePhysicalDnsFullyQualified);
}
#else
_g void long_site_name_common(addr *ret)
{
	*ret = Nil;
}
#endif


/*
 *  machine-instance
 */
_g void machine_instance_common(addr *ret)
{
	short_site_name_common(ret);
}


/*
 *  machine-type
 */
#if defined(LISP_POSIX)
_g void machine_type_common(addr *ret)
{
	struct utsname data;

	if (uname(&data))
		*ret = Nil;
	else
		string8_null_heap(ret, data.machine);
}
#elif defined(LISP_WINDOWS)
_g void machine_type_common(addr *ret)
{
	const char *str;
	SYSTEM_INFO info;

	GetNativeSystemInfo(&info);
	/* winnt.h */
	switch (info.wProcessorArchitecture) {
		case 0: str = "INTEL"; break;
		case 1: str = "MIPS"; break;
		case 2: str = "ALPHA"; break;
		case 3: str = "PPC"; break;
		case 4: str = "SHX"; break;
		case 5: str = "ARM"; break;
		case 6: str = "IA64"; break;
		case 7: str = "ALPHA64"; break;
		case 8: str = "MSIL"; break;
		case 9: str = "AMD64"; break;
		case 10: str = "IA32-ON-WIN64"; break;
		case 11: str = "NEUTRAL"; break;
		case 12: str = "ARM64"; break;
		case 13: str = "ARM32-ON-WIN64"; break;
		case 14: str = "IA32-ON-ARM64"; break;
		default: str = NULL; break;
	}
	if (str == NULL)
		*ret = Nil;
	else
		strvect_char_heap(ret, str);
}
#else
_g void machine_type_common(addr *ret)
{
	*ret = Nil;
}
#endif


/*
 *  machine-version
 */
#if defined(LISP_POSIX)
_g void machine_version_common(addr *ret)
{
	machine_type_common(ret);
}
#elif defined(LISP_WINDOWS)
_g void machine_version_common(addr *ret)
{
	const char *str;
	SYSTEM_INFO info;

	GetNativeSystemInfo(&info);
	/* winnt.h */
	switch (info.dwProcessorType) {
		case 386: str = "INTEL-386"; break;
		case 486: str = "INTEL-486"; break;
		case 586: str = "INTEL-PENTIUM"; break;
		case 2200: str = "INTEL-IA64"; break;
		case 8664: str = "AMD-X8664"; break;
		case 4000: str = "MIPS-R4000"; break;
		case 21064: str = "ALPHA-21064"; break;
		case 601: str = "PPC-601"; break;
		case 603: str = "PPC-603"; break;
		case 604: str = "PPC-604"; break;
		case 620: str = "PPC-620"; break;
		case 10003: str = "HITACHI-SH3"; break;
		case 10004: str = "HITACHI-SH3E"; break;
		case 10005: str = "HITACHI-SH4"; break;
		case 821: str = "MOTOROLA-821"; break;
		case 103: str = "SHx-SH3"; break;
		case 104: str = "SHx-SH4"; break;
		case 2577: str = "STRONGARM"; break;
		case 1824: str = "ARM720"; break;
		case 2080: str = "ARM820"; break;
		case 2336: str = "ARM920"; break;
		case 70001: str = "ARM-7TDMI"; break;
		case 0x494f: str = "OPTIL"; break;
		default: str = NULL; break;
	}
	if (str == NULL)
		*ret = Nil;
	else
		strvect_char_heap(ret, str);
}
#else
_g void machine_version_common(addr *ret)
{
	*ret = Nil;
}
#endif


/*
 *  software-type
 */
#if defined(LISP_POSIX)
_g void software_type_common(addr *ret)
{
	struct utsname data;

	if (uname(&data))
		*ret = Nil;
	else
		string8_null_heap(ret, data.sysname);
}
#elif defined(LISP_WINDOWS)
_g void software_type_common(addr *ret)
{
	strvect_char_heap(ret, "Windows");
}
#else
_g void software_type_common(addr *ret)
{
	*ret = Nil;
}
#endif


/*
 *  software-version
 */
#if defined(LISP_POSIX)
_g void software_version_common(addr *ret)
{
	struct utsname data;

	if (uname(&data))
		*ret = Nil;
	else
		string8_null_heap(ret, data.release);
}
#elif defined(LISP_WINDOWS)
_g void software_version_common(addr *ret)
{
	BOOL (*local_GetFileVersionInfo)(LPCSTR, DWORD, DWORD, LPVOID);
	DWORD (*local_GetFileVersionInfoSize)(LPCSTR, LPDWORD);
	BOOL (*local_VerQueryValue)(LPCVOID, LPCSTR, LPVOID *, PUINT);
	BOOL check;
	HMODULE version_dll;
	FARPROC call;
	DWORD size, handle;
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *translate;
	void *ptr;
	char buffer[256];
	const char *pversion;
	UINT usize;

	/* VERSION.DLL */
	version_dll = LoadLibraryA("VERSION.DLL");
	if (version_dll == NULL) {
		goto error_dll;
	}

	/* GetFileVersionInfo */
	call = GetProcAddress(version_dll, "GetFileVersionInfoA");
	if (call == NULL) {
		goto error_dll;
	}
	local_GetFileVersionInfo = (BOOL (*)(LPCSTR, DWORD, DWORD, LPVOID))call;

	/* GetFileVersionInfoSize */
	call = GetProcAddress(version_dll, "GetFileVersionInfoSizeA");
	if (call == NULL) {
		goto error_dll;
	}
	local_GetFileVersionInfoSize = (DWORD (*)(LPCSTR, LPDWORD))call;

	/* VerQueryValue */
	call = GetProcAddress(version_dll, "VerQueryValueA");
	if (call == NULL) {
		goto error_dll;
	}
	local_VerQueryValue = (BOOL (*)(LPCVOID, LPCSTR, LPVOID *, PUINT))call;

	/* size */
	handle = 0;
	size = local_GetFileVersionInfoSize("KERNEL32.DLL", &handle);
	if (size == 0) {
		goto error_dll;
	}

	/* version */
	ptr = malloc(size);
	if (ptr == NULL) {
		goto error_dll;
	}
	check = local_GetFileVersionInfo("KERNEL32.DLL", 0, size, ptr);
	if (check == 0) {
		goto error_dll;
	}

	/* query */
	check = local_VerQueryValue(ptr,
			"\\VarFileInfo\\Translation",
			(LPVOID *)&translate, &usize);
	if (check == 0) {
		goto error_dll;
	}
	snprintf(buffer, 256, "\\StringFileInfo\\%04x%04x\\ProductVersion",
			translate->wLanguage, translate->wCodePage);
	check = local_VerQueryValue(ptr, buffer, (LPVOID *)&pversion, &usize);
	if (check == 0) {
		goto error_dll;
	}

	/* result */
	string8_null_heap(ret, pversion);
	free(ptr);
	FreeLibrary(version_dll);
	return;

error_dll:
	free(ptr);
	if (version_dll)
		FreeLibrary(version_dll);
	*ret = Nil;
}

#else
_g void software_version_common(addr *ret)
{
	*ret = Nil;
}
#endif


/*
 *  user-homedir-pathname
 */
static void default_user_homedir_pathname_common(Execute ptr, addr *ret)
{
	parse_pathname_char_heap(ptr, ".", ret);
}

static void user_homedir_pathname_string(LocalRoot local, addr pos, addr *ret)
{
	unicode c;
	addr one;
	size_t size, i;

	string_length(pos, &size);
	if (size == 0)
		goto no_update;
	string_getc(pos, size - 1, &c);
	if (c == '/' || c == '\\')
		goto no_update;
	strvect_local(local, &one, size + 1);
	for (i = 0; i < size; i++) {
		string_getc(pos, i, &c);
		strvect_setc(one, i, c);
	}
#ifdef LISP_WINDOWS
	strvect_setc(one, i, '\\');
#else
	strvect_setc(one, i, '/');
#endif
	*ret = one;
	return;

no_update:
	*ret = pos;
}

_g void user_homedir_pathname_common(Execute ptr, addr *ret)
{
	addr pos;
	LocalRoot local;
	LocalStack stack;

	GetConst(SYSTEM_SPECIAL_ENVIRONMENT, &pos);
	getspecialcheck_local(ptr, pos, &pos);
	if (pos == Nil)
		goto error;
	if (! findvalue_char_hashtable(pos, "HOME", &pos))
		goto error;
	/* /home/name -> /home/name/ */
	local = ptr->local;
	push_local(local, &stack);
	user_homedir_pathname_string(local, pos, &pos);
	physical_pathname_heap(ptr, pos, ret);
	rollback_local(local, stack);
	return;

error:
	default_user_homedir_pathname_common(ptr, ret);
}

