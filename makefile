# (C) Copyright 2021 SuperDARN Canada, University of Saskatchewan
# Author(s): Marina Schmidt
# 
#
lib_analysis = $(wildcard ./codebase/analysis/src.lib/*/*/src/)
lib_base_rmxl = $(wildcard ./codebase/base/src.lib/*rxml*/*/src/)
lib_base = $(wildcard ./codebase/base/src.lib/*/*/src/)
lib_general = $(wildcard ./codebase/general/src.lib/*/src/)
lib_imagery = $(wildcard ./codebase/imagery/src.lib/*/src/)
lib_superdarn = $(wildcard ./codebase/superdarn/src.lib/tk/*/src/)

libraries := $(lib_base) $(lib_base_rxml) $(lib_general) $(lib_analysis) $(lib_imagery) $(lib_superdarn)

bin_analysis = $(wildcard ./codebase/analysis/src.bin/*/*/)
bin_base = $(wildcard ./codebase/base/src.bin/*/*/)
bin_general = $(wildcard ./codebase/general/src.bin/*/*/)
bin_superdarn_tool = $(wildcard ./codebase/superdarn/src.bin/tk/tool/*/)
bin_superdarn_testing_cmp = $(wildcard ./codebase/superdarn/src.bin/tk/testing/cmp*/)
bin_superdarn_testing = $(wildcard ./codebase/superdarn/src.bin/tk/testing/test*/)
bin_superdarn_tcpip = $(wildcard ./codebase/superdarn/src.bin/tk/tcpip/*/)
bin_superdarn_reformat = $(wildcard ./codebase/superdarn/src.bin/tk/reformat/*/)
bin_superdarn_plot = $(wildcard ./codebase/superdarn/src.bin/tk/plot/*/)



bin_superdarn := $(bin_superdarn_tool) $(bin_superdarn_testing_cmp) $(bin_superdarn_testing) $(bin_superdarn_tcpip) $(bin_superdarn_reformat) $(bin_superdarn_plot)
rst := $(bin_base) $(bin_analysis) $(bin_general) $(bin_superdarn) 

.PHONY: all $(rst) $(libraries)
all: $(rst)

$(rst) $(libraries):
	$(MAKE) --directory=$@

$(rst): $(libraries)

$(lib_base): $(lib_base_rxml)
$(lib_general): $(lib_base)
$(lib_analysis): $(lib_general)
$(lib_superdarn): $(lib_base) $(lib_analysis) $(lib_general) $(lib_imagery)

