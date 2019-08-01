<!--
(C) copyright 2019 University Centre in Svalbard (UNIS)
author: Emma Bland, UNIS
-->
# Colors in RST

RST includes a set of color tables which are used by the plotting routines. You can view the available color tables in the directory `$COLOR_TABLE_PATH` (`$RSTPATH/tables/base/key`). 

Colors in RST are specified in the (hex) format ''AARRGGBB'':

- `AA` refers to transparency (00 = fully transparent and FF = opaque)
- `RR`,`GG`,`BB` refer to the 8-bit values for the red, green, and blue channels respectively


## Making your own color palette

New color keys can be provided as a text file, for example:
```
# NEW VELOCITY COLOR KEY
#   blue=motion towards radar
#   red=motion away from radar
#   high contrast either side of 0 m/s
#
10
FFFF0000
FFFF3001
FFFF6002
FFFF9003
FFFFC105
FF1DED24
FF15B15A
FF0E7691
FF073BC8
FF0000FF
```

The first non-comment line specifies the number of colors in the key. Ideally this should be a multiple of 10 so that the colors align with the tick marks when displayed on plots (the number of tick marks is fixed). 

To use your new color key:
```
field_plot [options] -key [newcolorkey.key] [file.fitacf]
time_plot [options] -vkey [newcolorkey.key] [file.fitacf]
```

In `time_plot`, you can replace `-vkey` with `-pkey`, `-wkey`, `-fkey` or `-nkey` to specify color keys for the power, spectral width, frequency and noise respectively. 

If you want to keep your new color key in a directory other than `$COLOR_TABLE_PATH`, you can specify the path using `-vkey_path`, `-xkey_path` etc. Type `[function name] --help | grep key_path` to view the available options. 
