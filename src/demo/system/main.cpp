#include "demo/system/precompiled.h"

#if (DEA_PLATFORM == DEA_PLATFORM_WINDOWS)
#include <windows.h>
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	/* Just the get this to compile for now */
	(void*)hInstance;
	(void*)hPrevInstance;
	(void*)pScmdline;
	--iCmdshow;

	return 0;
}