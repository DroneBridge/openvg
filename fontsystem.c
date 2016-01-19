#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

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

typedef struct fcoord_T {
        float x, y;
} fcoord_T;

typedef struct paths_T {
        int cpos;
        int spos;
        int max_coords;
        int max_segs;
        fcoord_T *coords;
        VGubyte *segs;
} paths_T;

static paths_T paths;

static void alloc_paths()
{
        paths.cpos = paths.spos = 0;
        paths.coords = malloc(1024 * sizeof *paths.coords);
        paths.max_coords = 1024;
        paths.segs = malloc(256 * sizeof *paths.segs);
        paths.max_segs = 256;
}

static void free_paths()
{
        if (paths.coords) {
                free(paths.coords);
                paths.coords = NULL;
        }
        if (paths.segs) {
                free(paths.segs);
                paths.segs = NULL;
        }
}

static void paths_more_coords()
{
        int size = paths.max_coords;
        paths.coords = realloc(paths.coords, (size + 256) * sizeof *paths.coords);
        if (paths.coords) {
                paths.max_coords = size + 256;
        }
}

static void paths_more_segs()
{
        int size = paths.max_segs;
        paths.segs = realloc(paths.segs, (size + 64) * sizeof *paths.segs);
        if (paths.segs) {
                paths.max_segs = size + 64;
        }
}

static inline float Fixed26ToFloat(FT_Int fix)
{
        return (float)fix / 64.0f;
}

static inline float Fixed12ToFloat(FT_Int fix)
{
        return (float)fix / 4096.0f;
}

// Functions used in decomposing freetype outlines.
static int ft_move_to(const FT_Vector *to, paths_T *outline)
{
        int spos = outline->spos++;
        int cpos = outline->cpos++;
        if (cpos >= outline->max_coords)
                paths_more_coords();
        if (spos >= outline->max_segs)
                paths_more_segs();
        outline->coords[cpos].x = Fixed12ToFloat(to->x);
        outline->coords[cpos].y = Fixed12ToFloat(to->y);
        outline->segs[spos] = VG_MOVE_TO;
        return 0;
}

static int ft_line_to(const FT_Vector *to, paths_T *outline)
{
        int spos = outline->spos++;
        int cpos = outline->cpos++;
        if (cpos >= outline->max_coords)
                paths_more_coords();
        if (spos >= outline->max_segs)
                paths_more_segs();
        outline->coords[cpos].x = Fixed12ToFloat(to->x);
        outline->coords[cpos].y = Fixed12ToFloat(to->y);
        outline->segs[spos] = VG_LINE_TO;
        return 0;
}

static int ft_conic_to(const FT_Vector *control, const FT_Vector *to,
                       paths_T *outline)
{
        int spos = outline->spos++;
        int cpos = outline->cpos;
        outline->cpos += 2;
        if (outline->cpos >= outline->max_coords)
                paths_more_coords();
        if (spos >= outline->max_segs)
                paths_more_segs();
        outline->coords[cpos].x = Fixed12ToFloat(control->x);
        outline->coords[cpos].y = Fixed12ToFloat(control->y);
        outline->coords[cpos+1].x = Fixed12ToFloat(to->x);
        outline->coords[cpos+1].y = Fixed12ToFloat(to->y);
        outline->segs[spos] = VG_QUAD_TO;
        return 0;
}

static int ft_cubic_to(const FT_Vector *ctrl1, const FT_Vector *ctrl2,
                        const FT_Vector *to, paths_T *outline)
{
        int spos = outline->spos++;
        int cpos = outline->cpos;
        outline->cpos += 3;
        if (outline->cpos >= outline->max_coords)
                paths_more_coords();
        if (spos >= outline->max_segs)
                paths_more_segs();
        outline->coords[cpos].x = Fixed12ToFloat(ctrl1->x);
        outline->coords[cpos++].y = Fixed12ToFloat(ctrl1->y);
        outline->coords[cpos].x = Fixed12ToFloat(ctrl2->x);
        outline->coords[cpos++].y = Fixed12ToFloat(ctrl2->y);
        outline->coords[cpos].x = Fixed12ToFloat(to->x);
        outline->coords[cpos].y = Fixed12ToFloat(to->y);
        outline->segs[spos] = VG_CUBIC_TO;
        return 0;
}

FT_Outline_Funcs funcs = {
        (FT_Outline_MoveTo_Func)&ft_move_to,
        (FT_Outline_LineTo_Func)&ft_line_to,
        (FT_Outline_ConicTo_Func)&ft_conic_to,
        (FT_Outline_CubicTo_Func)&ft_cubic_to,
        0, 0
};

// LoadTTFFile() loads a TTF from named file
Fontinfo LoadTTFFile(const char *filename)
{
        Fontinfo font;
	FT_Face face;
        VGfloat origin[2] = { 0.0f, 0.0f };
        VGfloat escapement[2] = { 0.0f, 0.0f };

        if (ft_library == NULL) {
                if (FT_Init_FreeType(&ft_library)) {
                        return NULL;
                }
        }

        int faceIndex = 0;
	if (FT_New_Face(ft_library, filename, faceIndex, &face)) {
		return NULL;
	}

                // TODO: Fail loading bitmap fonts for now.
        if (!FT_IS_SCALABLE(face)) {
                FT_Done_Face(face);
                return NULL;
        }
        
        font = calloc(1, sizeof *font);
        assert(font != NULL);
        
	FT_Set_Char_Size(
              face,   /* handle to face object           */
              64*64,  /* char_width in 1/64th of points  */
              0,      /* char_height in 1/64th of points */
              96,     /* horizontal device resolution    */
              96 );   /* vertical device resolution      */

        font->descender_height = face->size->metrics.descender / 64;
        font->ascender_height = face->size->metrics.ascender / 64;
        font->AutoHint = VG_FALSE;
//        font->height = face->size->metrics.height / 64;

        alloc_paths();
        FT_Long numGlyphs = face->num_glyphs;
        font->Count = numGlyphs;
        short  *cmap = malloc(numGlyphs * sizeof *cmap);
        font->CharacterMap = cmap;
        font->vgfont = vgCreateFont(0);
        
        int cc, glyphs = 0;
        for (cc = 0; cc < 32; cc++)
                cmap[cc] = -1;
        for (cc = 32; cc < numGlyphs; cc++) {
                cmap[cc] = -1;
                FT_ULong glyphIndex = FT_Get_Char_Index(face, cc);
                if (!FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_IGNORE_TRANSFORM)) {
                        escapement[0] = Fixed12ToFloat(face->glyph->advance.x);
                        if (cc == ' ') {
                                
                                cmap[cc] = glyphs++;
                                continue;
                        }

                        paths.cpos = paths.spos = 0;
                        FT_Outline *outline = &face->glyph->outline;
                        FT_Outline_Decompose(outline, &funcs, &paths);
                        VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
                                                   VG_PATH_DATATYPE_F,
                                                   1.0f, 0.0f, 0, 0,
                                                   VG_PATH_CAPABILITY_APPEND_TO);
                        vgAppendPathData(path, paths.spos, paths.segs, paths.coords);
                        vgSetGlyphToPath(font->vgfont, glyphs, path, VG_FALSE, origin, escapement);
                        vgDestroyPath(path);
                        cmap[cc] = glyphs++;
                }
        }
        free_paths();
        FT_Done_Face(face);
        return font;
}


// LoadTTF() - Loads a font given a name.
//   name is e.g. "DejaVu:monospace"
Fontinfo LoadTTF(const char *name)
{
        char *fontFile;

        fontFile = "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf";
        
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
