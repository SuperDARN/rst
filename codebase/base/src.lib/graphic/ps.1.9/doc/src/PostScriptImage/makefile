# Makefile for PostScriptImage
# ========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
IGNVER=1
OBJS = PostScriptImage.o
OUTPUT = PostScriptImage
LIBS=-lrps.1 -lrfbuf.1 -lrxml.1 
SLIB=-lm
include $(MAKELIB).$(SYSTEM)
