# Makefile for read_fit
# ======================
# Author: AGB
#
include $(MAKECFG).$(SYSTEM)

INCLUDE = -I$(IPATH)/analysis -I$(IPATH)/base -I$(IPATH)/general \
		-I$(IPATH)/superdarn

SRC =	read_fit.c \
	grid_funcs.c \
	write_scan.c

OBJS =	$(SRC:.c=.o)

DSTPATH = $(BINPATH)

OUTPUT = read_fit

LIBS = -loldfit.1 -lgtabw.1 -loldgtabw.1 -lgtable.1 -lrpos.1 -lfilter.1 \
	-lcfit.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 \
	-lrcnv.1 -laacgm.1 -ligrf.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1 

SLIB = -lm -lz

include $(MAKEBIN).$(SYSTEM)
