#ifndef _OMB_LCD_H_
#define _OMB_LCD_H_

int omb_lcd_open();
void omb_lcd_close();
int omb_lcd_get_width();
int omb_lcd_get_height();

void omb_lcd_clear();
void omb_lcd_update();

void omb_lcd_draw_character(FT_Bitmap* bitmap, FT_Int x, FT_Int y, int color);

#endif // _OMB_LCD_H_