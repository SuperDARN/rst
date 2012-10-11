;+
; NAME:
; RAD_FIT_DATA2ASCII
;
; PURPOSE:
; This procedure writes fitACF data into a ASCII file. It creates one file
; per beam, putting the coordinates of the beam/gate cells at the beginning
; of the file.
;
; INPUTS:
; Date: A scalar or 2-element vector giving the date range to output,
; in YYYYMMDD or MMMYYYY format.
;
; Radar: The three letter code of the radar for which to output data.
;
; KEYWORD PARAMETERS:
; TIME: A 2-element vector giving the time range to plot, in HHII format.
;
; LONG: Set this keyword to indicate that the TIME value is in HHIISS
; format rather than HHII format.
;
; PARAM: Set this keyword to specify the parameter to plot. Allowable
; values are 'power', 'velocity', and 'width'. Default is 'power'.
;
; BEAMS: Set this keyword to an array of beam numbers to output.
;
; CHANNEL: Set this keyword to the channel number you want to output.
;
; SCAN_ID: Set this keyword to the numeric scan id you want to output.
;
; COORDS: Set this keyword to a string naming the coordinate system
; of the radar cell positions. Allowable inputs are 'magn' and 'geog'.
;
; OUTDIR: Set this keyword to a string naming the directory in which
; to output the file(s). Make sure you have write access. Default is ./
;
; IGNORE_GSCATTER: Set this keyword to not print data marked as ground scatter.
;
; COMMON BLOCKS:
; RAD_DATA_BLK: The common block holding the currently loaded radar data and
; information about that data.
;
; EXAMPLE:
;
; COPYRIGHT:
; Non-Commercial Purpose License
; Copyright © November 14, 2006 by Virginia Polytechnic Institute and State University
; All rights reserved.
; Virginia Polytechnic Institute and State University (Virginia Tech) owns the DaViT
; software and its associated documentation (“Software”). You should carefully read the
; following terms and conditions before using this software. Your use of this Software
; indicates your acceptance of this license agreement and all terms and conditions.
; You are hereby licensed to use the Software for Non-Commercial Purpose only. Non-
; Commercial Purpose means the use of the Software solely for research. Non-
; Commercial Purpose excludes, without limitation, any use of the Software, as part of, or
; in any way in connection with a product or service which is sold, offered for sale,
; licensed, leased, loaned, or rented. Permission to use, copy, modify, and distribute this
; compilation for Non-Commercial Purpose is hereby granted without fee, subject to the
; following terms of this license.
; Copies and Modifications
; You must include the above copyright notice and this license on any copy or modification
; of this compilation. Each time you redistribute this Software, the recipient automatically
; receives a license to copy, distribute or modify the Software subject to these terms and
; conditions. You may not impose any further restrictions on this Software or any
; derivative works beyond those restrictions herein.
; You agree to use your best efforts to provide Virginia Polytechnic Institute and State
; University (Virginia Tech) with any modifications containing improvements or
; extensions and hereby grant Virginia Tech a perpetual, royalty-free license to use and
; distribute such modifications under the terms of this license. You agree to notify
; Virginia Tech of any inquiries you have for commercial use of the Software and/or its
; modifications and further agree to negotiate in good faith with Virginia Tech to license
; your modifications for commercial purposes. Notices, modifications, and questions may
; be directed by e-mail to Stephen Cammer at cammer@vbi.vt.edu.
; Commercial Use
; If you desire to use the software for profit-making or commercial purposes, you agree to
; negotiate in good faith a license with Virginia Tech prior to such profit-making or
; commercial use. Virginia Tech shall have no obligation to grant such license to you, and
; may grant exclusive or non-exclusive licenses to others. You may contact Stephen
; Cammer at email address cammer@vbi.vt.edu to discuss commercial use.
; Governing Law
; This agreement shall be governed by the laws of the Commonwealth of Virginia.
; Disclaimer of Warranty
; Because this software is licensed free of charge, there is no warranty for the program.
; Virginia Tech makes no warranty or representation that the operation of the software in
; this compilation will be error-free, and Virginia Tech is under no obligation to provide
; any services, by way of maintenance, update, or otherwise.
; THIS SOFTWARE AND THE ACCOMPANYING FILES ARE LICENSED “AS IS”
; AND WITHOUT WARRANTIES AS TO PERFORMANCE OR
; MERCHANTABILITY OR ANY OTHER WARRANTIES WHETHER EXPRESSED
; OR IMPLIED. NO WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE IS
; OFFERED. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF
; THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE,
; YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
; CORRECTION.
; Limitation of Liability
; IN NO EVENT WILL VIRGINIA TECH, OR ANY OTHER PARTY WHO MAY
; MODIFY AND/OR REDISTRIBUTE THE PRORAM AS PERMITTED ABOVE, BE
; LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL,
; INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR
; INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS
; OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED
; BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE
; WITH ANY OTHER PROGRAMS), EVEN IF VIRGINIA TECH OR OTHER PARTY
; HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
; Use of Name
; Users will not use the name of the Virginia Polytechnic Institute and State University nor
; any adaptation thereof in any publicity or advertising, without the prior written consent
; from Virginia Tech in each case.
; Export License
; Export of this software from the United States may require a specific license from the
; United States Government. It is the responsibility of any person or organization
; contemplating export to obtain such a license before exporting.
;
; MODIFICATION HISTORY:
; Written by Lasse Clausen, Nov, 24 2009
;-
pro output_ascii, date, radar, time=time, long=long, $
	param=param, beams=beams, coords=coords, $
	scan_id=scan_id, channel=channel, $
	ignore_gscatter=ignore_gscatter, outdir=outdir

