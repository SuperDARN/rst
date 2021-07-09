# Makefile for make_fov
# =====================
# Author: AGB, NRL (2021)
#
# Disclaimer: This code is licensed under GPL v3.0 please LICENSE to 
# 			  see the full license

include $(MAKECFG).$(SYSTEM)

INCLUDE = -I$(IPATH)/analysis -I$(IPATH)/base -I$(IPATH)/general \
		-I$(IPATH)/superdarn -I../include/

# Notes:
# load_fit is copied from codebase/superdarn/src.bin/tk/tool/read_fit_v1.0
SRC =	make_fov.c \
	eval_groundscatter.c \
	leastsquares.c \
	linear_regression.c \
	location.c \
	select_alt_groups.c \
	sort_rout.c \
	update_beam.c \
	update_scan.c \
	utils.c \
	load_fit.c

OBJS =	$(SRC:.c=.o)

INC=$(IPATH)/superdarn

DSTPATH = $(BINPATH)

OUTPUT = make_fov

LIBS = -loldfit.1 -lgtabw.1 -loldgtabw.1 -lgtable.1 -lrpos.1 -lfilter.1 \
	-lcfit.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 \
	-lrcnv.1 -laacgm.1 -ligrf.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1 \
	-lfitacf.1

SLIB = -lm -lz

include $(MAKEBIN).$(SYSTEM)
