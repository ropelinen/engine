#ifndef DEA_DISABLES_MSVC_H
#define DEA_DISABLES_MSVC_H
/* Disable useless warnings */
#if defined(_MSC_VER)
#pragma warning(disable : 4115) /* named type definition in parentheses */
#pragma warning(disable : 4334) /* '<<': result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?) */
#pragma warning(disable : 4710) /* The given function was selected for inline expansion, but the compiler did not perform the inlining. */
#pragma warning(disable : 4711) /* function selected for automatic inline expansion */
#pragma warning(disable : 4820) /* x bytes padding added after data member */
#endif // _MSC:VER
#endif // DEA_DISABLES_MSVC_H