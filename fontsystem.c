#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include "fontconfig/fontconfig.h"

#include "VG/openvg.h"
#include "fontinfo.h"
#include "shapes.h"

// Freetype library, set globally so I only open it once
static FT_Library ft_library = NULL;

// Fontconfig library, set globally so I only open it once
static FcConfig *fc_config = NULL;

// Passing and returning structures rather than pointers *shudders*,
// but I must to keep compatability.

// LoadTTFFile() loads a TTF from named file
Fontinfo LoadTTFFile(const char *filename)
{
        Fontinfo font;
	FT_Face face;

        printf("Looking for font file: %s\n", filename);
        
        memset(&font, 0, sizeof(Fontinfo));
        if (ft_library == NULL) {
                if (FT_Init_FreeType(&ft_library)) {
                        return font;
                }
        }
           
        int faceIndex = 0;
	if (FT_New_Face(ft_library, filename, faceIndex, &face)) {
		return font;
	}

	FT_Set_Char_Size(
              face,    /* handle to face object           */
              0,       /* char_width in 1/64th of points  */
              64*64,   /* char_height in 1/64th of points */
              96,     /* horizontal device resolution    */
              96 );   /* vertical device resolution      */
// to do...
        return NULL;
}


// LoadTTF() - Loads a font given a name.
//   name is e.g. "DejaVu:monospace"
Fontinfo LoadTTF(const char *name)
{
        char *fontFile;

        fontFile = "/usr/share/fonts/truetype/ttf-dejavu/DejaVuS";
        
        if (fc_config == NULL) {
                fc_config = FcInitLoadConfigAndFonts();
        }

        if (fc_config) {
                        // configure the search pattern, 
                FcPattern *pattern = FcNameParse((FcChar8*)name);
                FcConfigSubstitute(fc_config, pattern, FcMatchPattern);
                FcDefaultSubstitute(pattern);
                
                        // find the font
                FcResult result;
                FcPattern *font = FcFontMatch(fc_config, pattern, &result);
                if (font) {
                        FcChar8 *file = NULL;
                        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
                                fontFile = (char*)file;
                        }
                        FcPatternDestroy(font);
                }
                
                FcPatternDestroy(pattern);
        }

        return LoadTTFFile(fontFile);
}

// closeFontSystem() - Close and free data used by freetype2 &
// fontconfig if they were opened
void closeFontSystem()
{
        if (fc_config) {
                FcFini();
                fc_config = NULL;
        }

        if (ft_library != NULL) {
                FT_Done_FreeType(ft_library);
                ft_library = NULL;
        }
}
