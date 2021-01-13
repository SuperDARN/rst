<!--
(C) copyright 2019 SuperDARN Canada, University of Saskatchewan
author: Marina Schmidt, SuperDARN Canada
-->
# Radar Software Toolkit (RST)

RST is an open source software package (licensed under GPL v3.0) that allows researchers to analyze, model, and quickly visualize the SuperDARN project data.

## RST source code 

The RST is released as a zip archive on [Zenodo](https://doi.org/10.5281/zenodo.801458) with a citable doi. 

The software is maintained by the SuperDARN Data Analysis Working Group (DAWG) on [GitHub](https://github.com/SuperDARN/rst). Bug reports, comments and suggestions can be provided by submitting an [issue](https://github.com/SuperDARN/rst/issues) on Github.

## Table of Contents 
  - Installation
	* [Linux](user_guide/linux_install.md)
	* [MacOSX](user_guide/mac_install.md)
  - SuperDARN Data:
    * [Accessing Data](user_guide/data.md)
    * [Citing Data](user_guide/citing.md)
    * [Filename Formats](references/general/filename.md)
    * [Radar Identifier](references/general/radar_id.md)
  -  RST Tutorials 
	  - Data Processing
       * [RAWACF to FITACF](user_guide/make_fit.md)
	     * [FITACF to GRID](user_guide/make_grid.md)
	     * [GRID to MAP](user_guide/map_grid.md)
	  - Plotting Data
         * [Range-time Plots](user_guide/time_plot.md)
         * [Field of View Plots](user_guide/fov_plot.md)
         * [Field Plots](user_guide/field_plot.md)
         * [Grid Plots](user_guide/grid_plot.md)
         * [Convection Plots](user_guide/map_plot.md)
         * [Customizing Colors](user_guide/colors.md)
  - Developers Guide
    - [Copyrights and Licensing](developers_guide/copyright_license.md) 
    - Data Structures in C
         * [cFit](developers_guide/cfit_struct_c.md)
         * [FitACF](developers_guide/fitacf_struct_c.md)
         * [Grid](developers_guide/grid_struct_c.md)
         * [IQDat](developers_guide/iqdat_struct_c.md)
         * [Map (cnvmap)](developers_guide/cnvmap_struct_c.md)
         * [Radar Parameter Block](developers_guide/radar_parameter_c.md)
         * [RawACF](developers_guide/rawacf_struct_c.md)
    - Data Structures in IDL
         * [FitACF](developer_guide/fitacf_struct_idl.md)
         * [Grid](developers_guide/grid_struct_idl.md)
         * [IQDat](developers_guide/iqdat_struct_idl.md)
         * [Map (cnvmap)](developers_guide/cnvmap_struct_idl.md)
         * [Radar Parameter Block](developers_guide/radar_parameter_idl.md)
         * [RawACF](developers_guide/rawacf_struct_idl.md)

  - SuperDARN Data Formats
    * [Dmap Data](references/general/dmap_data.md)
    * [IQDAT Format](references/general/iqdat.md)
    * [RAWACF Format](references/general/rawacf.md)
    * [FITACF Format](references/general/fitacf.md)
    * [cFit Format](references/general/cfit.md)
    * [GRID Format](references/general/grid.md)
  - Legacy Software
    * [rPlot Libraries](legacy_software/rPlot_lib.md)
    * [rPlot XML](legacy_software/rPlot_XML.md)
