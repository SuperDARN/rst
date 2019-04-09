# Makefile for fitidl library
# ===========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}

SRC=fitidl.c
OBJS = fitidl.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
ifdef IDL_IPATH
OUTPUT = fitidl
endif
LINK="1"

include $(MAKELIB).$(SYSTEM)
