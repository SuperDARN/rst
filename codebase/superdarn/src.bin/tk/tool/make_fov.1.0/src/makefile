# Makefile for read_fit
# ======================
# Copyright (C) 2019 <insitution or authors name>
# Author: AGB
#
# Disclaimer: This code is licensed under GPL v3.0 please LICENSE to 
# 			  see the full license 
#
# Modified:
# =========
#  Oct-18-2019 Marina Schmidt Added INC variable
include $(MAKECFG).$(SYSTEM)

INCLUDE = -I$(IPATH)/analysis -I$(IPATH)/base -I$(IPATH)/general \
		-I$(IPATH)/superdarn -I../include/

SRC =	make_fov.c

OBJS =	$(SRC:.c=.o)

INC=$(IPATH)/superdarn

DSTPATH = $(BINPATH)

OUTPUT = read_fit

LIBS = -loldfit.1 -lgtabw.1 -loldgtabw.1 -lgtable.1 -lrpos.1 -lfilter.1 \
	-lcfit.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 \
	-lrcnv.1 -laacgm.1 -ligrf.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1 

SLIB = -lm -lz

include $(MAKEBIN).$(SYSTEM)
