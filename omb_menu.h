#ifndef _OMB_MENU_H_
#define _OMB_MENU_H_

int omb_menu_count();
void omb_menu_set(omb_device_item *items);
omb_device_item *omb_menu_get_selected();
void omb_menu_next();
void omb_menu_prev();
void omb_menu_set_selected(const char *identifier);
void omb_menu_render();

#endif // _OMB_MENU_H_