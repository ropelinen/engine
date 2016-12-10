#ifndef DEA_CORE_ASSERT_H
#define DEA_CORE_ASSERT_H

#include "core/config/build_info.h"

#if (DEA_ASSERT_ENABLED == DEA_FALSE)
#define dea_assert(condition) ((void)0)
#define dea_static_assert(condition) ((void)0)
#define dea_is_pod_assert(type) ((void)0)
#define dea_is_not_pod_assert(type) ((void)0)
#elif (DEA_ASSERT_ENABLED == DEA_TRUE)
	#if (DEA_PLATFORM == DEA_PLATFORM_WINDOWS)
	/* Just use the default assert for now,
	 * Easier to add funtionality later if I'm already using my own assert. */
	#include <assert.h>
	#include <type_traits>
	#define dea_assert(condition) assert(condition)
	#define dea_static_assert(condition) static_assert(condition, "Static: "#condition)
	#define dea_is_pod_assert(type) dea_static_assert(std::is_trivially_copyable<type>::value)
	#define dea_is_not_pod_assert(type) dea_static_assert(!std::is_trivially_copyable<type>::value)
	#else
	#error dea_assert not defined for current platform
	#endif
#else
#error DEA_ASSERT_ENABLED not defined
#endif

#endif // DEA_CORE_ASSERT_H