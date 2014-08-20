#include <stdio.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "omb_common.h"
#include "omb_log.h"

#ifndef FBIO_BLIT
#define FBIO_SET_MANUAL_BLIT _IOW('F', 0x21, __u8)
#define FBIO_BLIT 0x22
#endif

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
	omb_fb_map = (unsigned char *)mmap(0, omb_screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, omb_fb_fd, 0);
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
		return OMB_ERROR;
	}
	
	return OMB_SUCCESS;
}

int omb_set_manual_blit()
{
	omb_log(LOG_DEBUG, "set manual blit");
	
	unsigned char tmp = 1;
	if (ioctl(omb_fb_fd, FBIO_SET_MANUAL_BLIT, &tmp)) {
		omb_log(LOG_ERROR, "failed to set manual blit");
		return OMB_ERROR;
	}
	
	return OMB_SUCCESS;
}

void omb_blit()
{
	if (ioctl(omb_fb_fd, FBIO_BLIT) == -1)
		omb_log(LOG_WARNING, "cannot blit the framebuffer");
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
		&& (omb_var_screen_info.yres != OMB_SCREEN_HEIGHT)
		&& (omb_var_screen_info.bits_per_pixel != OMB_SCREEN_BPP)) {
			
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