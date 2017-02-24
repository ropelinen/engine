#ifndef DEA_ENGINE_SYSTEM_H
#define DEA_ENGINE_SYSTEM_H

DEA_START()

struct window_platform;

struct window
{
	window_platform *wnd;
	uint width;
	uint height;
};

void init_system_data();
void destroy_system_data();
void error_popup(const char *msg, const bool kill_program);
void create_window(const uint width, const uint height, const float pos_x, const float pos_y, const wchar_t *name, const bool fullscreen, window &out_wnd);
void destroy_window(window &wnd, const bool fullscreen);
void focus_window(window &wnd);
void run(pod_vector<window> &windows);

DEA_END()

#endif // DEA_ENGINE_SYSTEM_H