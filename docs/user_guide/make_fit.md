# RAWACF to FITACF

SuperDARN data is distributed to the community in the `rawacf`  format. For most science applications you will want to process these files into the `fitacf` format using the `make_fit` routine.

## Basic syntax
```
make_fit [inputfile].rawacf > [outputfile].fitacf
```

*Use the `-vb` flag to print some information in the terminal during the data processing.*

*By default, the data are processed using FITACF2.5. To use FITACF3.0 instead, add `-fitacf-version 3.0`*

##  Convert multiple files
To perform the `rawacf` to `fitacf` conversion for all files in the current directory:
```
for file in *.rawacf
do 
  make_fit -vb $file > "${file%.rawacf}.fitacf"
done
```

You can then concatenate the 2-hour `fitacf` files into a daily file, for example:
```
cat 20181001.*.lyr.fitacf > 20181001.lyr.fitacf
```

*When concatenating `fitacf` files, it is important that the files are in chronological order. Usually the shell sorts the filenames in the correct order automatically, but if you do the listing with wildcards (e.g. system call from another program), you may need to do the sorting yourself.*




