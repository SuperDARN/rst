# Makefile for binplotlib library
# ===============================
# by E.G.Thomas
#
include $(MAKECFG).$(SYSTEM)
INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
          -I$(IPATH)/superdarn

SRC = plot_logo.c plot_source.c text_box.c

OBJS = plot_logo.o plot_source.o text_box.o

INC = $(IPATH)/superdarn
DSTPATH = $(LIBPATH)
OUTPUT = binplot
LINK="1"

include $(MAKELIB).$(SYSTEM)
