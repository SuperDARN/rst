# Makefile for stdkey library
# ===========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

OUTPUT=stdkey
DSTPATH=$(LIBPATH)
OBJS=paltable.o
INC=$(IPATH)/general

LINK="1"
lib: cleanall make_palette stdkey
	rm -fv paltable.c
	mv -f stdkey.h $(IPATH)/general/stdkey.h

paltable.o:paltable.c

make_palette:make_palette.c
	cc -o make_palette make_palette.c -I$(IPATH)/base -lm
	./make_palette



cleanall:
	rm -fv *.o
	rm -fv paltable.c
	rm -fv make_palette

















include $(MAKELIB).$(SYSTEM)
