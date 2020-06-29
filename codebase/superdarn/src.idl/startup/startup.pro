;set_plot, 'X'
;device, retain=2
;device, true_color=24
;device, decomposed=0

; for AACGM-v2
.run genmag
.run astalg
.run igrflib_v2
.run aacgmlib_v2
.run aacgm_v2
.run time
.run mlt_v2

.run datamap.pro
.run aacgmdefault.pro
.run aacgm.pro
.run radar.pro
.run rprm.pro
.run iq.pro
.run raw.pro
.run fit.pro
.run grd.pro
.run cnvmap.pro
.run fitcnx.pro
.run cfit.pro
.run oldraw.pro
.run oldfit.pro
.run oldgrd.pro
.run oldcnvmap.pro
.run fitacf.pro
.run efield.pro
.run snd.pro

; Legacy interfaces
.run genlib.pro
.run acflib.pro
.run rawlib.pro
.run gridlib.pro
.run maplib.pro
.run fitlib.pro
.run aacgmlib.pro
.run igrflib.pro
.run geoplib.pro
.run rbposlib.pro
.run istplib.pro
.run global.pro

