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

#ifndef _OMB_FREETYPE_H_
#define _OMB_FREETYPE_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#define OMB_TEXT_ALIGN_LEFT 0
#define OMB_TEXT_ALIGN_CENTER 1
#define OMB_TEXT_ALIGN_RIGHT 2

#define OMB_SYMBOL_LOGO 0x20
#define OMB_SYMBOL_ARROW_UP 0x21
#define OMB_SYMBOL_ARROW_DOWN 0x22

int omb_init_freetype();
void omb_deinit_freetype();
int omb_render_symbol(int code, int x, int y, int width, int color, int font_size, int align);
int omb_render_lcd_symbol(int code, int x, int y, int width, int color, int font_size, int align);
int omb_render_text(const char* text, int x, int y, int width, int color, int font_size, int align);
int omb_render_lcd_text(const char* text, int x, int y, int width, int color, int font_size, int align);

#endif // _OMB_FREETYPE_H_
