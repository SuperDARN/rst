#  Makefile for geopack library
#  ============================
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis

SRC = geopackwrap.c geopack_2005.c f2c.c
OBJS= geopackwrap.o geopack_2005.o f2c.o 

INC = $(IPATH)/analysis
DSTPATH=$(LIBPATH)
OUTPUT = geopack
LINK="1"

include $(MAKELIB).$(SYSTEM)
