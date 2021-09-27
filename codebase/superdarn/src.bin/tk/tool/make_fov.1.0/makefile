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

# Notes:
# grid_utils, get_fit_tfreq_band, should likely be called through libraries
SRC =	make_fov.c \
	eval_groundscatter.c \
	leastsquares.c \
	linear_regression.c \
	load_fit_update_fov.c \
	load_radar_site.c \
	multbsid.c \
	scan_utils.c \
	select_alt_groups.c \
	sort_rout.c \
	stat_utils.c \
	test_ut_fov_struct.c \
	update_beam.c \
	update_scan.c \
	write_scan.c

OBJS =	$(SRC:.c=.o)

INC=$(IPATH)/superdarn

DSTPATH = $(BINPATH)

OUTPUT = make_fov

LIBS = -lchannel.1 -lfreqband.1 -loldfit.1 -lgtabw.1 -loldgtabw.1 -lgtable.1 \
	-lrpos.1 -lfilter.1 -lcfit.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 \
	-lopt.1 -lrtime.1 -lrcnv.1 -laacgm.1 -ligrf.1 -laacgm_v2.1 -ligrf_v2.1 \
	-lastalg.1 -lfitacf.1 -lfitacf.3.0 -lmpfit.1.2 -lelevation.1.0 \
	-lpropagation.1.0

SLIB = -lm -lz

include $(MAKEBIN).$(SYSTEM)
