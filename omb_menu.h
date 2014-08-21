#ifndef _OMB_MENU_H_
#define _OMB_MENU_H_

void omb_menu_init();
void omb_menu_deinit();
int omb_menu_count();
void omb_menu_add(char *label, int selected);
void omb_menu_next();
void omb_menu_prev();
void omb_menu_render();

#endif // _OMB_MENU_H_