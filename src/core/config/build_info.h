#ifndef DEA_CORE_BUILD_INFO_H
#define DEA_CORE_BUILD_INFO_H

/* A bunch of defines based on the current build configuration and platform */

/* Platform defines 0x2 - 0x20 */
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
#define DEA_BUILD_TYPE RPLNN_DEBUG
#elif defined (CONF_RELEASE)
#define DEA_BUILD_TYPE DEA_RELEASE
#elif defined (CONF_PRODUCTION)
#define DEA_BUILD_TYPE DEA_PRODUCTION
#else
#error "Build configuration not defined"
#endif

#endif // DEA_CORE_BUILD_INFO_H