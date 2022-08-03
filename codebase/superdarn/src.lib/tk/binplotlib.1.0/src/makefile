# Makefile for binplotlib library
# ===============================
# by E.G.Thomas
# Copyright (C) <year>  <name of author>
#
#This file is part of the Radar Software Toolkit (RST).
#
#RST is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#Modifications:
include $(MAKECFG).$(SYSTEM)
INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
          -I$(IPATH)/superdarn

SRC = geobeam.c key.c make_fov.c make_grid.c plot_cell.c \
      plot_logo.c plot_raw.c plot_source.c plot_time.c \
	  plot_time_label.c plot_vec.c text_box.c

OBJS = geobeam.o key.o make_fov.o make_grid.o plot_cell.o \
       plot_logo.o plot_raw.o plot_source.o plot_time.o \
	   plot_time_label.o plot_vec.o text_box.o

INC = $(IPATH)/superdarn
DSTPATH = $(LIBPATH)
OUTPUT = binplot
LINK="1"

include $(MAKELIB).$(SYSTEM)
