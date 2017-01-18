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
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H

#include "omb_common.h"
#include "omb_log.h"
#include "omb_framebuffer.h"
#include "omb_lcd.h"
#include "omb_segoe_ui_font.h"
#include "omb_lcddot_font.h"
#include "omb_icomoon_font.h"
#include "omb_freetype.h"

#define MAX_GLYPHS 255

static FT_Library omb_freetype_library;
static FT_Face omb_freetype_face;
static FT_Face omb_freetype_lcd_face;
static FT_Face omb_freetype_symbols_face;
static FT_GlyphSlot omb_freetype_slot;
static FT_GlyphSlot omb_freetype_lcd_slot;
static FT_GlyphSlot omb_freetype_symbols_slot;

int omb_init_freetype(int small_lcd)
{
	if (FT_Init_FreeType(&omb_freetype_library) != 0) {
		omb_log(LOG_ERROR, "cannot init freetype");
		return OMB_ERROR;
	}

	//omb_log(LOG_DEBUG, "omb_init_freetype boxmodel: %s",omb_vumodel);
	if (strcmp(omb_vumodel,"duo2")) {
		//omb_log(LOG_DEBUG, "omb_init_freetype omb_segoe_ui_font");
		if (FT_New_Memory_Face(omb_freetype_library, (const FT_Byte*)omb_segoe_ui_font, omb_segoe_ui_font_length, 0, &omb_freetype_face) != 0) {
			omb_log(LOG_ERROR, "%-33s: cannot open base font", __FUNCTION__);
			return OMB_ERROR;
		}
		if (FT_New_Memory_Face(omb_freetype_library, (const FT_Byte*)omb_segoe_ui_font, omb_segoe_ui_font_length, 0, &omb_freetype_lcd_face) != 0) {
			omb_log(LOG_ERROR, "%-33s: cannot open base font", __FUNCTION__);
			return OMB_ERROR;
		}
	} else {
		//omb_log(LOG_DEBUG, "omb_init_freetype omb_lcddot_font");
		if (FT_New_Memory_Face(omb_freetype_library, (const FT_Byte*)omb_segoe_ui_font, omb_segoe_ui_font_length, 0, &omb_freetype_face) != 0) {
			omb_log(LOG_ERROR, "%-33s: cannot open base font", __FUNCTION__);
			return OMB_ERROR;
		}
		if (FT_New_Memory_Face(omb_freetype_library, (const FT_Byte*)omb_lcddot_font, omb_lcddot_font_length, 0, &omb_freetype_lcd_face) != 0) {
			omb_log(LOG_ERROR, "%-33s: cannot open base font", __FUNCTION__);
			return OMB_ERROR;
		}
	}

	if (FT_New_Memory_Face(omb_freetype_library, (const FT_Byte*)omb_icomoon_font, omb_icomoon_font_length, 0, &omb_freetype_symbols_face) != 0) {
		omb_log(LOG_ERROR, "cannot open symbols font");
		return OMB_ERROR;
	}
	
	omb_freetype_slot = omb_freetype_face->glyph;
	omb_freetype_lcd_slot = omb_freetype_lcd_face->glyph;
	omb_freetype_symbols_slot = omb_freetype_symbols_face->glyph;
	
	return OMB_SUCCESS;
}

void omb_deinit_freetype()
{
	FT_Done_Face(omb_freetype_face);
	FT_Done_Face(omb_freetype_lcd_face);
	FT_Done_Face(omb_freetype_symbols_face);
	FT_Done_FreeType(omb_freetype_library);
}

int omb_render_symbol(int code, int x, int y, int width, int color, int font_size, int align)
{
	if (FT_Set_Char_Size(omb_freetype_symbols_face, font_size * 64, 0, 100, 0)) {
		omb_log(LOG_ERROR, "cannot set font size");
		return OMB_ERROR;
	}
	
	if (FT_Load_Char(omb_freetype_symbols_face, code, FT_LOAD_RENDER) != 0)
		return OMB_ERROR;
	
	int offset = 0;
	if (align == OMB_TEXT_ALIGN_CENTER)
		offset = (width - omb_freetype_symbols_slot->bitmap.width) / 2;
	else if (align == OMB_TEXT_ALIGN_RIGHT)
		offset = width - omb_freetype_symbols_slot->bitmap.width;
	
	omb_draw_character(&omb_freetype_symbols_slot->bitmap, offset + x, y, color);
	
	return OMB_SUCCESS;
}

