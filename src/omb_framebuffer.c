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
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef __sh__
#include <linux/stmfb.h>
#endif

#include "omb_common.h"
#include "omb_log.h"

#ifndef FBIO_BLIT
#define FBIO_SET_MANUAL_BLIT _IOW('F', 0x21, __u8)
#define FBIO_BLIT 0x22
#endif

#define ALPHA(x) (x >> 24) & 0xff;
#define RED(x)   (x >> 16) & 0xff;
#define GREEN(x) (x >> 8) & 0xff;
#define BLUE(x)   x & 0xff;

static int omb_fb_fd = 0;
static unsigned char* omb_fb_map = 0;
static struct fb_var_screeninfo omb_var_screen_info;
static struct fb_fix_screeninfo omb_fix_screen_info;
static int omb_screen_size;

int omb_read_screen_info()
{
	if (ioctl(omb_fb_fd, FBIOGET_FSCREENINFO, &omb_fix_screen_info) == -1) {
		omb_log(LOG_ERROR, "cannot read fixed information");
		return OMB_ERROR;
	}

	if (ioctl(omb_fb_fd, FBIOGET_VSCREENINFO, &omb_var_screen_info) == -1) {
		omb_log(LOG_ERROR, "cannot read variable information");
		return OMB_ERROR;
	}

	omb_log(LOG_DEBUG, "current mode is %dx%d, %dbpp, stride %d",
		omb_var_screen_info.xres, omb_var_screen_info.yres, omb_var_screen_info.bits_per_pixel, omb_fix_screen_info.line_length);
	
	omb_screen_size = omb_fix_screen_info.smem_len;//omb_var_screen_info.xres * omb_var_screen_info.yres * omb_var_screen_info.bits_per_pixel / 8;

#ifdef __sh__
	omb_screen_size -= 1920*1080*4;
#endif

	return OMB_SUCCESS;
}

int omb_set_screen_info(int width, int height, int bpp)
{
	omb_var_screen_info.xres_virtual = omb_var_screen_info.xres = width;
	omb_var_screen_info.yres_virtual = omb_var_screen_info.yres = height;
	omb_var_screen_info.bits_per_pixel = bpp;
	omb_var_screen_info.xoffset = omb_var_screen_info.yoffset = 0;
	omb_var_screen_info.height = 0;
	omb_var_screen_info.width = 0;
	
	if (ioctl(omb_fb_fd, FBIOPUT_VSCREENINFO, &omb_var_screen_info) < 0) {
		omb_log(LOG_ERROR, "cannot set variable information");
		return OMB_ERROR;
	}
	
	if ((omb_var_screen_info.xres != width) && (omb_var_screen_info.yres != height) && (omb_var_screen_info.bits_per_pixel != bpp)) {
		omb_log(LOG_ERROR, "cannot set variable information: got %dx%dx%d instead of %dx%dx%d",
			omb_var_screen_info.xres, omb_var_screen_info.yres, omb_var_screen_info.bits_per_pixel, width, height, bpp);
		return OMB_ERROR;
	}
	
	if (ioctl(omb_fb_fd, FBIOGET_FSCREENINFO, &omb_fix_screen_info) == -1) {
		omb_log(LOG_ERROR, "cannot read fixed information");
		return OMB_ERROR;
	}
	
	return OMB_SUCCESS;
}

int omb_map_framebuffer()
{
#ifdef __sh__
	omb_fb_map = (unsigned char *)mmap(0, omb_screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, omb_fb_fd, 1920*1080*4);
#else
	omb_fb_map = (unsigned char *)mmap(0, omb_screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, omb_fb_fd, 0);
#endif
	if (omb_fb_map == MAP_FAILED) {
		omb_log(LOG_ERROR, "failed to map framebuffer device to memory");
		return OMB_ERROR;
	}
	
	omb_log(LOG_DEBUG, "the framebuffer device was mapped to memory successfully");
	
	return OMB_SUCCESS;
}

static unsigned short red[256], green[256], blue[256], trans[256];
int omb_make_palette()
{
	int r = 8, g = 8, b = 4, i;

	struct fb_cmap colormap;
	colormap.start=0;
	colormap.len=256;
	colormap.red=red;
	colormap.green = green;
	colormap.blue = blue;
	colormap.transp=trans;

	int rs = 256 / (r - 1);
	int gs = 256 / (g - 1);
	int bs = 256 / (b - 1);

	for (i = 0; i < 256; i++) {
		colormap.red[i]   = (rs * ((i / (g * b)) % r)) * 255;
		colormap.green[i] = (gs * ((i / b) % g)) * 255;
		colormap.blue[i]  = (bs * ((i) % b)) * 255;
	}

	omb_log(LOG_DEBUG, "set color palette");
	if (ioctl(omb_fb_fd, FBIOPUTCMAP, &colormap) == -1) {
		omb_log(LOG_ERROR, "failed to set color palette");
		//return OMB_ERROR;
		return OMB_SUCCESS; // NEED TO BE FIXED FOR VU+ BOXES !!
	}
	
	return OMB_SUCCESS;
}

