# Makefile for solve_model
# ========================
# Author: E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)


INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
          -I$(IPATH)/superdarn
SRC = hlpstr.h solve_model.c errstr.h
OBJS = solve_model.o

DSTPATH = $(BINPATH)
OUTPUT = solve_model

SLIB = -lm -lz
LIBS = -lcnvmodel.1 -lshf.1 -laacgm.1 -lmlt.1 -lrtime.1 -lopt.1 -lrcnv.1 \
       -lmlt_v2.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1

include $(MAKEBIN).$(SYSTEM)
