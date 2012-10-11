# idl.tcsh
# ========

# IDL header directory

setenv IDL_IPATH "/usr/local/itt/idl/external/include"

# RST DLM directory

setenv DLMPATH "${RSTPATH}/dlm"

# IDL environment

setenv IDL_PATH "<IDL_DEFAULT>:+/${RSTPATH}/idl/lib"
setenv IDL_DLM_PATH "<IDL_DEFAULT>:${RSTPATH}/dlm"
setenv IDL_STARTUP "${RSTPATH}/idl/startup.pro"

# Makefile prototype for IDL DLMs

setenv MAKEDLM ${BUILD}/make/makedlm

