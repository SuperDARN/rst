# makefile for elevation library
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE= -I$(IPATH)/base -I$(IPATH)/superdarn
SRC = elevation.c elevation_v2.c elev_goose.c
OBJS = elevation.o elevation_v2.o elev_goose.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT=elevation
LINK="1"

include $(MAKELIB).$(SYSTEM)

