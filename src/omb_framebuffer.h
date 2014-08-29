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