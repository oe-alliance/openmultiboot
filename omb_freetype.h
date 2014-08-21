#ifndef _OMB_FREETYPE_H_
#define _OMB_FREETYPE_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#define OMB_TEXT_ALIGN_LEFT 0
#define OMB_TEXT_ALIGN_CENTER 1
#define OMB_TEXT_ALIGN_RIGHT 2

#define OMB_SYMBOL_ARROW_UP 0x20
#define OMB_SYMBOL_ARROW_DOWN 0x21

int omb_init_freetype();
void omb_deinit_freetype();
int omb_render_symbol(int code, int x, int y, int width, int color, int font_size, int align);
int omb_render_text(const char* text, int x, int y, int width, int color, int font_size, int align);

#endif // _OMB_FREETYPE_H_