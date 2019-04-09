# base.tcsh
# =========

#####################################################################
#                                                                   #
# Compilation directives                                            #
#                                                                   #
#####################################################################

# Path of the X11 packages

setenv XPATH "/usr/X11R6"

# Compile netCDF software

setenv NETCDF_PATH "/usr/include"

# pathname for the CDF software

setenv CDF_PATH "/usr/local/cdf"

# SVG library

setenv SVGLIB "rsvg.1"

#####################################################################
#                                                                   #
# Font Data Tables                                                  #
#                                                                   #
#####################################################################

setenv FONTPATH ${RSTPATH}"/tables/base/fonts"
setenv FONTDB ${RSTPATH}"/tables/base/fonts/fontdb.xml"

