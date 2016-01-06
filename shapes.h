#include <VG/openvg.h>
#include <VG/vgu.h>
#include "fontinfo.h"
#if defined(__cplusplus)
extern "C" {
#endif
	extern void Translate(VGfloat, VGfloat);
	extern void Rotate(VGfloat);
	extern void Shear(VGfloat, VGfloat);
	extern void Scale(VGfloat, VGfloat);
	extern void Text(VGfloat, VGfloat, char *, Fontinfo, int);
	extern void TextMid(VGfloat, VGfloat, char *, Fontinfo, int);
	extern void TextEnd(VGfloat, VGfloat, char *, Fontinfo, int);
	extern VGfloat TextWidth(char *, Fontinfo, int);
	extern void Cbezier(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat);
	extern void Qbezier(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat);
	extern void Polygon(VGfloat *, VGfloat *, VGint);
	extern void Polyline(VGfloat *, VGfloat *, VGint);
	extern void Rect(VGfloat, VGfloat, VGfloat, VGfloat);
	extern void Line(VGfloat, VGfloat, VGfloat, VGfloat);
	extern void Roundrect(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat);
	extern void Ellipse(VGfloat, VGfloat, VGfloat, VGfloat);
	extern void Circle(VGfloat, VGfloat, VGfloat);
	extern void Arc(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat);
	extern void Image(VGfloat, VGfloat, int, int, char *);
	extern void Start(int, int);
	extern void End();
	extern void SaveEnd(char *);
	extern void Background(unsigned int, unsigned int, unsigned int);
	extern void BackgroundRGB(unsigned int, unsigned int, unsigned int, VGfloat);
	extern void init(int *, int *);
	extern void finish();
	extern void setfill(VGfloat[4]);
	extern void setstroke(VGfloat[4]);
	extern void StrokeWidth(VGfloat);
	extern void Stroke(unsigned int, unsigned int, unsigned int, VGfloat);
	extern void Fill(unsigned int, unsigned int, unsigned int, VGfloat);
	extern void RGBA(unsigned int, unsigned int, unsigned int, VGfloat, VGfloat[4]);
	extern void RGB(unsigned int, unsigned int, unsigned int, VGfloat[4]);
	extern void FillLinearGradient(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat *, int);
	extern void FillRadialGradient(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat *, int);
	extern void ClipRect(VGint x, VGint y, VGint w, VGint h);
	extern void ClipEnd();
	extern Fontinfo loadfont(const int *, const int *, const unsigned char *, const int *, const int *, const int *,
				 const short *, int);
	extern void unloadfont(VGPath *, int);
	extern void makeimage(VGfloat, VGfloat, int, int, VGubyte *);
	extern void saveterm();
	extern void restoreterm();
	extern void rawterm();

	// Added by Paeryn
	extern void initWindowSize(int x, int y, unsigned int w, unsigned int h);
    
	extern VGfloat TextHeight(Fontinfo f, int pointsize);
	extern VGfloat TextDepth(Fontinfo f, int pointsize);

        extern void AreaClear(unsigned int x, unsigned int y,
                              unsigned int w, unsigned int h);
	extern void WindowClear();
	extern void WindowOpacity(unsigned int alpha);
	extern void WindowPosition(int x, int y);
	extern void CbezierOutline(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat);
	extern void QbezierOutline(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat);
	extern void RectOutline(VGfloat, VGfloat, VGfloat, VGfloat);
	extern void RoundrectOutline(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat);
	extern void EllipseOutline(VGfloat, VGfloat, VGfloat, VGfloat);
	extern void CircleOutline(VGfloat, VGfloat, VGfloat);
	extern void ArcOutline(VGfloat, VGfloat, VGfloat, VGfloat, VGfloat, VGfloat);
#if defined(__cplusplus)
}
#endif

/* Colour name defines for use in above functions (not very useful outside
 * as they are 3 comma separated integers).
 */

