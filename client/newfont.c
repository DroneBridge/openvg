// first OpenVG program
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

Fontinfo myfont;

int main(int argc, char *argv[]) {
	int width, height;
	VGfloat font_width, font_height, font_depth;
	char s[3];
	char hello1[] = {'H','e','j',',',' ','v', 0xc3, 0xa4,'r' , 'l','d' ,'e','n',0};
	char hello2[] = {'H','e','l','l',0xc3,0xb3,' ', 'V', 'i', 'l', 0xc3,0xa1,'g',0};
	char hello3[] = {'A','h','o','j',' ','s','v',0xc4,0x95,'t','e',0};
	setlocale(LC_CTYPE, "");
	init(&width, &height);				   // Graphics initialization

	Start(width, height);				   // Start the picture

	// Dynamically load a font, pass the font name on the command
	// line, else "helvetica" will be used (which on the RPi will
	// load "Nimbus Sans L:Regular").
	// If you know the font filename (including full path) you can
	// use LoadTTFFile(filename);
	myfont = LoadTTF(argc == 2 ? argv[1] : "helvetica");

	Background(0, 0, 0);				   // Black background
	Translate(200, 200);
	Scale(0.5, 0.5);
	Rotate(45);
	Fill(44, 77, 232, 1);				   // Big blue marble
	Circle(width / 2, 0, width);			   // The "world"
	Fill(255, 255, 255, 1);				   // White text
	Stroke(255, 0, 0, 1);
	StrokeWidth(1);
	TextMid(width / 2, (height * 0.7), "hello, world", SerifTypeface, width / 15);	// Greetings 
	TextMid(width / 2, (height * 0.5), hello1 , SerifTypeface, width / 15);
	TextMid(width / 2, (height * 0.3), hello2 , SerifTypeface, width / 15);
	TextMid(width / 2, (height * 0.1), hello3 , SerifTypeface, width / 15);

	vgLoadIdentity();
	StrokeWidth(0);

	// FontAutoHint() allows turning on/off auto-hinting of the font.
	// Mainly used for small sizes, might not have any effect.
	FontAutoHint(myfont, 1);
	Text(20, 20, "`1234567890-=qwertyuiop[]asdfghjkl;'#\\zxcvbnm,./", myfont, 24);
	Text(20, 50, "¬!\"£$%^&*()_+QWERTYUIOP{}ASDFGHJKL:@~|ZXCVBNM<>?", myfont, 24);
	End();						   // End the picture

	fgets(s, 2, stdin);				   // look at the pic, end with [RETURN]
	finish();					   // Graphics cleanup
	exit(0);
}
