# Makefile for szamap
# ===================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
        -I$(IPATH)/imagery
SRC =  szamap.c   
OBJS = szamap.o
INC=$(IPATH)/imagery
LINK="1"

DSTPATH=$(LIBPATH)
OUTPUT = szamap

include $(MAKELIB).$(SYSTEM)
