# Makefile for cnvmapidl library
# ==============================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}

SRC=cnvmapidl.c
OBJS = cnvmapidl.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
ifdef IDL_IPATH
OUTPUT = cnvmapidl
endif

LINK="1"
include $(MAKELIB).$(SYSTEM)

