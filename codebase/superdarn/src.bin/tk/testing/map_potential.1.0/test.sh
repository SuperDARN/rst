#!/bin/sh

com_opts='-ps -coast -fcoast -rotate -igrd -igrdontop -igrdlon 30 \
          -igrdcol aaff69b4 -grd -latmin 40 -tmlbl -time \
          -grdontop -vkey color.key -vkeyp -fterm -dn -logo'

#
# Create some PostScript figures of grid data
#

date='20130316'
grd="${date}.0600-0602.north.grd"

opts="$com_opts -raw"

#
# FIGURES 1. use geographic coordinates and plot old AACGM grid for grid vectors
#
echo ""
echo "Making grid plots using OLD AACGM coordinates"
echo ""
grid_plot -old -old_aacgm $opts $grd

files=`ls ${date}.????.??.ps`
for file in $files ; do
	mv $file ${file%??}grd_aacgm.g.ps
  ps2pdf ${file%??}grd_aacgm.g.ps
done

#
# FIGURES 2. use geographic coordinates and plot new AACGM_v2 grid for grid vectors
#
echo ""
echo "Making grid plots using AACGM-v2 coordinates"
echo ""
grid_plot -old $opts $grd

files=`ls ${date}.????.??.ps`
for file in $files ; do
	mv $file ${file%??}grd_aacgm_v2.g.ps
  ps2pdf ${file%??}grd_aacgm_v2.g.ps
done

com_opts='-ps -coast -fcoast -rotate -mag -igrd -igrdontop -igrdlon 30 \
          -igrdcol aaff69b4 -grd -latmin 40 -tmlbl -time \
          -grdontop -vkey color.key -vkeyp -fterm -dn -logo'
opts="$com_opts -raw"

#
# FIGURES 3. use old AACGM coordinates for grid vectors
#
echo ""
echo "Making grid plots using AACGM coordinates"
echo ""
grid_plot -old -old_aacgm $opts $grd

files=`ls ${date}.????.??.ps`
for file in $files ; do
	mv $file ${file%??}grd_aacgm.ps
	ps2pdf ${file%??}grd_aacgm.ps
done

#
# FIGURES 4. use new AACGM_v2 coordinates for grid vectors
#
echo ""
echo "Making grid plots using AACGM-v2 coordinates"
echo ""
grid_plot -old $opts $grd

files=`ls ${date}.????.??.ps`
for file in $files ; do
	mv $file ${file%??}grd_aacgm_v2.ps
	ps2pdf ${file%??}grd_aacgm_v2.ps
done


map="${grd%????}.old.map"

#
# Make a map file
#
echo ""
echo "Making map file using old AACGM"
echo ""
map_grd -old_aacgm -old $grd | map_addhmb -old_aacgm -old | \
               map_addimf -bx 0 -by 5 -bz -5 -old | \
               map_addmodel -o 8 -old | map_fit -vb -old > $map

#
# FIGURES 5. 
#
echo ""
echo "Making grid plots using old AACGM coordinates"
echo ""
opts="$com_opts -fit -ctr -hmb -modn -imf -pot"
map_plot -old -old_aacgm $opts $map

files=`ls ${date}.????.??.ps`
for file in $files ; do
	mv $file ${file%??}map_old.ps
	ps2pdf ${file%??}map_old.ps
done

# plot model vectors
opts="$com_opts -mod -ctr -hmb -modn -imf -pot"
map_plot -old -old_aacgm $opts $map

files=`ls ${date}.????.??.ps`
for file in $files ; do
	mv $file ${file%??}map_old.RG96.ps
	ps2pdf ${file%??}map_old.RG96.ps
done

#
# Make a map file
#
map="${grd%????}.map"
echo ""
echo "Making map file using AACGM-v2"
echo ""
map_grd -old $grd | map_addhmb -old | \
               map_addimf -bx 0 -by 5 -bz -5 -old | \
               map_addmodel -o 8 -old | map_fit -vb -old > $map

#
# FIGURES 6. 
#
echo ""
echo "Making grid plots using AACGM-v2 coordinates"
echo ""
opts="$com_opts -fit -ctr -hmb -modn -imf -pot"
map_plot -old $opts $map

files=`ls ${date}.????.??.ps`
for file in $files ; do
	mv $file ${file%??}map_v2.ps
	ps2pdf ${file%??}map_v2.ps
done

# plot model vectors
opts="$com_opts -mod -ctr -hmb -modn -imf -pot"
map_plot -old $opts $map

files=`ls ${date}.????.??.ps`
for file in $files ; do
	mv $file ${file%??}map_v2.RG96.ps
	ps2pdf ${file%??}map_v2.RG96.ps
done

exit

#out="${grd%????}.out"
#extract_map -mid -p $map > $out

opts="$com_opts -fit -ctr -hmb -modn -imf -pot"
map_plot $opts $map

files=`ls ${date}.????.??.ps`
for file in $files ; do
	mv $file ${file%??}map.ps
done

