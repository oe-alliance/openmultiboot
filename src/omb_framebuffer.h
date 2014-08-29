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

#ifndef _OMB_FRAMEBUFFER_H_
#define _OMB_FRAMEBUFFER_H_

int omb_open_framebuffer();
void omb_close_framebuffer();
void omb_blit();
void omb_clear_screen();
void omb_draw_rect(int x, int y, int width, int height, int color);
void omb_draw_rounded_rect(int x, int y, int width, int height, int color, int radius);
void omb_draw_character(FT_Bitmap* bitmap, FT_Int x, FT_Int y, int color);
int omb_get_screen_width();
int omb_get_screen_height();

#endif // _OMB_FRAMEBUFFER_H_
