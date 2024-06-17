# Makefile for elevation statistics library
# =========================================
# Author: Angeline G. Burrell, NRL (2023)
# This is a U.S. government work and not under copyright protection in the U.S.
#
# Disclaimer: RST is licensed under GPL v3.0. Please visit 
#             <https://www.gnu.org/licenses/> to see the full license
#
# Modifications:

include $(MAKECFG).$(SYSTEM)

INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I../include/
INC = $(IPATH)/superdarn
DSTPATH = $(LIBPATH)
OUTPUT = elvstat

SRC = get_fit_elv_stat.c

OBJS = $(SRC:.c=.o)

LINK = "1"

include $(MAKELIB).$(SYSTEM)
