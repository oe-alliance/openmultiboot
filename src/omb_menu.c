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

#include "omb_common.h"
#include "omb_log.h"
#include "omb_freetype.h"
#include "omb_framebuffer.h"
#include "omb_lcd.h"
#include "omb_utils.h"
#include "omb_menu.h"

static omb_device_item *omb_device_items = NULL;
static int omb_menu_offset = 0;
static int omb_menu_selected = 0;

void omb_menu_set(omb_device_item *items)
{
	omb_device_items = items;
}

omb_device_item *omb_menu_get_last()
{
	omb_device_item *tmp = omb_device_items;
	while (tmp) {
		if (!tmp->next)
			return tmp;
		
		tmp = tmp->next;
	}
	return NULL;
}

int omb_menu_count()
{
	int count = 0;
	omb_device_item *tmp = omb_device_items;
	while (tmp) {
		count++;
		tmp = tmp->next;
	}
	return count;
}

omb_device_item *omb_menu_get(int position) {
	omb_device_item *tmp = omb_device_items;
	int count = 0;
	while (tmp) {
		if (count == position)
			return tmp;
		tmp = tmp->next;
		count++;
	}
	return NULL;
}

omb_device_item *omb_menu_get_selected()
{
	return omb_menu_get(omb_menu_selected);
}

void omb_menu_set_selected(const char *identifier)
{
	omb_device_item *tmp = omb_device_items;
	int count = 0;
	while (tmp) {
		if (strcmp(tmp->identifier, identifier) == 0) {
			omb_menu_selected = count;
			break;
		}
		tmp = tmp->next;
		count++;
	}
}

void omb_menu_next()
{
	int position = omb_menu_selected;
	position++;
	if (position >= omb_menu_count())
		position = omb_menu_count() - 1;
	omb_menu_selected = position;
	
	if (position >= omb_menu_offset + OMB_MENU_MAX_ITEMS)
		omb_menu_offset = position - OMB_MENU_MAX_ITEMS + 1;
	if (omb_menu_offset < 0)
		omb_menu_offset = 0;
}

void omb_menu_prev()
{
	int position = omb_menu_selected;
	position--;
	if (position < 0)
		position = 0;
	omb_menu_selected = position;
	
	if (position < omb_menu_offset)
		omb_menu_offset = position;
}

void omb_menu_render()
{
	int i;
	int count = omb_menu_count();
	int visible_count = count < OMB_MENU_MAX_ITEMS ? count : OMB_MENU_MAX_ITEMS;
	int screen_width = omb_get_screen_width();
	int screen_height = omb_get_screen_height();
	int box_width = OMB_MENU_ITEM_WIDTH + (OMB_MENU_BOX_MARGIN * 2);
	int box_height = (OMB_MENU_ITEM_HEIGHT * visible_count) + (OMB_MENU_BOX_MARGIN * (visible_count + 1));
	int box_x = (screen_width - box_width) / 2;
	int box_y = (screen_height - box_height) / 2;
	
	omb_draw_rounded_rect(box_x, box_y, box_width, box_height, OMB_MENU_BOX_COLOR, OMB_MENU_BOX_RADIUS);
	
	if (omb_menu_offset > 0) {
		omb_render_symbol(OMB_SYMBOL_ARROW_UP,
			box_x + OMB_MENU_BOX_MARGIN,
			box_y - 70,
			OMB_MENU_ITEM_WIDTH,
			OMB_MENU_ARROWS_COLOR,
			OMB_MENU_ARROWS_SIZE,
			OMB_TEXT_ALIGN_CENTER);
	}
		
	for (i = omb_menu_offset; i < visible_count + omb_menu_offset; i++) {
		omb_device_item *item = omb_menu_get(i);
		int color = OMB_MENU_ITEM_COLOR;
		if (i == omb_menu_selected) {
#ifdef OMB_HAVE_TEXTLCD
			omb_lcd_write_text(item->label);
#else
			int selection_y = omb_lcd_get_height() * OMB_LCD_SELECTION_Y;
			int selection_size = omb_lcd_get_width() * OMB_LCD_SELECTION_SIZE;
			
			omb_render_lcd_text(item->label,
				0,
				selection_y,
				omb_lcd_get_width(),
				OMB_LCD_SELECTION_COLOR,
				selection_size,
				OMB_TEXT_ALIGN_CENTER);
#endif
			
			color = OMB_MENU_ITEM_SELECTED_COLOR;
		}
		
		omb_draw_rounded_rect(box_x + OMB_MENU_BOX_MARGIN,
			box_y + OMB_MENU_BOX_MARGIN,
			OMB_MENU_ITEM_WIDTH,
			OMB_MENU_ITEM_HEIGHT,
			color,
			OMB_MENU_ITEM_RADIUS);
			
		omb_render_text(item->label,
			box_x + OMB_MENU_BOX_MARGIN,
			box_y + OMB_MENU_BOX_MARGIN + OMB_MENU_ITEM_HEIGHT - OMB_MENU_ITEM_TEXT_BOTTON_MARGIN,
			OMB_MENU_ITEM_WIDTH,
			OMB_MENU_ITEM_TEXT_COLOR,
			OMB_MENU_ITEM_TEXT_FONT_SIZE,
			OMB_TEXT_ALIGN_CENTER);
			
		box_y += OMB_MENU_ITEM_HEIGHT + OMB_MENU_BOX_MARGIN;
	}
	
	if (omb_menu_offset + OMB_MENU_MAX_ITEMS < count) {
		omb_render_symbol(OMB_SYMBOL_ARROW_DOWN,
			box_x + OMB_MENU_BOX_MARGIN,
			box_y + 20,
			OMB_MENU_ITEM_WIDTH,
			OMB_MENU_ARROWS_COLOR,
			OMB_MENU_ARROWS_SIZE,
			OMB_TEXT_ALIGN_CENTER);
	}
}
