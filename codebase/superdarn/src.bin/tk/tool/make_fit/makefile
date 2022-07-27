# Makefile for make_fit
# =====================
# Author: R.J.Barnes
#
#  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
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
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program. If not, see <https://www.gnu.org/licenses/>.
#
#Modifications:
#   2022-05-21 Emma Bland (University Centre in Svalbard) Added support for fitex & lmfit

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_fit.o

SRC=hlpstr.h errstr.h make_fit.c

DSTPATH = $(BINPATH)
OUTPUT = make_fit
LIBS=-loldraw.1 -loldfit.1 -lcfit.1 -lrscan.1 -lradar.1 -lfitacf.1 -lfitacf.3.0 -lfitacfex2.1 -lfitacfex.1 -llmfit.1 -lelevation.1 -lraw.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 -lrmath.1 -lmpfit.1
SLIB=-lm -lz -lpthread
 

include $(MAKEBIN).$(SYSTEM)
