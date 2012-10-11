# Makefile for rcdf
# =================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/analysis -I$(CDF_PATH)/include

SRC = rcdf.c    
OBJS = rcdf.o
INC=$(IPATH)/analysis
LINK="1"
DSTPATH=$(LIBPATH)

ifdef CDF_PATH
  OUTPUT = rcdf
endif




include $(MAKELIB).$(SYSTEM)