int omb_render_lcd_symbol(int code, int x, int y, int width, int color, int font_size, int align)
{
	if (FT_Set_Char_Size(omb_freetype_symbols_face, font_size * 64, 0, 100, 0)) {
		omb_log(LOG_ERROR, "cannot set font size");
		return OMB_ERROR;
	}
	
	if (FT_Load_Char(omb_freetype_symbols_face, code, FT_LOAD_RENDER) != 0)
		return OMB_ERROR;
	
	int offset = 0;
	if (align == OMB_TEXT_ALIGN_CENTER)
		offset = (width - omb_freetype_symbols_slot->bitmap.width) / 2;
	else if (align == OMB_TEXT_ALIGN_RIGHT)
		offset = width - omb_freetype_symbols_slot->bitmap.width;
	
	omb_lcd_draw_character(&omb_freetype_symbols_slot->bitmap, offset + x, y, color);
	
	return OMB_SUCCESS;
}

int omb_render_text(const char* text, int x, int y, int width, int color, int font_size, int align)
{
	int i, pen_x, pen_y;
	int num_chars = strlen(text);
	FT_Bitmap bitmaps[MAX_GLYPHS];
	FT_Vector pos[MAX_GLYPHS];
	
	if (num_chars > MAX_GLYPHS)
		num_chars = MAX_GLYPHS;
	
	pen_x = x;
	pen_y = y;

	if (FT_Set_Char_Size(omb_freetype_face, font_size * 64, 0, 100, 0)) {
		omb_log(LOG_ERROR, "cannot set font size");
		return OMB_ERROR;
	}

	for(i = 0; i < num_chars; i++) {
		if (FT_Load_Char(omb_freetype_face, text[i], FT_LOAD_RENDER) != 0)
			continue;
		
		FT_Bitmap_New(&bitmaps[i]);
		FT_Bitmap_Copy(omb_freetype_library, &omb_freetype_slot->bitmap, &bitmaps[i]);
		pos[i].x = pen_x + omb_freetype_slot->bitmap_left;
		pos[i].y = pen_y - omb_freetype_slot->bitmap_top;
		pen_x += omb_freetype_slot->advance.x >> 6;
	}
	
	int text_width = (pos[num_chars - 1].x + bitmaps[num_chars - 1].width) - pos[0].x;
		
	int offset = 0;
	if (align == OMB_TEXT_ALIGN_CENTER)
		offset = (width - text_width) / 2;
	else if (align == OMB_TEXT_ALIGN_RIGHT)
		offset = width - text_width;
	
	for(i = 0; i < num_chars; i++)
		omb_draw_character(&bitmaps[i], offset + pos[i].x, pos[i].y, color);

	return OMB_SUCCESS;
}

int omb_render_lcd_text(const char* text, int x, int y, int width, int color, int font_size, int align)
{
	int i, pen_x, pen_y;
	int num_chars = strlen(text);
	FT_Bitmap bitmaps[MAX_GLYPHS];
	FT_Vector pos[MAX_GLYPHS];
	
	if (num_chars > MAX_GLYPHS)
		num_chars = MAX_GLYPHS;
	
	pen_x = x;
	pen_y = y;

//	omb_log(LOG_DEBUG, "omb_render_lcd_text boxmodel: %s",omb_vumodel);
	if (strcmp(omb_vumodel,"duo2")) {
//		omb_log(LOG_DEBUG, "FT_Set_Char_Size");
		if (FT_Set_Char_Size(omb_freetype_lcd_face, font_size * 64, 0, 100, 0)) {
			omb_log(LOG_ERROR, "cannot set font size");
			return OMB_ERROR;
		}
	} else {
//		omb_log(LOG_DEBUG, "FT_Set_Pixel_Size");
		if (FT_Set_Pixel_Sizes(omb_freetype_lcd_face, 16, 16)){
			omb_log(LOG_ERROR, "cannot set font size");
			return OMB_ERROR;
		}
	}

	for(i = 0; i < num_chars; i++) {
		if (FT_Load_Char(omb_freetype_lcd_face, text[i], FT_LOAD_RENDER) != 0)
			continue;
		
		FT_Bitmap_New(&bitmaps[i]);
		FT_Bitmap_Copy(omb_freetype_library, &omb_freetype_lcd_slot->bitmap, &bitmaps[i]);
		pos[i].x = pen_x + omb_freetype_lcd_slot->bitmap_left;
		pos[i].y = pen_y - omb_freetype_lcd_slot->bitmap_top;
		pen_x += omb_freetype_lcd_slot->advance.x >> 6;

		//if (!strcmp(omb_vumodel,"duo2"))
		//	pen_x += 1;
	}
	
	int text_width = (pos[num_chars - 1].x + bitmaps[num_chars - 1].width) - pos[0].x;
		
	int offset = 0;
	if (align == OMB_TEXT_ALIGN_CENTER)
		offset = (width - text_width) / 2;
	else if (align == OMB_TEXT_ALIGN_RIGHT)
		offset = width - text_width;
	
	for(i = 0; i < num_chars; i++)
		omb_lcd_draw_character(&bitmaps[i], offset + pos[i].x, pos[i].y, color);

	return OMB_SUCCESS;
}
