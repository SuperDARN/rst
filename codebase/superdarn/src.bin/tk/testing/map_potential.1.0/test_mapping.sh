#!/bin/sh

com_opts='-ps -coast -fcoast -rotate -igrd -igrdontop -igrdlon 30 \
          -igrdcol aaff69b4 -grd -latmin 40 -tmlbl -time \
          -grdontop -vkey color.key -vkeyp -fterm -dn -logo'
com_opts='-ps -coast -fcoast -rotate -grd -latmin 40 -tmlbl -time \
          -grdontop -vkey color.key -vkeyp -fterm -dn -logo -imf \
          -fpoly -pot -ctr -modn -hmb -nopad -mod -fit -mag'

date='20130316'
root="${date}.0600-0602.north"

grd="${root}.grd"
grdx="${root}.grdx"

maproot="${grd%????}"
map="${grd%????}.map"
cnvmap="${grd%????}.cnvmap"

#
# Make a map file
#
mods="RG96 PSR10 CS10"
old="-old_aacgm"
aacgm="aacgm"
old=""
aacgm="aacgm_v2"

for mod in $mods ; do
  echo ""
  echo "Making map file using old AACGM and $mod"
  echo ""
  map_grd $old $grdx | map_addhmb $old | \
                       map_addimf -bx 0.1 -by 4 -bz -2 -vx 450 | \
                       map_addmodel $old -o 8 -${mod} -nointerp | \
                       map_fit -vb > ${maproot}.${mod}.${aacgm}.cnvmap

  echo "Making map plots "
  opts="$com_opts $old"
  map_plot $opts ${maproot}.${mod}.${aacgm}.cnvmap

  files=`ls ${date}.????.??.ps 2>/dev/null`
  for file in $files ; do
	  mv $file ${file%???}.${mod}.${aacgm}.ps
	  ps2pdf ${file%???}.${mod}.${aacgm}.ps
  done

done

exit

#
# FIGURES 5. 
#

map_grd -old_aacgm $grdx | map_addhmb -old_aacgm | \
               map_addimf -bx 0 -by 5 -bz -5 | \
               map_addmodel -old_aacgm -o 8 -RG96 | \
               map_fit -vb > ${maproot}.RG96.n.cnvmap

map_plot -old_aacgm $opts ${maproot}.RG96.n.cnvmap
files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}map_old.RG96.ps
	ps2pdf ${file%??}map_old.RG96.ps
done

exit

# plot model vectors
opts="$com_opts -mod -ctr -hmb -modn -imf -pot"
map_plot -old -old_aacgm $opts $map

files=`ls ${date}.????.??.ps 2>/dev/null`
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

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}map_v2.ps
	ps2pdf ${file%??}map_v2.ps
done

# plot model vectors
opts="$com_opts -mod -ctr -hmb -modn -imf -pot"
map_plot -old $opts $map

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}map_v2.RG96.ps
	ps2pdf ${file%??}map_v2.RG96.ps
done

exit

#out="${grd%????}.out"
#extract_map -mid -p $map > $out

opts="$com_opts -fit -ctr -hmb -modn -imf -pot"
map_plot $opts $map

files=`ls ${date}.????.??.ps 2>/dev/null`
for file in $files ; do
	mv $file ${file%??}map.ps
done

