<!--
(C) copyright 2019 University Centre in Svalbard (UNIS)
author: Emma Bland, UNIS

Modifications: 2022-06 Emma Bland (UNIS) Added fitting algorithm descriptions and updated examples
-->
# RAWACF to FITACF

SuperDARN data is distributed to the community in the `rawacf` format. For most science applications you will want to process these files into the `fitacf` format using the `make_fit` routine.

## Choosing a fitting algorithm

Several methods have been developed by the SuperDARN community to determine the physical properties of SuperDARN backscatter (power, velocity, spectral width, elevation, and their errors).

The fitting algorithms available for processing SuperDARN data are:

| Name       | Notes |
| :----------| :-----------|
| `fitacf3`  | Suitable for all scientific applications. Fitted data may appear 'noisy' for some radars due to operational problems (remove with [`fit_speck_removal`](despecking.md)).<br>**Released:** 2017 <br>**References:** (1) [FitACF 3.0 White Paper](https://superdarn.github.io/dawg/files/sup_material/FITACF3_white_paper.pdf), (2) [SuperDARN noise estimation](https://doi.org/10.1002/essoar.10510616.1)|
| `fitacf2`  | Suitable for all scientific applications. Generally results in fewer fitted ACFs compared to `fitacf3`. <br>**Released:** 2006<br>**Reference:** [Annales Geophysicae, 24, 115–128, 2006](https://doi.org/10.5194/angeo-24-115-2006) |
| `lmfit2`   | Suitable for scientific applications of fit-level data. Levenburg-Marquardt fitting of SuperDARN auto-correlation functions (ACFs). With no ad hoc assumptions/conditions. No elevation angles. **Call using separate binary,** `make_lmfit2` <br>**Released:** 2018<br>**Reference:** [Radio Science, 53, 93-111, 2018](10.1002/2017RS006450) |
| `lmfit1`   | Suitable for scientific applications of fit-level data. A model complex ACF (single component, exponential decay) is fitted to the observed one using the Levenberg-Marquardt algorithm. No elevation angles. <br>**Released:** c.a. 2012<br>**Reference:** [Radio Science, 48, 274–282, 2013](doi:10.1002/rds.20031) |
| `fitex2` | Intended for use with the `tauscan` multipulse sequence. Phase fitting performed with 120 phase variation models. <br>**Released:** c.a. 2012<br>**Reference:** [Radio Science, 48, 274–282, 2013](doi:10.1002/rds.20031) |
| `fitex1` | Original `fitex` algorithm. Not thoroughly tested. No elevation angles. |

<br>

!!! Warning
    The `lmfit2` algorithm must be called using the separate binary, `make_lmfit2`.


## Basic syntax
```
make_fit -fitacf3 [inputfile].rawacf > [outputfile].fitacf3

make_fit -fitacf2 [inputfile].rawacf > [outputfile].fitacf2
```

*Use the `-vb` flag to print some information in the terminal during the data processing.*

### Old format files
The Data Analysis Working Group recommends converting old-format `dat` files to `rawacf` format before calling `make_fit`. For example, 
```
dattorawacf 2002020202f.dat > 2002020202f.rawacf

make_fit -fitacf2 2002020202f.rawacf > 2002020202f.fitacf2
```

It is also possible to process `dat` files directly using `make_fit -old`, which will produce an old-format `fit` file as output. However, lossy compression associated with the old-format `fit` files will lead to small differences in the fitted parameters compared to the recommended procedure shown above. 
```
make_fit -old -fitacf2 [inputfile].dat [outputfile].fit
```


##  Convert multiple files
To perform the `rawacf` to `fitacf` conversion for all files in the current directory:
```
for file in *.rawacf
do 
  make_fit -fitacf3 -vb $file > "${file%.rawacf}.fitacf3"
done
```

You can then concatenate the 2-hour `fitacf` files into a daily file, for example:
```
cat 20181001.*.lyr.fitacf3 > 20181001.lyr.fitacf3
```

!!! Warning
    When concatenating `fitacf` files, it is important that the files are in chronological order. Usually the shell sorts the filenames in the correct order automatically, but if you do the listing with wildcards (e.g. system call from another program), you may need to do the sorting yourself.




