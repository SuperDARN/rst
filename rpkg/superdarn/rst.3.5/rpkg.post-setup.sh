#!/bin/bash

if test -n "${INSTALL_PKG_SESSION_ID}"
then
  # darwin installation from package manager
  export HOMEPATH=${2}
else
  export HOMEPATH=${PWD}/rst
fi


mkdir -p ${HOMEPATH}/idl
mkdir -p ${HOMEPATH}/idl/lib
mkdir -p ${HOMEPATH}/idl/app

cd ${HOMEPATH}/idl/lib
ln -svf ../../codebase/analysis/src.idl/lib analysis
ln -svf ../../codebase/general/src.idl/lib general
ln -svf ../../codebase/superdarn/src.idl/lib superdarn

cd ${HOMEPATH}/idl/app
ln -svf ../../codebase/superdarn/src.idl/app superdarn

cd ${HOMEPATH}/idl
ln -svf ../codebase/superdarn/src.idl/startup/startup.pro startup.pro


