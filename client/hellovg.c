// first OpenVG program
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

int main() {
	int width, height;
	VGfloat font_width, font_height, font_depth;
	char s[3];
	char hello1[] = {'H','e','j',',',' ','v', 0xc3, 0xa4,'r' , 'l','d' ,'e','n',0};
	char hello2[] = {'H','e','l','l',0xc3,0xb3,' ', 'V', 'i', 'l', 0xc3,0xa1,'g',0};
	char hello3[] = {'A','h','o','j',' ','s','v',0xc4,0x95,'t','e',0};
	init(&width, &height);				   // Graphics initialization

	Start(width, height);				   // Start the picture
	Background(0, 0, 0);				   // Black background
	Fill(44, 77, 232, 1);				   // Big blue marble
	Circle(width / 2, 0, width);			   // The "world"
	Fill(255, 255, 255, 1);				   // White text
	TextMid(width / 2, (height * 0.7), "hello, world", SerifTypeface, width / 15);	// Greetings 
	TextMid(width / 2, (height * 0.5), hello1 , SerifTypeface, width / 15);
	TextMid(width / 2, (height * 0.3), hello2 , SerifTypeface, width / 15);
	TextMid(width / 2, (height * 0.1), hello3 , SerifTypeface, width / 15);
// Test clearing an area the width of an 'A' in the serif font 10pt
// First I'll draw it, then clear over the top of it so it shouldn't appear
// on the screen. Also test the colour defines.
// Red background so we can see the area cleared
	Fill(SH_red, 1);
	Rect(30, 30, 50, 50);
// Green text, draw 2 letter A, the second so we can see what it looked like
	Fill(SH_green, 1);
	Text(40, 40, "AA", SerifTypeface, 16);
	// Size the font goes below the baseline (descender)
	font_depth = TextDepth(SerifTypeface, 16);
	// Size the font goes above the baseline (height)
	font_height = TextHeight(SerifTypeface, 16);
	// Width of an 'A'
	font_width = TextWidth("A", SerifTypeface, 16);
	// Total height to clear starts from baseline-depth to depth+height
	AreaClear(40, 40-font_depth, font_width, font_depth+font_height);

	End();						   // End the picture

	fgets(s, 2, stdin);				   // look at the pic, end with [RETURN]
	finish();					   // Graphics cleanup
	exit(0);
}