int omb_set_manual_blit()
{
	omb_log(LOG_DEBUG, "set manual blit");
	
#ifndef __sh__
	unsigned char tmp = 1;
	if (ioctl(omb_fb_fd, FBIO_SET_MANUAL_BLIT, &tmp)) {
		omb_log(LOG_ERROR, "failed to set manual blit");
		return OMB_ERROR;
	}
#endif
	
	return OMB_SUCCESS;
}

void omb_blit()
{
#ifdef __sh__
	STMFBIO_BLT_DATA    bltData;
	memset(&bltData, 0, sizeof(STMFBIO_BLT_DATA));
	bltData.operation  = BLT_OP_COPY;
	bltData.srcOffset  = 1920*1080*4;
	bltData.srcPitch   = omb_var_screen_info.xres * 4;
	bltData.dstOffset  = 0;
	bltData.dstPitch   = omb_var_screen_info.xres * 4;
	bltData.src_top    = 0;
	bltData.src_left   = 0;
	bltData.src_right  = omb_var_screen_info.xres;
	bltData.src_bottom = omb_var_screen_info.yres;
	bltData.srcFormat  = SURF_BGRA8888;
	bltData.dstFormat  = SURF_BGRA8888;
	bltData.srcMemBase = STMFBGP_FRAMEBUFFER;
	bltData.dstMemBase = STMFBGP_FRAMEBUFFER;
	bltData.dst_top    = 0;
	bltData.dst_left   = 0;
	bltData.dst_right  = omb_var_screen_info.xres;
	bltData.dst_bottom = omb_var_screen_info.yres;
	if (ioctl(omb_fb_fd, STMFBIO_BLT, &bltData ) < 0)
		omb_log(LOG_WARNING, "cannot blit the framebuffer");
	if (ioctl(omb_fb_fd, STMFBIO_SYNC_BLITTER) < 0)
		omb_log(LOG_WARNING, "cannot sync blit");
#else
	if (ioctl(omb_fb_fd, FBIO_BLIT) == -1)
		omb_log(LOG_WARNING, "cannot blit the framebuffer");
#endif
}

int omb_get_screen_width()
{
	return omb_var_screen_info.xres;
}

int omb_get_screen_height()
{
	return omb_var_screen_info.yres;
}

int omb_open_framebuffer()
{
	omb_fb_fd = open(OMB_FB_DEVICE, O_RDWR);
	if (omb_fb_fd == -1) {
		omb_log(LOG_ERROR, "cannot open framebuffer device");
		return OMB_ERROR;
	}
	omb_log(LOG_DEBUG, "the framebuffer device was opened successfully");
	
	if (omb_read_screen_info() == OMB_ERROR)
		return OMB_ERROR;
	
	if ((omb_var_screen_info.xres != OMB_SCREEN_WIDTH)
		|| (omb_var_screen_info.yres != OMB_SCREEN_HEIGHT)
		|| (omb_var_screen_info.bits_per_pixel != OMB_SCREEN_BPP)) {
			
		if (omb_set_screen_info(OMB_SCREEN_WIDTH, OMB_SCREEN_HEIGHT, OMB_SCREEN_BPP) == OMB_ERROR)
			return OMB_ERROR;
	
		if (omb_read_screen_info() == OMB_ERROR)
			return OMB_ERROR;
	}
	
	if (omb_map_framebuffer() == OMB_ERROR)
		return OMB_ERROR;
	
	if (omb_make_palette() == OMB_ERROR)
		return OMB_ERROR;
	
	if (omb_set_manual_blit() == OMB_ERROR)
		return OMB_ERROR;
	
	return OMB_SUCCESS;
}

void omb_close_framebuffer()
{
	munmap(omb_fb_map, omb_screen_size);
	close(omb_fb_fd);
}

void omb_clear_screen()
{
	memset(omb_fb_map, '\0', omb_screen_size);
}

