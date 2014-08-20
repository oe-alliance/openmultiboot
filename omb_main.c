#include <stdio.h>
#include <stdlib.h>

#include "omb_common.h"
#include "omb_log.h"

/* MAIN */

int main(int argc, char *argv[]) 
{
	if (omb_open_framebuffer() == OMB_ERROR)
		return OMB_ERROR;
	
	if (omb_init_freetype() == OMB_ERROR)
		return OMB_ERROR;
	
	/*
	int x = 0, y = 0;
	long int location = 0;
	
	for (y = 100; y < 300; y++) {
		for (x = 100; x < 300; x++) {

			location = ((x + omb_var_screen_info.xoffset) * (omb_var_screen_info.bits_per_pixel / 8)) +
				((y + omb_var_screen_info.yoffset) * omb_fix_screen_info.line_length);

			*(omb_fb_map + location) = 50;        // Some blue
			*(omb_fb_map + location + 1) = 200;      // A little green
			*(omb_fb_map + location + 2) = 50;      // A lot of red
			*(omb_fb_map + location + 3) = 200;      // No transparency
		}
	}
	*/
	omb_blit();
	
	sleep(5);
	omb_close_framebuffer();
	return OMB_SUCCESS;
}

