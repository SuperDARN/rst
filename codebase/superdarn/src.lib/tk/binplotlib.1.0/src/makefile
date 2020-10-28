# Makefile for binplotlib library
# ===============================
# by E.G.Thomas
#
include $(MAKECFG).$(SYSTEM)
INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
          -I$(IPATH)/superdarn

SRC = geobeam.c key.c make_fov.c make_grid.c plot_cell.c \
      plot_logo.c plot_raw.c plot_source.c plot_time_label.c \
      plot_vec.c text_box.c

OBJS = geobeam.o key.o make_fov.o make_grid.o plot_cell.o \
       plot_logo.o plot_raw.o plot_source.o plot_time_label.o \
       plot_vec.o text_box.o

INC = $(IPATH)/superdarn
DSTPATH = $(LIBPATH)
OUTPUT = binplot
LINK="1"

include $(MAKELIB).$(SYSTEM)