common rad_data_blk

if n_params() ne 2 then begin
	prinfo, 'Must give date and radar code.'
	return
endif

; set deault time if neccessary
if ~keyword_set(time) then $
	time = [0000,2400]

if ~keyword_set(param) then $
	param = get_parameter()

if n_elements(beams) eq 0 then $
	beams = rad_get_beam()

if n_elements(coords) eq 0 then $
	coords = 'geog'

if coords eq 'mlt' then begin
	prinfo, 'MLT coordinates do not make any sense.'
	return
endif

if ~keyword_set(scan_id) then $
	scan_id = -1

if n_elements(channel) eq 0 and scan_id eq -1 then begin
		channel = rad_fit_info.channels[0]
endif

if ~keyword_set(outdir) then $
	outdir = './'

if ~file_test(outdir, /dir) then begin
	prinfo, 'Cannot find output directory: '+outdir
	return
endif

; read radar data
rad_fit_read, date, radar, time=time, long=long
di = rad_fit_get_data_index()
rad_define_beams, id=(*rad_fit_info[di]).id, coords=coords, $
	fov_loc_full=fov_loc_full, fov_loc_center=fov_loc_center

; check if anythign was loaded
if (*rad_fit_info[di]).nrecs eq 0L then begin
	if ~keyword_set(silent) then $
		prinfo, 'No data loaded.'
	return
endif

; check what data was loaded
if (*rad_fit_info[di]).fitex then $
	fitstr = 'fitEX'
if (*rad_fit_info[di]).fitacf then $
	fitstr = 'fitACF'
if (*rad_fit_info[di]).fit then $
	fitstr = 'fit'

sfjul, date, time, sjul, fjul

; construct output filename
obasefile = format_date(date)+'_'+format_time(time)+'_'+radar

rad_fit_calc_velocity


for b=0, n_elements(beams)-1 do begin
	; get data for beam
	binds = where((*rad_fit_data[di]).beam eq beams[b], bc)
	if bc eq 0 then begin
		prinfo, 'No data found for beam: '+string(beams[b])
		continue
	endif
	bjuls = (*rad_fit_data[di]).juls[binds]
	bvels = (*rad_fit_data[di]).velocity[binds,*]
	bpowe = (*rad_fit_data[di]).power[binds,*]
	bwidt = (*rad_fit_data[di]).width[binds,*]
	bscan = (*rad_fit_data[di]).scan_id[binds]
	bgsca = (*rad_fit_data[di]).gscatter[binds,*]
	nranges = n_elements(bvels[0,*])
	; get data in time frame
	jinds = where(bjuls ge sjul and bjuls le fjul, jc)
	if jc eq 0 then begin
		prinfo, 'No data found for in range: '+format_juldate(sjul)+'-'+format_juldate(fjul)
		continue
	endif
	; open file
	ofile = outdir+obasefile+'_'+string(beams[b],format='(I02)')+'.dat'
	prinfo, 'Writing to: '+ofile
	openw, olun, ofile, /get_lun
	; print header
	printf, olun, '# '+radar+' ('+fitstr+') '+format_date(date)+' '+format_time(time)
	printf, olun, '# Location of ranges along beam '+string(beams[b],format='(I02)')
	printf, olun, '# all four corners of cell in geographic coords'
	printf, olun, '# lat1, lon1, lat2, lon2, lat3, lon3, lat4, lon4'
	printf, olun, string(nranges,format='(I03)')
	for r=0, nranges-1 do begin
		printf, olun, string(fov_loc_full[*,*,beams[b],r], format='(8f8.2)')
	endfor
	printf, olun, string(jc,format='(I04)')
	for j=0L, jc-1L do begin
		if scan_id ne -1 then begin
			if bscan[jinds[j]] ne bscan[jinds[j]] then $
				continue
		endif
		line = ''
		rcount = 0
		for r=0, nranges-1 do begin
			if bvels[jinds[j], r] eq 10000. then $
				continue
			if keyword_set(ignore_gscatter) then begin
				if bgsca[jinds[j], r] then $
					continue
			endif
			line += '( '+string(r,format='(I03)')+','+string(bpowe[jinds[j], r],format='(e12.5)')+','+$
				string(bvels[jinds[j], r],format='(e12.5)')+','+string(bwidt[jinds[j], r],format='(e12.5)')+' )'
			rcount += 1
		endfor
		printf, olun, format_juldate(bjuls[jinds[j]], /redox)+' '+string(bscan[jinds[j]], format='(I6)')+' '+string(rcount,format='(I03)')+' '+line
	endfor
	; close file
	free_lun, olun
endfor

end
