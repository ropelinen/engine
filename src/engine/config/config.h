#ifndef DEA_ENGINE_CONFIG_H
#define DEA_ENGINE_CONFIG_H

/* Generic engine defines */

/* Renderer defines 0x40 - 0x80*/
#define DEA_RENDERER_DX12 0x40

#define DEA_RENDERER DEA_RENDERER_DX12

#if !defined(DEA_RENDERER)
#error "Renderer not defined"
#endif

#endif // DEA_ENGINE_CONFIG_H