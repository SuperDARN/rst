#!/bin/bash

flist=`find -L $HOMEPATH -name ".git" | sort`
for fname in ${flist}
do
  fpath=${fname%%.git}
  echo ${fpath}
  cd ${fpath}
  git --no-pager diff


done
