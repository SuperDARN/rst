<!--
(C) copyright 2019 University Centre in Svalbard (UNIS)
author: Emma Bland, UNIS
-->
# FITACF to GRD
Grid files contain combined data from multiple SuperDARN radars which have been organized into a latitude/longitude grid. Grid files are created in a two-step process:

1. Create separate grid files for each radar
2. Combine the separate grid files into a single file

```
make_grid -vb [inputfile].fitacf > [outputfile].grd
combine_grid -vb *.grd > [outputfile].grd
```

*Sometimes `make_grid` is not able to use the scan flag to determine the scan length. If you get the "error reading file" error message, try adding `-tl 60`*

The `make_grid` routine provides a lot of options for customizing the gridding process, such as the altitude at which the mapping is performed, or whether to exclude ground/ionospheric scatter. The optional `-xtd` ("extended") flag will include the power and spectral width parameters in the output file, in addition to the velocity. Type `make_grid --help` for more information. 
	
## Full-day grid files
The standard practice is to generate 24-hour grid files. If you have already made a concatenated 24-hour fitacf file, then use that file in the manner shown above. Otherwise, you can use the `-c` flag to get RST to concatenate the input fitacf files on-the-fly. This works only when the input files are all from the same radar; the method for combining data from multiple radars into a single grid file is shown in the next section.
```
make_grid -vb -tl 60 -xtd -c 20181001.*.lyr.fitacf > 20181001.lyr.grd
```

## Multi-Channel Data 

Some SuperDARN radars provide multi-channel data, either through stereo capability (e.g. Hankasalmi) or as a convenient way to separate data from different frequencies (e.g. the  *twofsound* mode). In both cases, the data are assigned to either channel 1 or channel 2. When only a single channel is used, the channel number is 0. 

By default, `make_grid` will use the data from all available channels when making a grid file. To include data from just one channel, use the `-cn` option: 

 - `-cn a` : will grab only channel 0 and 1 data and will not combine the two channels. 
 - `-cn b` : will grab only channel 2 data. 

For example:
```
make_grid -cn a 20180101.C0.cly.fitacf > 20180101.cly.1.grid 
```

```
make_grid -cn b 20180101.C0.cly.fitacf > 20180101.cly.2.grid 
```


!!! Warning
    By default, `make_grid` will grab all available channels and put all the data in a median filter for the given integration time.

!!! Note
    Canadian radars typically run `twofsound`: *Saskatoon*, *Prince George*, *Rankin Inlet*, *Clyde River*, *Inuvik*.
    To check if a fitacf file contains channel 1 or 2 data you can use the following command:
    ```
    dmapdump 20180101.C0.cly.fitacf | grep "channel\" \= 1" | wc -l
    ```

## Combine data from multiple radars
First, generate grid files for each `fitacf` file you want to include. For example,

```
# Make grid files for specific radars
#
for radar in lyr cly inv rkn
do 
  make_grid -new -tl 60 -xtd -vb -c \
  20181001*.$radar.fitacf > 20181001.$radar.grd
done
```

*The `-c` flag will concatenate the `fitacf` files together during the gridding process.*

*The `-f 4` option extracts the 3-letter radar code `rad` from the filename `YYYYMMDD.hhmm.ss.rad.fitacf`.*

```
# Make grid files for all 2-hour fitacf files in the current
# directory. The fitacf files are concatenated using the -c flag

radarlist="$(ls | cut -d "." -f 4 | sort -u)"

for radar in $radarlist
do
  make_grid -tl 60 -xtd -vb -c 20181001*.$radar.fitacf > 20181001.$radar.grd
done
```

Second, combine the grid files for each radar into a single multi-radar grid file:
```
combine_grid -vb 20181001*.grd > 20181001.north.grd
```