#define SH_aliceblue		240,248,255
#define SH_antiquewhite		250,235,215
#define SH_aqua 		0,255,255
#define SH_aquamarine		127,255,212
#define SH_azure		240,255,255
#define SH_beige		245,245,220
#define SH_bisque		255,228,196
#define SH_black		0,0,0
#define SH_blanchedalmond	255,235,205
#define SH_blue			0,0,255
#define SH_blueviolet		138,43,226
#define SH_brown		165,42,42
#define SH_burlywood		222,184,135
#define SH_cadetblue		95,158,160
#define SH_chartreuse		127,255,0
#define SH_chocolate		210,105,30
#define SH_coral		255,127,80
#define SH_cornflowerblue	100,149,237
#define SH_cornsilk		255,248,220
#define SH_crimson		220,20,60
#define SH_cyan			0,255,255
#define SH_darkblue		0,0,139
#define SH_darkcyan		0,139,139
#define SH_darkgoldenrod	184,134,11
#define SH_darkgray		169,169,169
#define SH_darkgreen		0,100,0
#define SH_darkgrey		169,169,169
#define SH_darkkhaki		189,183,107
#define SH_darkmagenta		139,0,139
#define SH_darkolivegreen	85,107,47
#define SH_darkorange		255,140,0
#define SH_darkorchid		153,50,204
#define SH_darkred		139,0,0
#define SH_darksalmon		233,150,122
#define SH_darkseagreen		143,188,143
#define SH_darkslateblue	72,61,139
#define SH_darkslategray	47,79,79
#define SH_darkslategrey	47,79,79
#define SH_darkturquoise	0,206,209
#define SH_darkviolet		148,0,211
#define SH_deeppink		255,20,147
#define SH_deepskyblue		0,191,255
#define SH_dimgray		105,105,105
#define SH_dimgrey		105,105,105
#define SH_dodgerblue		30,144,255
#define SH_firebrick		178,34,34
#define SH_floralwhite		255,250,240
#define SH_forestgreen		34,139,34
#define SH_fuchsia		255,0,255
#define SH_gainsboro		220,220,220
#define SH_ghostwhite		248,248,255
#define SH_gold			255,215,0
#define SH_goldenrod		218,165,32
#define SH_gray			128,128,128
#define SH_green		0,128,0
#define SH_greenyellow		173,255,47
#define SH_grey			128,128,128
#define SH_honeydew		240,255,240
#define SH_hotpink		255,105,180
#define SH_indianred		205,92,92
#define SH_indigo		75,0,130
#define SH_ivory		255,255,240
#define SH_khaki		240,230,140
#define SH_lavender		230,230,250
#define SH_lavenderblush	255,240,245
#define SH_lawngreen		124,252,0
#define SH_lemonchiffon		255,250,205
#define SH_lightblue		173,216,230
#define SH_lightcoral		240,128,128
#define SH_lightcyan		224,255,255
#define SH_lightgoldenrodyellow	250,250,210
#define SH_lightgray		211,211,211
#define SH_lightgreen		144,238,144
#define SH_lightgrey		211,211,211
#define SH_lightpink		255,182,193
#define SH_lightsalmon		255,160,122
#define SH_lightseagreen	32,178,170
#define SH_lightskyblue		135,206,250
#define SH_lightslategray	119,136,153
#define SH_lightslategrey	119,136,153
#define SH_lightsteelblue	176,196,222
#define SH_lightyellow		255,255,224
#define SH_lime			0,255,0
#define SH_limegreen		50,205,50
#define SH_linen		250,240,230
#define SH_magenta		255,0,255
#define SH_maroon		128,0,0
#define SH_mediumaquamarine	102,205,170
#define SH_mediumblue		0,0,205
#define SH_mediumorchid		186,85,211
#define SH_mediumpurple		147,112,219
#define SH_mediumseagreen	60,179,113
#define SH_mediumslateblue	123,104,238
#define SH_mediumspringgreen	0,250,154
#define SH_mediumturquoise	72,209,204
#define SH_mediumvioletred	199,21,133
#define SH_midnightblue		25,25,112
#define SH_mintcream		245,255,250
#define SH_mistyrose		255,228,225
#define SH_moccasin		255,228,181
#define SH_navajowhite		255,222,173
#define SH_navy			0,0,128
#define SH_oldlace		253,245,230
#define SH_olive		128,128,0
#define SH_olivedrab		107,142,35
#define SH_orange		255,165,0
#define SH_orangered		255,69,0
#define SH_orchid		218,112,214
#define SH_palegoldenrod	238,232,170
#define SH_palegreen		152,251,152
#define SH_paleturquoise	175,238,238
#define SH_palevioletred	219,112,147
#define SH_papayawhip		255,239,213
#define SH_peachpuff		255,218,185
#define SH_peru			205,133,63
#define SH_pink			255,192,203
#define SH_plum			221,160,221
#define SH_powderblue		176,224,230
#define SH_purple		128,0,128
#define SH_red			255,0,0
#define SH_rosybrown		188,143,143
#define SH_royalblue		65,105,225
#define SH_saddlebrown		139,69,19
#define SH_salmon		250,128,114
#define SH_sandybrown		244,164,96
#define SH_seagreen		46,139,87
#define SH_seashell		255,245,238
#define SH_sienna		160,82,45
#define SH_silver		192,192,192
#define SH_skyblue		135,206,235
#define SH_slateblue		106,90,205
#define SH_slategray		112,128,144
#define SH_slategrey		112,128,144
#define SH_snow			255,250,250
#define SH_springgreen		0,255,127
#define SH_steelblue		70,130,180
#define SH_tan			210,180,140
#define SH_teal			0,128,128
#define SH_thistle		216,191,216
#define SH_tomato		255,99,71
#define SH_turquoise		64,224,208
#define SH_violet		238,130,238
#define SH_wheat		245,222,179
#define SH_white		255,255,255
#define SH_whitesmoke		245,245,245
#define SH_yellow		255,255,0
#define SH_yellowgreen		154,205,50
