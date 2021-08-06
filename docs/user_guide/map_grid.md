<!--
(C) Copyright 2019 University Centre in Svalbard (UNIS)
author: Emma Bland, UNIS

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Modifications:
    2021-03-10 Emma Bland (UNIS) updated instructions on how to add IMF data
    2021-07-14 Kevin Sterne (VT) updated with option incompatibility related
               to reading in Kp values.
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

IMF data can be supplied as a text file to the `map_addimf` routine using `-if [imffile]` 
The file should have the columns shown below. The IMF values will change only when a subsequent line the IMF file alters it.

```
year month day hour minute second bx by bz Vx Kp
```

You can also add fixed IMF values for the entire map file, for example
```
map_addimf -bx 1.5 -by -1.2 -bz 0.4 [file].map > [file].imf.map
```

**NOTE:** When supplying IMF data via a text file, **AND** using the `-old` option, the Kp values will not be read in and stored in the resulting file.


### CDF files
IMF data from the ACE and WIND satellites can also be supplied in CDF format using the `-ace` or `-wind` options in `map_addimf`:

```
map_addimf -ace 20181001.hmb.map > 20181001.imf.map
```
The CDF files can be obtained from [CDAWeb](https://cdaweb.gsfc.nasa.gov/). ACE and WIND data should be placed in the directory `$ISTP_PATH/ace/[year]` and `$ISTP_PATH/wind/[year]` respectively. You can edit the `$ISTP_PATH` environment variable in `$RST_PATH/.profile/superdarn.bash` (or in your `.bashrc` file).

For ACE data, RST searches for the following datasets:

- Magnetic Field 16-Second Level 2 Data: `ac_H0_mfi_[date]_v[xx].cdf` (first preference)
- Magnetic Field 16-Second Key Parameters: `ac_k1_mfi_[date]_v[xx].cdf`

For WIND data, RST searches for datasets of the form: `wi_[level]_mfi_[date]_v[xx].cdf`




!!! Warning
    By default, RST loads 24 hours of IMF data. If you need to load data for a longer time period, use `-ex hr:mt`.


