# rst.bash
# ========

#####################################################################
#                                                                   #
# Directory Paths                                                   #
#                                                                   #
#####################################################################

export BUILD="${RSTPATH}/build"
export CODEBASE="${RSTPATH}/codebase"
export LOGPATH="${RSTPATH}/log"
export DOCPATH="${RSTPATH}/doc"

export WWWPATH="${DOCPATH}/www"
export URLBASE="https://superdarn.github.io/rst"

export LIBPATH="${RSTPATH}/lib"
export BINPATH="${RSTPATH}/bin"
export IPATH="${RSTPATH}/include"

export PATH="${PATH}:${BUILD}/bin:${BUILD}/script:${RSTPATH}/bin:${RSTPATH}/script"
export LD_LIBRARY_PATH="${LIBPATH}:/usr/local/cdf/lib"
export DYLD_LIBRARY_PATH="${LIBPATH}:/Applications/cdf/cdf/lib"

#####################################################################
#                                                                   #
# Makefiles                                                         #
#                                                                   #
#####################################################################

export MAKECFG=${BUILD}/make/makecfg
export MAKEBIN=${BUILD}/make/makebin
export MAKELIB=${BUILD}/make/makelib


