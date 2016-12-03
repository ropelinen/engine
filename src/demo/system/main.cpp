#include "demo/system/precompiled.h"
#include "engine/system/system.h"

DEA_START()

#if (DEA_PLATFORM == DEA_PLATFORM_WINDOWS)
#include <windows.h>

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prev_instance, PSTR cmd_line, int cmd_show)
{
	/* Just the get this to compile for now */
	(void*)hinstance;
	(void*)prev_instance;
	(void*)cmd_line;
	--cmd_show;

	const bool fullscreen = false;
	window windows[2];
	create_window(1280, 720, 0.5f, 0.5f, L"Engine", fullscreen, windows[0]);
	create_window(600, 400, 0.0f, 0.0f, L"Editor", fullscreen, windows[1]);
	focus_window(windows[0]);

	run();

	destroy_window(windows[0], fullscreen);
	destroy_window(windows[1], fullscreen);

	return 0;
}
#else
#error No Main for current platform
#endif

DEA_END()