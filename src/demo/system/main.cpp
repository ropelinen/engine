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

	init_system_data();

	const bool fullscreen = false;
	pod_vector<window> windows;
	{
		window temp;
		windows.resize(2, temp);
		create_window(1280, 720, 0.5f, 0.5f, L"Engine", fullscreen, windows[0]);
		create_window(600, 400, 0.0f, 0.0f, L"Editor", fullscreen, windows[1]);
	}
	focus_window(windows[0]);

	run(windows);

	for (auto it = windows.get_begin(); it != windows.get_end(); ++it)
		destroy_window(*it, fullscreen);

	destroy_system_data();

	return 0;
}
#else
#error No Main for current platform
#endif

DEA_END()