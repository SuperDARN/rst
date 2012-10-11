/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"istp_text - Extract ASCII listings of  ISTP data from CDF files.\n",
"istp_text --help\n",
"istp_text  [-cf cfgname] [-h] [-path pathname] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn [-gse] [-pos] [-mfi] [-swe] [-mag] [-pla] [-mgf] [-lep] [-cpi] [-ace] [-wind] [-imp8] [-geotail]\n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-h\tinclude headers in the text file.\n",
"-path path\tread the data files from the directory path.\n",
"-sd yyyymmdd\textract starting from the date yyyymmdd.\n",
"-st hr:mn\textract starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop extracting at the date yyyymmdd.\n",
"-et hr:mn\tstop extracting at the time hr:mn.\n",
"-ex hr:mn\textract an interval whose extent is hr:mn.\n",
"-gse\tplot in GSE coordinates, not GSM.\n",
"-pos\tinclude spacecraft position in the text file.\n",
"-mfi\tinclude MFI data.\n",
"-swe\tinclude SWE data.\n",
"-mag\tinclude MAG data.\n",
"-pla\tinclude PLA data.\n",
"-mgf\tinclude MGF data.\n",
"-lep\tinclude LEP data.\n",
"-cpi\tinclude CPI data.\n",
"-ace\tinclude ACE data.\n",
"-wind\tinclude Wind data.\n",
"-imp8\tinclude IMP8 data.\n",
"-geotail\tinclude Geotail data.\n",

NULL};
