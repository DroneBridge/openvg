ifdef RPISDK
	CROSS_COMPILE ?= arm-linux-gnueabihf-
endif

CC = $(CROSS_COMPILE)gcc
CCOPTS = -O2 -Wall

INCLUDEFLAGS=-I$(RPISDK)/opt/vc/include -I$(RPISDK)/opt/vc/include/interface/vmcs_host/linux -I$(RPISDK)/opt/vc/include/interface/vcos/pthreads -I..
LIBFLAGS= -Wl,-rpath=$(RPISDK)/opt/vc/lib -L$(RPISDK)/opt/vc/lib -lEGL -lGLESv2 -lbcm_host -lpthread  -ljpeg
all: shapedemo hellovg mouse-hellovg particles screensize clip

shapedemo:	shapedemo.c ../libshapes.o ../oglinit.o
	$(CC) $(CCOPTS) $(INCLUDEFLAGS) -o shapedemo shapedemo.c ../libshapes.o ../oglinit.o $(LIBFLAGS)

test:	shapedemo
	./shapedemo demo 5

#newfont: newfont.c ../libshapes.o ../oglinit.o ../fontsystem.o
#	$(CC) $(CCOPTS) $(INCLUDEFLAGS) -o  newfont newfont.c ../libshapes.o ../oglinit.o ../fontsystem.o $(LIBFLAGS) -lfreetype -lfontconfig

chars:	chars.c ../libshapes.o ../oglinit.o
	$(CC) $(CCOPTS) $(INCLUDEFLAGS) -o  chars chars.c ../libshapes.o ../oglinit.o $(LIBFLAGS)

hellovg:	hellovg.c ../libshapes.o ../oglinit.o
	$(CC) $(CCOPTS) $(INCLUDEFLAGS) -o  hellovg hellovg.c ../libshapes.o ../oglinit.o $(LIBFLAGS)

mouse-hellovg:	mouse-hellovg.c ../libshapes.o ../oglinit.o
	$(CC) $(CCOPTS) $(INCLUDEFLAGS) -o  mouse-hellovg mouse-hellovg.c ../libshapes.o ../oglinit.o $(LIBFLAGS)

particles:	particles.c ../libshapes.o ../oglinit.o
	$(CC) $(CCOPTS) $(INCLUDEFLAGS) -o  particles particles.c ../libshapes.o ../oglinit.o $(LIBFLAGS)

clip:	clip.c ../libshapes.o ../oglinit.o
	$(CC) $(CCOPTS) $(INCLUDEFLAGS) -o  clip clip.c ../libshapes.o ../oglinit.o $(LIBFLAGS)

screensize:	screensize.c ../libshapes.o ../oglinit.o
	$(CC) $(CCOPTS) $(INCLUDEFLAGS) -o  screensize screensize.c ../libshapes.o ../oglinit.o $(LIBFLAGS)

indent:
	indent -linux -c 60 -brf -l 132 shapedemo.c hellovg.c mouse-hellovg.c particles.c screensize.c
