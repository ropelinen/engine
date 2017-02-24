#include "engine/config/precompiled.h"

#if (DEA_PLATFORM == DEA_PLATFORM_WINDOWS)
#include "engine/system/system.h"

#include <Windows.h>

DEA_START()

struct window_platform
{
	HWND hwnd;
	LPCWSTR name;
};

/* Translation unit locals */
namespace
{
	void close_windows(pod_vector<window> &windows, pod_vector<HWND> &windows_to_close)
	{
		for (auto window_to_close = windows_to_close.get_begin(); window_to_close != windows_to_close.get_end(); ++window_to_close)
		{
			for (auto window = windows.get_begin(); window != windows.get_end(); ++window)
			{
				if (*window_to_close == window->wnd->hwnd)
				{
					destroy_window(*window, false);
					windows.remove_it(window);

					break;
				}
			}
		}

		windows_to_close.clear();
	}

	/* Could use a better implementation */
	pod_vector<HWND> *windows_to_close = NULL;

	LRESULT CALLBACK wnd_proc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
	{
		/* Unused parametres, can't get rid of these */
		(void *)hwnd;
		(void *)wparam;
		(void *)lparam;

		switch (umessage)
		{
		case WM_DESTROY:
		{
			/* We don't want to do anything as we manually destroy windows */
			return 0;
		}
		case WM_CLOSE:
		{
			if (windows_to_close)
			{
				windows_to_close->push_back(hwnd);
			}
			else
			{
				/* TODO: Localization/Error number */
				error_popup("Error closing a window", false);
			}

			return 0;
		}
		default:
		{
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
		}
	}
}

void init_system_data()
{
	/* TODO: Memory manager */
	windows_to_close = new pod_vector<HWND>(1);
}

void destroy_system_data()
{
	/* TODO: Memory manager */
	if (windows_to_close)
		delete windows_to_close;
}

void error_popup(const char *msg, const bool kill_program)
{
	dea_assert(msg && "error_popup: msg is NULL");

	MessageBoxA(NULL, msg, kill_program ? "Fatal Error" : "Error", MB_OK | MB_ICONERROR);
	if (kill_program)
		ExitProcess(~(UINT)0);
}

/* TODO: Create a string class for the name */
void create_window(const uint width, const uint height, const float pos_x, const float pos_y, const wchar_t *name, const bool fullscreen, window &out_wnd)
{
	int position_x = 0;
	int position_y = 0;

	HINSTANCE hinstance = GetModuleHandle(NULL);

	/* TODO: Memory manager */
	out_wnd.wnd = new window_platform;
	out_wnd.wnd->hwnd = NULL;
	out_wnd.wnd->name = name;
	out_wnd.width = width;
	out_wnd.height = height;

	/* Setup the windows class with default settings */
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = wnd_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = name;

	// Register the window class.
	if (RegisterClassEx(&wc) == 0)
	{
		/* TODO: Localization/Error number */
		error_popup("Failed to register window class", true);
	}

	if (!fullscreen)
	{
		position_x = (int)((GetSystemMetrics(SM_CXSCREEN) - width) * pos_x);
		position_y = (int)((GetSystemMetrics(SM_CYSCREEN) - height) * pos_y);
	}
	else
	{
		DEVMODE screen_settings;
		memset(&screen_settings, 0, sizeof(screen_settings));
		screen_settings.dmSize = sizeof(screen_settings);
		screen_settings.dmPelsHeight = (unsigned long)height;
		screen_settings.dmPelsWidth = (unsigned long)width;
		screen_settings.dmBitsPerPel = 32;
		screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);
	}

	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	DWORD exStyle = 0;

	// Create the window with the screen settings and get the handle to it.
	out_wnd.wnd->hwnd = CreateWindowEx(exStyle,
	                                  name,
	                                  name,
	                                  style,
	                                  position_x, position_y, 
	                                  (int)width, (int)height, 
	                                  NULL, 
	                                  NULL, 
	                                  hinstance, 
	                                  NULL);

	if (out_wnd.wnd->hwnd == NULL)
	{
		/* TODO: Localization/Error number */
		error_popup("Failed to create a window", true);
	}

	ShowWindow(out_wnd.wnd->hwnd, SW_SHOW);
}

void destroy_window(window &wnd, const bool fullscreen)
{
	dea_assert(wnd.wnd && "destroy_window: Platform specific window pointer is NULL");

	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(wnd.wnd->hwnd);
	wnd.wnd->hwnd = NULL;

	HINSTANCE hinstance = GetModuleHandle(NULL);
	UnregisterClass(wnd.wnd->name, hinstance);
	hinstance = NULL;

	/* TODO: Memory manager */
	delete wnd.wnd;
}

void focus_window(window &wnd)
{
	dea_assert(wnd.wnd && wnd.wnd->hwnd);
	SetForegroundWindow(wnd.wnd->hwnd);
	SetFocus(wnd.wnd->hwnd);
}

void run(pod_vector<window> &windows)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	bool running = true;
	while (running)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			running = false;
		}
		else
		{
			if (windows_to_close && !windows_to_close->empty())
			{
				close_windows(windows, *windows_to_close);

				if (windows.empty())
					running = false;
			}

			/* Run frame */	
		}
	}
}

DEA_END()

#endif // DEA_PLATFORM == DEA_PLATFORM_WINDOWS