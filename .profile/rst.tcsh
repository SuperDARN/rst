# rst.tcsh
# ========

#####################################################################
#                                                                   #
# Directory Paths                                                   #
#                                                                   #
#####################################################################

setenv BUILD "${RSTPATH}/build"
setenv CODEBASE "${RSTPATH}/codebase"
setenv RPKGPATH "${RSTPATH}/rpkg"
setenv LOGPATH "${RSTPATH}/log"
setenv DOCPATH "${RSTPATH}/doc"


setenv GITURLBASE "http://superdarn.jhuapl.edu/git"

setenv WWWPATH "/www/superdarn/htdocs/doc"
setenv URLBASE "/doc"

setenv LIBPATH "${RSTPATH}/lib"
setenv BINPATH "${RSTPATH}/bin"
setenv IPATH "${RSTPATH}/include"

setenv PATH "${PATH}:${BUILD}/bin:${BUILD}/script:${RSTPATH}/bin:${RSTPATH}/script"
setenv LD_LIBRARY_PATH "${LIBPATH}:/usr/local/cdf/lib"

#####################################################################
#                                                                   #
# Makefiles                                                         #
#                                                                   #
#####################################################################

setenv MAKECFG ${BUILD}/make/makecfg
setenv MAKEBIN ${BUILD}/make/makebin
setenv MAKELIB ${BUILD}/make/makelib

