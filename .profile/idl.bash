# idl.bash
# ========

# IDL header directory

export IDL_IPATH="/Applications/exelis/idl85/external/include"

# RST DLM directory

export DLMPATH="${RSTPATH}/dlm"

# IDL environment

export IDL_PATH="<IDL_DEFAULT>:+/${RSTPATH}/idl/lib"
export IDL_DLM_PATH="<IDL_DEFAULT>:${RSTPATH}/dlm"
export IDL_STARTUP="${RSTPATH}/idl/startup.pro"

# Makefile prototype for IDL DLMs

export MAKEDLM=${BUILD}/make/makedlm

