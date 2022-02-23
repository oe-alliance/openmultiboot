/*
 *
 * Copyright (C) 2014 Impex-Sat Gmbh & Co.KG
 * Written by Sandro Cavazzoni <sandro@skanetwork.com>
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

#include "omb_common.h"
#include "omb_log.h"
#include "omb_freetype.h"
#include "omb_framebuffer.h"
#include "omb_lcd.h"
#include "omb_input.h"
#include "omb_utils.h"
#include "omb_menu.h"

static int omb_timer_enabled;
static int omb_current_timer;
static int omb_timer;
char omb_vumodel[63];

void omb_draw_header()
{
	char tmp[255];
	sprintf(tmp, "%s %s", OMB_DISPLAY_NAME, OMB_APP_VERION);
	omb_render_symbol(OMB_SYMBOL_LOGO,
		OMB_HEADER_X,
		OMB_HEADER_Y - 25,
		400,
		OMB_HEADER_COLOR,
		OMB_HEADER_FONT_SIZE,
		OMB_TEXT_ALIGN_LEFT);
	
	omb_render_text(tmp,
		OMB_HEADER_X + 45,
		OMB_HEADER_Y,
		400,
		OMB_HEADER_COLOR,
		OMB_HEADER_FONT_SIZE,
		OMB_TEXT_ALIGN_LEFT);
}

void omb_draw_lcd()
{
	char tmp[255];
	sprintf(tmp, "%s %s", OMB_DISPLAY_NAME, OMB_APP_VERION);
	
	int logo_x = omb_lcd_get_width() * OMB_LCD_LOGO_X;
	int logo_y = omb_lcd_get_height() * OMB_LCD_LOGO_Y;
	int logo_size = omb_lcd_get_width() * OMB_LCD_LOGO_SIZE;
	
	int title_x = omb_lcd_get_width() * OMB_LCD_TITLE_X;
	int title_y = omb_lcd_get_height() * OMB_LCD_TITLE_Y;
	int title_size = omb_lcd_get_width() * OMB_LCD_TITLE_SIZE;

	if (! strcmp(omb_vumodel,""))
	omb_render_lcd_symbol(OMB_SYMBOL_LOGO,
		logo_x,
		logo_y,
		0,
		OMB_LCD_LOGO_COLOR,
		logo_size,
		OMB_TEXT_ALIGN_LEFT);
	else {
		if (! strcmp(omb_vumodel,"duo2"))
			title_y += 2;

		sprintf(tmp, "VU+ %s %s", OMB_DISPLAY_NAME, OMB_APP_VERION);
		title_x = logo_x;
	}
	omb_render_lcd_text(tmp,
		title_x,
		title_y,
		0,
		OMB_LCD_TITLE_COLOR,
		title_size,
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
	omb_lcd_clear();
	
	omb_draw_lcd();
	omb_draw_header();
	omb_draw_timer();
	omb_menu_render();
	
	omb_blit();
	omb_lcd_update();
}

int omb_show_menu()
{
	struct timeval start, end;
	
	if (omb_open_framebuffer() == OMB_ERROR)
		return OMB_ERROR;
	
	if (omb_init_freetype() == OMB_ERROR)
		return OMB_ERROR;
	
	if (omb_input_open() == OMB_ERROR)
		return OMB_ERROR;
	
	omb_lcd_open();
	
	omb_timer_enabled = 1;
	omb_timer = omb_utils_gettimer();
	omb_current_timer = omb_timer;
	gettimeofday(&start, NULL);
	
	omb_refresh_gui();
	
	for(;;) {
		usleep(20000);
		int need_refresh_gui = 0;
		int code = omb_input_get_code();
		if (code == KEY_OK)
			break;
		else if (code == KEY_UP) {
			omb_menu_prev();
			omb_utils_update_background(omb_menu_get_selected());
			need_refresh_gui = 1;
			omb_timer_enabled = 0;
		}
		else if (code == KEY_DOWN) {
			omb_menu_next();
			omb_utils_update_background(omb_menu_get_selected());
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
			omb_current_timer = omb_timer - (mtime / 1000);
			
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
	
	omb_lcd_clear();
	omb_lcd_update();
	
	omb_lcd_close();
	omb_input_close();
	omb_deinit_freetype();
	omb_close_framebuffer();
	
	return OMB_SUCCESS;
}

int main(int argc, char *argv[]) 
{
	int is_rebooting = 0;

	if (argc > 1 && getppid() > 1) {
		omb_utils_sysvinit(NULL, argv[1]);
	}
	else {
		omb_vumodel[0] = '\0';

		omb_utils_init_system();
		omb_device_item *item = NULL;
		omb_device_item *items = NULL;
		char *selected = NULL;
		char *nextboot = NULL;
		if (omb_utils_find_and_mount() == OMB_SUCCESS) {
			items = omb_utils_get_images();
			omb_menu_set(items);
			selected = omb_utils_read(OMB_SETTINGS_SELECTED);
			if (!selected) {
				selected = malloc(sizeof(OMB_SETTINGS_FLASH));
				strcpy(selected, OMB_SETTINGS_FLASH);
			}
			omb_menu_set_selected(selected);
			item = omb_menu_get_selected();
		}
/*
 * by Meo. load_modules moved !
 */
		omb_utils_prepare_destination(item);

		int lock_menu = omb_utils_check_lock_menu();
		int force = omb_utils_read_int(OMB_SETTINGS_FORCE);
		if (!force && items) 
		{
			omb_log(LOG_DEBUG, "%-33s: preparing environment...", __FUNCTION__);
			if (!lock_menu) {
				omb_log(LOG_DEBUG, "%-33s: loading modules...", __FUNCTION__);
				omb_utils_load_modules(item);
				if (!omb_utils_file_exists(OMB_VIDEO_DEVICE)) {
					omb_utils_load_modules_gl(item);
				}
				omb_utils_setrctype();
			}
			omb_utils_update_background(item);
			omb_utils_backup_kernel(item);

			nextboot = omb_utils_read(OMB_SETTINGS_NEXTBOOT);
			if (nextboot) {
				omb_menu_set_selected(nextboot);
				omb_utils_remove_nextboot();
				item = omb_menu_get_selected();
				omb_utils_update_background(item);
				free(nextboot);
			}
			
			if (!lock_menu) {
				omb_log(LOG_DEBUG, "%-33s: menu enabled", __FUNCTION__);
				FILE *fvu = fopen("/proc/stb/info/vumodel", "r");
				if (fvu) {
					char tmp[63];
					if (fscanf(fvu, "%s", &tmp) == 1) {
						strcpy(omb_vumodel, tmp);
					}
					fclose(fvu);
				}
				omb_log(LOG_DEBUG, "%-33s: boxmodel: %s", __FUNCTION__, omb_vumodel);
				omb_show_menu();
			} else {
				omb_log(LOG_DEBUG, "%-33s: menu disabled", __FUNCTION__);
			}
		}
		else {
			omb_log(LOG_DEBUG, "%-33s: omb_utils_save_int(OMB_SETTINGS_FORCE, 0)", __FUNCTION__);
			omb_utils_save_int(OMB_SETTINGS_FORCE, 0);
		}

		item = omb_menu_get_selected();
		if ((item && selected && strcmp(selected, item->identifier)) != 0 || (item && strstr(item->identifier, "vti") && !force)) {
			omb_utils_restore_kernel(item);
			omb_utils_save(OMB_SETTINGS_SELECTED, item->identifier);
			omb_utils_save_int(OMB_SETTINGS_FORCE, 1);
			omb_utils_umount(OMB_MAIN_DIR);
			omb_utils_reboot();
			is_rebooting = 1;
		}
		
		if (!is_rebooting) {
			if (item != NULL && strcmp(item->identifier, OMB_SETTINGS_FLASH) != 0)
				omb_utils_remount_media(item);
			omb_utils_umount(OMB_MAIN_DIR);
			omb_utils_sysvinit(item, NULL);
		}

		if (items) omb_utils_free_items(items);
		if (selected) free(selected);
	}

	return OMB_SUCCESS;
}

