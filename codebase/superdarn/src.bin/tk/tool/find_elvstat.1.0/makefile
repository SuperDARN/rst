# Makefile for find_elvstat
# =========================
# Author: Angeline G. Burrell, NRL (2023)
# This is a U.S. government work and not under copyright protection in the U.S.
#
# Disclaimer:  RST is licensed under GPL v3.0. Please visit 
#              <https://www.gnu.org/licenses/> to see the full license
#
# Modifications:
#

include $(MAKECFG).$(SYSTEM)

INCLUDE = -I$(IPATH)/analysis -I$(IPATH)/base -I$(IPATH)/general \
                -I$(IPATH)/superdarn -I../include/

# Notes:
# load_fit is copied from codebase/superdarn/src.bin/tk/tool/read_fit_v1.0
SRC =   find_elvstat.c

OBJS =  $(SRC:.c=.o)

INC=$(IPATH)/superdarn

DSTPATH = $(BINPATH)

OUTPUT = find_elvstat

LIBS =  -lchannel.1 -lelvstat.1 -loldfit.1 -lgtabw.1 -loldgtabw.1 -lgtable.1 \
	-lrpos.1 -lfilter.1 -lcfit.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 \
	-lopt.1 -lrtime.1 -lrcnv.1 -laacgm.1 -ligrf.1 -laacgm_v2.1 \
	-ligrf_v2.1 -lastalg.1 -lfitacf.1

SLIB = -lm -lz

include $(MAKEBIN).$(SYSTEM)
