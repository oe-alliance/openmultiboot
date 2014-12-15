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
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "omb_common.h"
#include "omb_log.h"
#include "omb_lcd.h"

#ifndef LCD_IOCTL_ASC_MODE
#define LCDSET					0x1000
#define LCD_IOCTL_ASC_MODE		(21|LCDSET)
#define	LCD_MODE_ASC			0
#define	LCD_MODE_BIN			1
#endif

#define RED(x)   (x >> 16) & 0xff;
#define GREEN(x) (x >> 8) & 0xff;
#define BLUE(x)   x & 0xff;

static int omb_lcd_fd = -1;
static int omb_lcd_width = 0;
static int omb_lcd_height = 0;
static int omb_lcd_stride = 0;
static int omb_lcd_bpp = 0;
static unsigned char *omb_lcd_buffer = NULL;


int omb_lcd_read_value(const char *filename)
{
	int value = 0;
	FILE *fd = fopen(filename, "r");
	if (fd) {
		int tmp;
		if (fscanf(fd, "%x", &tmp) == 1)
			value = tmp;
		fclose(fd);
	}
	return value;
}

int omb_lcd_open()
{
	omb_lcd_fd = open("/dev/dbox/lcd0", O_RDWR);
	if (omb_lcd_fd == -1)
		 omb_lcd_fd = open("/dev/dbox/oled0", O_RDWR);
	if (omb_lcd_fd == -1) {
		omb_log(LOG_ERROR, "cannot open lcd device");
		return OMB_ERROR;
	}

#ifdef OMB_HAVE_TEXTLCD
	return OMB_SUCCESS;
#endif

	int tmp = LCD_MODE_BIN;
	if (ioctl(omb_lcd_fd, LCD_IOCTL_ASC_MODE, &tmp)) {
		omb_log(LOG_ERROR, "failed to set lcd bin mode");
#ifndef OMB_DREAMBOX
		return OMB_ERROR;
#endif
	}
	
	omb_lcd_width = omb_lcd_read_value(OMB_LCD_XRES);
	if (omb_lcd_width == 0) {
		omb_log(LOG_ERROR, "cannot read lcd x resolution");
		return OMB_ERROR;
	}
	
	omb_lcd_height = omb_lcd_read_value(OMB_LCD_YRES);
	if (omb_lcd_height == 0) {
		omb_log(LOG_ERROR, "cannot read lcd y resolution");
		return OMB_ERROR;
	}
	omb_lcd_bpp = omb_lcd_read_value(OMB_LCD_BPP);
	if (omb_lcd_bpp == 0) {
		omb_log(LOG_ERROR, "cannot read lcd bpp");
		return OMB_ERROR;
	}
	
	omb_lcd_stride = omb_lcd_width * (omb_lcd_bpp / 8);
	omb_lcd_buffer = malloc(omb_lcd_height * omb_lcd_stride);
	
	omb_log(LOG_DEBUG, "current lcd is %dx%d, %dbpp, stride %d", omb_lcd_width, omb_lcd_height, omb_lcd_bpp, omb_lcd_stride);

	return OMB_SUCCESS;
}

int omb_lcd_get_width()
{
	return omb_lcd_width;
}

int omb_lcd_get_height()
{
	return omb_lcd_height;
}

void omb_lcd_clear()
{
	if (!omb_lcd_buffer)
		return;
	
	memset(omb_lcd_buffer, '\0', omb_lcd_height * omb_lcd_stride);
}

void omb_lcd_update()
{
	if (!omb_lcd_buffer)
		return;
	
	write(omb_lcd_fd, omb_lcd_buffer, omb_lcd_height * omb_lcd_stride);
}

void omb_lcd_close()
{
	if (omb_lcd_fd >= 0)
		close(omb_lcd_fd);
	
	if (omb_lcd_buffer)
		free(omb_lcd_buffer);
}

void omb_lcd_draw_character(FT_Bitmap* bitmap, FT_Int x, FT_Int y, int color)
{
	if (!omb_lcd_buffer)
		return;
		
	int i, j, z = 0;
	long int location = 0;
	unsigned char red = RED(color);
	unsigned char green = GREEN(color);
	unsigned char blue = BLUE(color);
	
	red = (red >> 3) & 0x1f;
	green = (green >> 3) & 0x1f;
	blue = (blue >> 3) & 0x1f;
	
	for (i = y; i < y + bitmap->rows; i++) {
		for (j = x; j < x + bitmap->width; j++) {
			if (i < 0 || j < 0 || i > omb_lcd_height || j > omb_lcd_width) {
				z++;
				continue;
			}
			
			if (bitmap->buffer[z] != 0x00) {
				location = (j * (omb_lcd_bpp / 8)) +
					(i * omb_lcd_stride);
			
				omb_lcd_buffer[location] = red << 3 | green >> 2;
				omb_lcd_buffer[location + 1] = green << 6 | blue << 1;
			}
			z++;
		}
	}
}

void omb_lcd_write_text(const char* text)
{
	if(omb_lcd_fd < 0)
		return;

	write(omb_lcd_fd, text, strlen(text));
}
