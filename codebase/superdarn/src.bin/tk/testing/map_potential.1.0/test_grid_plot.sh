#!/bin/sh

com_opts='-ps -coast -fcoast -rotate -igrd -igrdontop -igrdlon 30 \
          -igrdcol aaff69b4 -grd -latmin 40 -tmlbl -time \
          -grdontop -vkey color.key -vkeyp -fterm -dn -logo'

#
# Create some PostScript figures of grid data
#

date='20130316'
root="${date}.0600-0602.north"
grd="${root}.grd"
grdx="${root}.grdx"

opts="$com_opts -raw"

#
# FIGURES 1. use geographic coordinates and plot old AACGM grid for grid vectors
#
echo ""
echo "Making grid plots using OLD AACGM coordinates"
echo ""
echo ".grd file"
grid_plot -old -old_aacgm $opts $grd

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}grd_aacgm.o.g.ps
  ps2pdf ${file%??}grd_aacgm.o.g.ps
  echo ${file%??}grd_aacgm.o.g.ps
done

echo ".grdx file"
grid_plot -old_aacgm $opts $grdx

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}grd_aacgm.n.g.ps
  ps2pdf ${file%??}grd_aacgm.n.g.ps
  echo ${file%??}grd_aacgm.n.g.ps
done

#
# FIGURES 2. use geographic coordinates and plot new AACGM_v2 grid for grid vectors
#
echo ""
echo "Making grid plots using AACGM-v2 coordinates"
echo ""
echo ".grd file"
grid_plot -old $opts $grd

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}grd_aacgm_v2.o.g.ps
  ps2pdf ${file%??}grd_aacgm_v2.o.g.ps
  echo ${file%??}grd_aacgm_v2.o.g.ps
done

echo ".grdx file"
grid_plot $opts $grdx

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}grd_aacgm_v2.n.g.ps
  ps2pdf ${file%??}grd_aacgm_v2.n.g.ps
  echo ${file%??}grd_aacgm_v2.n.g.ps
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
echo ".grd file"
grid_plot -old -old_aacgm $opts $grd

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}grd_aacgm.o.ps
	ps2pdf ${file%??}grd_aacgm.o.ps
  echo ${file%??}grd_aacgm.o.ps
done

echo ".grdx file"
grid_plot -old_aacgm $opts $grdx

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}grd_aacgm.n.ps
	ps2pdf ${file%??}grd_aacgm.n.ps
  echo ${file%??}grd_aacgm.n.ps
done

#
# FIGURES 4. use new AACGM_v2 coordinates for grid vectors
#
echo ""
echo "Making grid plots using AACGM-v2 coordinates"
echo ""
echo ".grd file"
grid_plot -old $opts $grd

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}grd_aacgm_v2.o.ps
	ps2pdf ${file%??}grd_aacgm_v2.o.ps
	echo ${file%??}grd_aacgm_v2.o.ps
done

echo ".grdx file"
grid_plot $opts $grdx

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}grd_aacgm_v2.n.ps
	ps2pdf ${file%??}grd_aacgm_v2.n.ps
	echo ${file%??}grd_aacgm_v2.n.ps
done

