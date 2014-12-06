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

#ifndef _OMB_COMMON_H_
#define _OMB_COMMON_H_

#define OMB_SUCCESS 0
#define OMB_ERROR -1

#define OMB_DEVICES_DIR "/dev"
#define OMB_FB_DEVICE "/dev/fb/0"
//#define OMB_INPUT_DEVICE "/dev/input/event0"
#define OMB_VIDEO_DEVICE "/dev/dvb/adapter0/video0"
#define OMB_LCD_DEVICE "/dev/dbox/lcd0"
#define OMB_LCD_XRES "/proc/stb/lcd/xres"
#define OMB_LCD_YRES "/proc/stb/lcd/yres"
#define OMB_LCD_BPP "/proc/stb/lcd/bpp"
//#define OMB_KERNEL_MTD "/dev/mtd2"
#define OMB_MAIN_DIR "/omb"
#define OMB_DATA_DIR "open-multiboot"
#define OMB_PLUGIN_DIR "/usr/lib/enigma2/python/Plugins/Extensions/OpenMultiboot"
#define OMB_SCREEN_WIDTH 1280
#define OMB_SCREEN_HEIGHT 720
#define OMB_SCREEN_BPP 32
#define OMB_APP_NAME "open-multiboot"
#define OMB_APP_VERION "1.0"
#define OMB_DISPLAY_NAME "openMultiboot"
#ifndef OMB_DEFAULT_TIMER
#define OMB_DEFAULT_TIMER 5
#endif
#define OMB_SHOWIFRAME_BIN "/usr/bin/showiframe"
#define OMB_VOLATILE_MEDIA_BIN "/etc/init.d/volatile-media.sh"
#define OMB_MDEV_BIN "/etc/init.d/mdev"
#define OMB_MODUTILS_BIN "/etc/init.d/modutils.sh"
#define OMB_INIT_BIN "/sbin/init"
#define OMB_SYSVINIT_BIN "/sbin/init.sysvinit"
#define OMB_CHROOT_BIN "/usr/sbin/chroot"
#define OMB_NANDDUMP_BIN "/usr/sbin/nanddump"
#define OMB_NANDWRITE_BIN "/usr/sbin/nandwrite"
#define OMB_FLASHERASE_BIN "/usr/sbin/flash_erase"
#define OMB_PYTHON_BIN "/usr/bin/python"
#define OMB_BRANDING_HELPER_BIN "/sbin/open-multiboot-branding-helper.py"

#define OMB_MENU_ITEM_RADIUS 10
#define OMB_MENU_ITEM_HEIGHT 80
#define OMB_MENU_ITEM_WIDTH 800
#ifndef OMB_MENU_ITEM_COLOR
#define OMB_MENU_ITEM_COLOR 0xFA202020
#endif
#ifndef OMB_MENU_ITEM_SELECTED_COLOR
#define OMB_MENU_ITEM_SELECTED_COLOR 0xFA404040
#endif
#ifndef OMB_MENU_ITEM_TEXT_COLOR
#define OMB_MENU_ITEM_TEXT_COLOR 0xFFFFFFFF
#endif
#define OMB_MENU_ITEM_TEXT_BOTTON_MARGIN 24
#define OMB_MENU_ITEM_TEXT_FONT_SIZE 34
#define OMB_MENU_BOX_RADIUS 10
#define OMB_MENU_BOX_MARGIN 6
#ifndef OMB_MENU_BOX_COLOR
#define OMB_MENU_BOX_COLOR 0xE0202020
#endif
#define OMB_MENU_MAX_ITEMS 5
#define OMB_MENU_ARROWS_SIZE 120
#ifndef OMB_MENU_ARROWS_COLOR
#define OMB_MENU_ARROWS_COLOR 0xFA202020
#endif

#define OMB_LCD_LOGO_X 0.1 // 10% of display width
#define OMB_LCD_LOGO_Y 0.1 // like the X axis (same margin)
#define OMB_LCD_LOGO_SIZE 0.1 // 10% of display width
#define OMB_LCD_LOGO_COLOR 0xffffffff

#define OMB_LCD_TITLE_X 0.3 // 30% of display width
#define OMB_LCD_TITLE_Y 0.19 // 19% of display width (keep proportion with x axis)
#define OMB_LCD_TITLE_SIZE 0.05 // 5% of display width
#define OMB_LCD_TITLE_COLOR 0xffffffff

#define OMB_LCD_SELECTION_Y 0.75 // 75% of display height
#define OMB_LCD_SELECTION_SIZE 0.05 // 5% of display width
#define OMB_LCD_SELECTION_COLOR 0xffffffff

#define OMB_HEADER_X 20
#define OMB_HEADER_Y 40
#define OMB_HEADER_FONT_SIZE 24
#define OMB_HEADER_COLOR 0xFFFFFFFF

#define OMB_TIMER_RIGHT_MARGIN 20
#define OMB_TIMER_Y 50
#define OMB_TIMER_FONT_SIZE 34
#define OMB_TIMER_COLOR 0xFFFFFFFF

#endif // _OMB_COMMON_H_
