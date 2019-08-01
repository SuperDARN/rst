<!--
(C) copyright 2019 University Centre in Svalbard (UNIS)
author: Emma Bland, UNIS
-->
# RAWACF to FITACF

SuperDARN data is distributed to the community in the `rawacf`  format. For most science applications you will want to process these files into the `fitacf` format using the `make_fit` routine.

## Basic syntax
```
make_fit [inputfile].rawacf > [outputfile].fitacf
```

*Use the `-vb` flag to print some information in the terminal during the data processing.*

*By default, the data are processed using FITACF2.5. To use FITACF3.0 instead, add `-fitacf-version 3.0`*

### Old format files
The Data Analysis Working Group recommends converting old-format `dat` files to `rawacf` format before calling `make_fit`. For example, 
```
dattorawacf 2002020202f.dat > 2002020202f.rawacf
make_fit 2002020202f.rawacf > 2002020202f.fitacf
```

It is also possible to process `dat` files directly using `make_fit -old`, which will produce an old-format `fit` file as output. However, lossy compression associated with the old-format `fit` files will lead to small differences in the fitted parameters compared to the recommended procedure shown above. 
```
make_fit -old [inputfile].dat [outputfile].fit
```


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

!!! Warning
    When concatenating `fitacf` files, it is important that the files are in chronological order. Usually the shell sorts the filenames in the correct order automatically, but if you do the listing with wildcards (e.g. system call from another program), you may need to do the sorting yourself.




