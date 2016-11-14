#ifndef DEA_CORE_CONFIG_H
#define DEA_CORE_CONFIG_H

/* Generic defines */
#define DEA_TRUE  0x8080
#define DEA_FALSE 0x8000

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#endif

#define DEA_START() namespace dea {
#define DEA_END() } // namespace dea

#endif // DEA_CORE_CONFIG_H