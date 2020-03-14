<!--
(C) Copyright 2019 University Centre in Svalbard (UNIS)
author: Emma Bland, UNIS
Modifications:
    2020-03-14 Emma Bland updated instructions on how to add IMF data
-->
# GRID to MAP
Map files contain the same information as grid files, and also contain the fitted coefficients describing the convection pattern. They are produced from combined grid files using a multi-step process:

1. Reformat a grid file into an empty `cnvmap` format file
2. Calculate the position of the Heppner Maynard Boundary and adds it to the `cnvmap` file
3. Add IMF and solar wind data to the `cnvmap` file
4. Calculate the statistical model and add it to the `cnvmap` file
5. Perform the spherical harmonic fitting for the `cnvmap` file

*If IMF data are not provided, RST uses the default values (Bx, By, Bz)=0*

*For southern hemisphere data, add the `-sh` flag in `map_grd`*

```
map_grd 20181001.grd > 20181001.empty.map
map_addhmb 20181001.empty.map > 20181001.hmb.map
map_addimf -if imfdata.txt 20181001.hmb.map > 20181001.imf.map
map_addmodel -o 8 -d l 20181001.imf.map > 20181001.model.map
map_fit  20181001.model.map > 20181001.north.map
```

## Supplying IMF data

IMF data from the ACE and WIND satellites can be supplied in CDF format using the `-ace` or `-wind` options in `map_addimf`. 
```
map_addimf -ace 20181001.hmb.map > 20181001.imf.map
```
The CDF files can be obtained from CDAWeb (https://cdaweb.gsfc.nasa.gov/). The ACE files have the form `ac_k1_mfi_[date]_v[xx].cdf` and should be placed in the directory `$ISTP_PATH/ace/[year]`. You can edit the `$ISTP_PATH` environment variable in `$RST_PATH/.profile/superdarn.bash`.

!!! Warning
    By default, RST loads 24 hours of IMF data. If you need to load data for a longer time period, use `-ex hr:mt`.

IMF data can also be supplied as a text file to the `map_addimf` routine using `-if [imffile]` 
The file should have the columns shown below. The IMF values will change only when a subsequent line the IMF file alters it.

```
year month day hour minute second bx by bz
```

You can also add fixed IMF values for the entire map file, for example
```
map_addimf -bx 1.5 -by -1.2 -bz 0.4 [file].map > [file].imf.map
```
