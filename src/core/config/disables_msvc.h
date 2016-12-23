#ifndef DEA_CORE_DISABLES_MSVC_H
#define DEA_CORE_DISABLES_MSVC_H

/* Disable useless warnings */
#if defined(_MSC_VER)
	#pragma warning(disable : 4115) /* named type definition in parentheses */
	#pragma warning(disable : 4334) /* '<<': result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?) */
	#pragma warning(disable : 4514) /* unreferenced inline function has been removed, from cstdlib */
	#pragma warning(disable : 4577) /* 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed, from  <new> */
	#pragma warning(disable : 4668) /* is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'. Comes from windows headers */
	#pragma warning(disable : 4710) /* The given function was selected for inline expansion, but the compiler did not perform the inlining. */
	#pragma warning(disable : 4711) /* function selected for automatic inline expansion */
	#pragma warning(disable : 4820) /* x bytes padding added after data member */
#endif // _MSC_VER

#endif // DEA__CORE_DISABLES_MSVC_H