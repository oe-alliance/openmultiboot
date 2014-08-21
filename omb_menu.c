#include <stdio.h>
#include <stdlib.h>

#include "omb_common.h"
#include "omb_log.h"
#include "omb_freetype.h"
#include "omb_framebuffer.h"

typedef struct omb_menu_item
{
	char *label;
	int selected;
	struct omb_menu_item *next;
} omb_menu_item;

static omb_menu_item *omb_menu_items = NULL;
static int omb_menu_offset = 0;

void omb_menu_init()
{
	omb_menu_items = NULL;
}

void omb_menu_deinit()
{
	omb_menu_item *tmp = omb_menu_items;
	while (tmp) {
		omb_menu_item *tmp2 = tmp;
		tmp = tmp->next;
			
		free(tmp2->label);
		free(tmp2);
	}
	omb_menu_items = NULL;
}

omb_menu_item *omb_menu_get_last()
{
	omb_menu_item *tmp = omb_menu_items;
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
	omb_menu_item *tmp = omb_menu_items;
	while (tmp) {
		count++;
		tmp = tmp->next;
	}
	return count;
}

omb_menu_item *omb_menu_get(int position) {
	omb_menu_item *tmp = omb_menu_items;
	int count = 0;
	while (tmp) {
		if (count == position)
			return tmp;
		tmp = tmp->next;
		count++;
	}
	return NULL;
}

int omb_menu_get_selected_position() {
	omb_menu_item *tmp = omb_menu_items;
	int count = 0;
	while (tmp) {
		if (tmp->selected)
			return count;
		tmp = tmp->next;
		count++;
	}
	return 0;
}

void omb_menu_add(char *label, int selected)
{
	omb_menu_item *item = malloc(sizeof(omb_menu_item));
	omb_menu_item *last_item = omb_menu_get_last();
	item->label = malloc(strlen(label) + 1);
	strcpy(item->label, label);
	item->selected = selected;
	item->next = NULL;
	if (last_item) {
		last_item->next = item;
	}
	else {
		omb_menu_items = item;
	}
}

void omb_menu_next()
{
	int position = omb_menu_get_selected_position();
	omb_menu_item *item = omb_menu_get(position);
	item->selected = 0;
	position++;
	if (position >= omb_menu_count())
		position = omb_menu_count() - 1;
	item = omb_menu_get(position);
	item->selected = 1;
	
	if (position >= omb_menu_offset + OMB_MENU_MAX_ITEMS)
		omb_menu_offset = position - OMB_MENU_MAX_ITEMS + 1;
	if (omb_menu_offset < 0)
		omb_menu_offset = 0;
}

void omb_menu_prev()
{
	int position = omb_menu_get_selected_position();
	omb_menu_item *item = omb_menu_get(position);
	item->selected = 0;
	position--;
	if (position < 0)
		position = 0;
	item = omb_menu_get(position);
	item->selected = 1;
	
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
			box_y - 50,
			OMB_MENU_ITEM_WIDTH,
			OMB_MENU_ARROWS_COLOR,
			OMB_MENU_ARROWS_SIZE,
			OMB_TEXT_ALIGN_CENTER);
	}
		
	for (i = omb_menu_offset; i < visible_count + omb_menu_offset; i++) {
		omb_menu_item *item = omb_menu_get(i);
		int color = OMB_MENU_ITEM_COLOR;
		if (item->selected)
			color = OMB_MENU_ITEM_SELECTED_COLOR;
		
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