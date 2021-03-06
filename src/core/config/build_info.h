#ifndef DEA_CORE_BUILD_INFO_H
#define DEA_CORE_BUILD_INFO_H

#include "core/config/config.h"

/* A bunch of defines based on the current build configuration and platform */

/* Platform defines 0x2 - 0x20 */
#define DEA_PLATFORM_WINDOWS 0x2

#if defined (_WIN32) || defined(_WIN64)
#define DEA_PLATFORM DEA_PLATFORM_WINDOWS
#else
#error "Platform not defined" 
#endif

/* Build configuration 0x100 - 0x110 */
#define DEA_DEBUG 0x100
#define DEA_RELEASE 0x105
#define DEA_PRODUCTION 0x110

#if defined (CONF_DEBUG)
#define DEA_BUILD_TYPE DEA_DEBUG
#elif defined (CONF_RELEASE)
#define DEA_BUILD_TYPE DEA_RELEASE
#elif defined (CONF_PRODUCTION)
#define DEA_BUILD_TYPE DEA_PRODUCTION
#else
#error "Build configuration not defined"
#endif

/* Anything that is directly based on the current congiguration */
#if (DEA_BUILD_TYPE == DEA_DEBUG)
#define DEA_ASSERT_ENABLED DEA_TRUE
#elif (DEA_BUILD_TYPE == DEA_RELEASE)
#define DEA_ASSERT_ENABLED DEA_TRUE
#elif (DEA_BUILD_TYPE == DEA_PRODUCTION)
#define DEA_ASSERT_ENABLED DEA_FALSE
#else
#error Current unknown build type
#endif

#ifndef DEA_ASSERT_ENABLED
#error Assert enabled macro not defined
#endif

#if (DEA_BUILD_TYPE == DEA_PRODUCTION)
#define DEA_MEMORY_DEBUGGING DEA_FALSE
#else
#define DEA_MEMORY_DEBUGGING DEA_TRUE
#endif

#ifndef DEA_MEMORY_DEBUGGING
#error Memory debugging macro not defined
#endif

#endif // DEA_CORE_BUILD_INFO_H