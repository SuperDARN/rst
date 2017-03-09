#!/bin/sh

com_opts='-ps -coast -fcoast -mag -rotate -grd -grdlon 30 -tmlbl -time \
          -grdontop -vkey color.key -vkeyp -fterm -dn'
#
# Create some PostScript figures of grid data
#

grd='20130316.0600-0610.north.grd'

opts="$com_opts -raw"

#grid_plot $opts $grd


#
# Create a map file
#

map="${grd%????}.map"
echo $map

map_grd $grd | map_addhmb | map_addimf -bx 0 -by 5 -bz -5 | \
               map_addmodel -o 8 | map_fit -vb > $map

out="${grd%????}.out"
extract_map -mid -p $map > $out

opts="$com_opts -fit -ctr -hmb -modn -imf -pot"
map_plot $opts $map

