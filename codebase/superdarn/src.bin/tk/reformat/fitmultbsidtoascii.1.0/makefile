# Makefile for make_fov
# =====================
# Author: Angeline G. Burrell, NRL (2021)
# This is a U.S. government work and not under copyright protection in the U.S.
#
# Disclaimer:  RST is licensed under GPL v3.0. Please visit 
#              <https://www.gnu.org/licenses/> to see the full license
#
# Modifications:


include $(MAKECFG).$(SYSTEM)

INCLUDE = -I$(IPATH)/analysis -I$(IPATH)/base -I$(IPATH)/general \
		-I$(IPATH)/superdarn -I../include/

SRC =	fitmultbsidtoascii.c

OBJS =	$(SRC:.c=.o)

INC=$(IPATH)/superdarn

DSTPATH = $(BINPATH)

OUTPUT = fitmultbsidtoascii

LIBS = -lchannel.1 -lfitmultbsid.1.0 -lrpos.1 -lfit.1 -lrscan.1 -lradar.1 \
	-ldmap.1 -lopt.1 -lrtime.1 -ligrf.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1

SLIB = -lm -lz

include $(MAKEBIN).$(SYSTEM)
