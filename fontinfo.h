#ifndef OPENVG_FONTINFO_H
#define OPENVG_FONTINFO_H

#include "VG/openvg.h"

#if defined(__cplusplus)
extern "C" {
#endif
	typedef struct Fontinfo_T {
		const short *CharacterMap;
		const int *GlyphAdvances;
		int Count;
		int descender_height;
		int ascender_height;
		VGPath Glyphs[500];
	} Fontinfo_T;
        typedef struct Fontinfo_T* Fontinfo;

	extern Fontinfo SansTypeface, SerifTypeface, MonoTypeface;
#if defined(__cplusplus)
}
#endif				// OPENVG_FONTINFO_H
#endif
