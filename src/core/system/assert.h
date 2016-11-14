#ifndef DEA_CORE_ASSERT_H
#define DEA_CORE_ASSERT_H

#include "core/config/build_info.h"

#if (DEA_ASSERT_ENABLED == DEA_FALSE)
#define dea_assert(condition) ((void)0)
#elif (DEA_ASSERT_ENABLED == DEA_TRUE)
	#if (DEA_PLATFORM == DEA_PLATFORM_WINDOWS)
	/* Just use the default assert for now,
	 * Easier to add funtionality later if I'm already using my own assert. */
	#include <assert.h>
	#define dea_assert(condition) assert(condition)
	#else
	#error dea_assert not defined for current platform
	#endif
#else
#error DEA_ASSERT_ENABLED not defined
#endif

#endif // DEA_CORE_ASSERT_H