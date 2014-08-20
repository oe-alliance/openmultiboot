#include <stdio.h>
#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "omb_common.h"
#include "omb_log.h"

static FT_Library omb_freetype_library;
static FT_Face omb_freetype_face;
static FT_GlyphSlot omb_freetype_slot;

int omb_init_freetype()
{
	if (FT_Init_FreeType(&omb_freetype_library) != 0) {
		omb_log(LOG_ERROR, "cannot init freetype");
		return OMB_ERROR;
	}
	
	if (FT_New_Face(omb_freetype_library, OMB_FONT, 0, &omb_freetype_face) != 0) {
		omb_log(LOG_ERROR, "cannot open font");
		return OMB_ERROR;
	}
	
	if (FT_Set_Char_Size(omb_freetype_face, 50 * 64, 0, 100, 0)) {
		omb_log(LOG_ERROR, "cannot set font size");
		return OMB_ERROR;
	}
	
	omb_freetype_slot = omb_freetype_face->glyph;
}
