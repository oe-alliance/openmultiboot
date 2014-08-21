#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>

#include "omb_common.h"
#include "omb_log.h"
#include "omb_freetype.h"
#include "omb_framebuffer.h"
#include "omb_menu.h"

static int omb_timer_enabled;
static int omb_current_timer;

void omb_draw_header()
{
	char tmp[255];
	sprintf(tmp, "%s %s", OMB_DISPLAY_NAME, OMB_APP_VERION);
	omb_render_text(tmp,
		OMB_HEADER_X,
		OMB_HEADER_Y,
		400,
		OMB_HEADER_COLOR,
		OMB_HEADER_FONT_SIZE,
		OMB_TEXT_ALIGN_LEFT);
}

void omb_draw_timer()
{
	if (omb_timer_enabled) {
		char tmp[255];
		sprintf(tmp, "%d", omb_current_timer);
		omb_render_text(tmp,
			omb_get_screen_width() - (400 + OMB_TIMER_RIGHT_MARGIN),
			OMB_TIMER_Y,
			400,
			OMB_TIMER_COLOR,
			OMB_TIMER_FONT_SIZE,
			OMB_TEXT_ALIGN_RIGHT);
	}
}

void omb_refresh_gui()
{
	omb_clear_screen();
	omb_draw_header();
	omb_draw_timer();
	omb_menu_render();
	omb_blit();
}

int main(int argc, char *argv[]) 
{
	struct timeval start, end;
	
	if (omb_open_framebuffer() == OMB_ERROR)
		return OMB_ERROR;
	
	if (omb_init_freetype() == OMB_ERROR)
		return OMB_ERROR;
	
	if (omb_input_open() == OMB_ERROR)
		return OMB_ERROR;
	
	omb_menu_init();
	
	omb_menu_add("OpenMIPS", 1);
	omb_menu_add("OpenATV", 0);
	omb_menu_add("OpenVIX", 0);
	omb_menu_add("OpenSIF", 0);
	omb_menu_add("OpenPLI", 0);
	omb_menu_add("OpenSomething", 0);
	omb_menu_add("OpenSomething2", 0);
	omb_menu_add("OpenSomething3", 0);
	omb_menu_add("OpenSomething4", 0);
	
	omb_timer_enabled = 1;
	omb_current_timer = OMB_DEFAULT_TIMER;
	gettimeofday(&start, NULL);
	
	omb_refresh_gui();
	
	for(;;) {
		usleep(10000);
		int need_refresh_gui = 0;
		int code = omb_input_get_code();
		if (code == KEY_OK)
			break;
		else if (code == KEY_UP) {
			omb_menu_prev();
			need_refresh_gui = 1;
			omb_timer_enabled = 0;
		}
		else if (code == KEY_DOWN) {
			omb_menu_next();
			need_refresh_gui = 1;
			omb_timer_enabled = 0;
		}
		
		if (omb_timer_enabled) {
			long mtime, seconds, useconds;
			gettimeofday(&end, NULL);
			
			seconds  = end.tv_sec  - start.tv_sec;
			useconds = end.tv_usec - start.tv_usec;

			mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
			int last_value = omb_current_timer;
			omb_current_timer = OMB_DEFAULT_TIMER - (mtime / 1000);
			
			if (omb_current_timer != last_value)
				need_refresh_gui = 1;
		}
		
		if (need_refresh_gui)
			omb_refresh_gui();
		
		if (omb_current_timer == 0)
			break;
	}

	omb_clear_screen();
	omb_blit();
	
	omb_input_close();
	omb_menu_deinit();
	omb_deinit_freetype();
	omb_close_framebuffer();
	return OMB_SUCCESS;
}