void omb_draw_rect(int x, int y, int width, int height, int color)
{
	int i, j;
	long int location = 0;
	unsigned char alpha = ALPHA(color);
	unsigned char red = RED(color);
	unsigned char green = GREEN(color);
	unsigned char blue = BLUE(color);
	
	for (i = y; i < y + height; i++) {
		for (j = x; j < x + width; j++) {
			
			if (i < 0 || j < 0 || i > omb_var_screen_info.yres || j > omb_var_screen_info.xres)
				continue;

			location = ((j + omb_var_screen_info.xoffset) * (omb_var_screen_info.bits_per_pixel / 8)) +
				((i + omb_var_screen_info.yoffset) * omb_fix_screen_info.line_length);

			*(omb_fb_map + location) = blue;
			*(omb_fb_map + location + 1) = green;
			*(omb_fb_map + location + 2) = red;
			*(omb_fb_map + location + 3) = alpha;
		}
	}
}

static inline int omb_is_point_inside_circle(int x, int y, int radius)
{
	if (((x - radius) * (x - radius)) + ((y - radius) * (y - radius)) < radius * radius)
		return 1;
	return 0;
}

void omb_draw_rounded_rect(int x, int y, int width, int height, int color, int radius)
{
	int i, j;
	long int location = 0;
	unsigned char alpha = ALPHA(color);
	unsigned char red = RED(color);
	unsigned char green = GREEN(color);
	unsigned char blue = BLUE(color);
	
	for (i = y; i < y + height; i++) {
		for (j = x; j < x + width; j++) {
			if (i < 0 || j < 0 || i > omb_var_screen_info.yres || j > omb_var_screen_info.xres)
				continue;
			
			int relative_x = j - x;
			int relative_y = i - y;
			
			// top left corner
			if (relative_y < radius && relative_x < radius) {
				if (!omb_is_point_inside_circle(relative_x, relative_y, radius)) {
					continue;
				}
			}
			
			// top right corner
			else if (relative_y < radius && width - relative_x < radius) {
				if (!omb_is_point_inside_circle(width - relative_x, relative_y, radius)) {
					continue;
				}
			}
			
			// bottom left corner
			else if (height - relative_y < radius && relative_x < radius) {
				if (!omb_is_point_inside_circle(relative_x, height - relative_y, radius)) {
					continue;
				}
			}

			// bottom right corner
			else if (height - relative_y < radius && width - relative_x < radius) {
				if (!omb_is_point_inside_circle(width - relative_x, height - relative_y, radius)) {
					continue;
				}
			}

			location = ((j + omb_var_screen_info.xoffset) * (omb_var_screen_info.bits_per_pixel / 8)) +
				((i + omb_var_screen_info.yoffset) * omb_fix_screen_info.line_length);

			*(omb_fb_map + location) = blue;
			*(omb_fb_map + location + 1) = green;
			*(omb_fb_map + location + 2) = red;
			*(omb_fb_map + location + 3) = alpha;
		}
	}
}

static inline unsigned char omb_blend_pixel(unsigned char background, unsigned char foreground, unsigned char foreground_alpha)
{
	return (foreground * (foreground_alpha / 255.0)) + (background * (1.0 - (foreground_alpha / 255.0)));
}

void omb_draw_character(FT_Bitmap* bitmap, FT_Int x, FT_Int y, int color)
{
	int i, j, z = 0;
	long int location = 0;
	unsigned char red = RED(color);
	unsigned char green = GREEN(color);
	unsigned char blue = BLUE(color);
	
	for (i = y; i < y + bitmap->rows; i++) {
		for (j = x; j < x + bitmap->width; j++) {
			if (i < 0 || j < 0 || i > omb_var_screen_info.yres || j > omb_var_screen_info.xres) {
				z++;
				continue;
			}
			
			if (bitmap->buffer[z] != 0x00) {
				location = ((j + omb_var_screen_info.xoffset) * (omb_var_screen_info.bits_per_pixel / 8)) +
					((i + omb_var_screen_info.yoffset) * omb_fix_screen_info.line_length);
			
				if (*(omb_fb_map + location + 3) == 0x00) {
					*(omb_fb_map + location) = blue;
					*(omb_fb_map + location + 1) = green;
					*(omb_fb_map + location + 2) = red;
					*(omb_fb_map + location + 3) = bitmap->buffer[z];
				}
				else {
					*(omb_fb_map + location) = omb_blend_pixel(*(omb_fb_map + location), blue, bitmap->buffer[z]);
					*(omb_fb_map + location + 1) = omb_blend_pixel(*(omb_fb_map + location + 1), green, bitmap->buffer[z]);
					*(omb_fb_map + location + 2) = omb_blend_pixel(*(omb_fb_map + location + 2), red, bitmap->buffer[z]);
					if (bitmap->buffer[z] == 0xff)
						*(omb_fb_map + location + 3) = bitmap->buffer[z];
				}
			}
			z++;
		}
	}
}
