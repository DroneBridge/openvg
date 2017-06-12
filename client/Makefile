ifdef RPISDK
  CROSS_COMPILE ?= arm-linux-gnueabihf-
endif

CC = $(CROSS_COMPILE)gcc
CCOPTS = -std=c99 -Wall -Wextra -Wdouble-promotion -Wshadow

ifdef DEBUG
  CCOPTS += -O0 -g
else
  CCOPTS += -O2
endif

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILEC = $(CC) $(DEPFLAGS) $(CCOPTS)
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SRCS = shapedemo.c newfont.c chars.c hellovg.c mouse-hellovg.c particles.c \
	clip.c screensize.c ptest.c fontlist.c fonttest.c image.c

INCLUDEFLAGS = -I$(RPISDK)/opt/vc/include \
	       -I$(RPISDK)/opt/vc/include/interface/vmcs_host/linux \
	        -I$(RPISDK)/opt/vc/include/interface/vcos/pthreads -I..

LIBFLAGS = -Wl,-rpath=$(RPISDK)/opt/vc/lib -L$(RPISDK)/opt/vc/lib -lEGL \
	   -lGLESv2 -lbcm_host -pthread -ljpeg -lpng -lfreetype \
	   -lfontconfig -lz -lm

all :	chars clip dot fontlist fonttest hellovg image newfont mouse-hellovg \
	particles ptest screensize shapedemo tst

chars :		chars.o
clip :		clip.o
dot :		dot.o
fontlist :	fontlist.o
fonttest :	fonttest.o
hellovg :	hellovg.o
image :		image.o
newfont :	newfont.o
mouse-hellovg :	mouse-hellovg.o
particles :	particles.o
ptest :		ptest.o
screensize :	screensize.o
shapedemo :	shapedemo.o
tst :		tst.o

.PHONY : test indent clean

clean :
	-rm $(SRCS:.c=) $(SRCS:.c=.o)
	-rm -r .d

test:	shapedemo
	./shapedemo demo 5

indent :
	indent -linux -c 60 -brf -l 132 $(SRCS)


% : %.o ../libshapes.o ../oglinit.o ../fontsystem.o
	$(CC) $(CCOPTS) -o $@ $^ $(LIBFLAGS)

%.o : %.c
%.o : %.c $(DEPDIR)/%.d
	$(COMPILEC) $(CCOPTS) $(INCLUDEFLAGS) -c $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(pathsubs %,$(DEPDIR)/%.d,$(basename $(SRCS))))

