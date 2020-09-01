# Makefile for sndidl library
# ===========================
# by E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}

SRC=sndidl.c
OBJS = sndidl.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
ifdef IDL_IPATH
OUTPUT = sndidl
endif
LINK="1"

include $(MAKELIB).$(SYSTEM)
