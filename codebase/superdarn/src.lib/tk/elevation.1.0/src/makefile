# makefile for elevation library
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE= -g -I$(IPATH)/base -I$(IPATH)/superdarn
SRC = elevation.c elevation_v2.c elev_goose.c elevation_error.c
OBJS = elevation.o elevation_v2.o elev_goose.o elevation_error.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT=elevation
LINK="1"

include $(MAKELIB).$(SYSTEM)

