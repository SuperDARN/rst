# Copyright (c) University of Saskatchewan
# Author: Marina Schmidt 
#
#
# This file is part of the Radar Software Toolkit (RST).
# 
# RST is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
# 
# Modifications:
#

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
SRC=datdump.c hlpstr.h
OBJS =datdump.o
DSTPATH = $(BINPATH)
OUTPUT = datdump
LIBS= -g -lopt.1 -ldmap.1 -ldat.1 -lrcnv.1 -lrtime.1
SLIB= -lm -lz

include $(MAKEBIN).$(SYSTEM)
