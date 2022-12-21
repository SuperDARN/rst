<!--
(C) copyright 2019 University Centre in Svalbard (UNIS)
author: Emma Bland, UNIS
-->
# FITACF to GRD
Grid files contain combined data that have been organized into a latitude/longitude grid. They may contain data from a single radar or from multiple radars. Multi-radar grid files are created in a two-step process:

1. Create separate grid files for each radar
2. Combine the separate grid files into a single file

```
make_grid -vb [inputfile].fitacf > [outputfile].grd
combine_grid -vb *.grd > [outputfile].grd
```

*Sometimes `make_grid` is not able to use the scan flag to determine the scan length. If you get the "error reading file" error message, try adding `-tl 60`*

The `make_grid` routine provides a lot of options for customizing the gridding process, such as the altitude at which the mapping is performed, or whether to exclude ground/ionospheric scatter. The optional `-xtd` ("extended") flag will include the power and spectral width parameters in the output file, in addition to the velocity. Type `make_grid --help` for more information. 
	
	
### Full-day grid files
The standard practice is to generate 24-hour grid files. If you have already made a concatenated 24-hour fitacf file, then use that file in the manner shown above. Alternatively, you can provide several fitacf files as input to `make_grid`, and the routine will automatically concatenate the files for you. This works only when the input files are all from the same radar; the method for combining data from multiple radars into a single grid file is shown in the next section.
```
make_grid -vb -tl 60 -xtd 20181001.*.lyr.fitacf > 20181001.lyr.grd
```

### Multi-Channel Data 

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

!!! Warning
    Do not combine data from both hemispheres into a single grid file. This will create problems later when determining the convection pattern.

First, generate grid files for each `fitacf` file you want to include. For example,

```
# Make grid files for specific radars. The 2-hour fitacf files from each radar are
# automatically concatenated by make_grid

radarlist=lyr cly inv rkn

for radar in $radarlist
do
  make_grid -tl 60 -xtd -vb 20181001.*.$radar.fitacf > 20181001.$radar.grd
done
```

Second, combine the grid files for each radar into a single multi-radar grid file:
```
combine_grid -vb 20181001*.grd > 20181001.north.grd
```
