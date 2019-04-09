/* geobeam.h
   =========
*/

struct GeoLocBeam {
  int bm;
  int frang;
  int rsep;
  int rxrise;
  int nrang;
  float *glat[3];
  float *glon[3];
  float *mlat[3];
  float *mlon[3];
  float *gazm;
  float *mazm;
};

struct GeoLocScan {
  int num;
  struct GeoLocBeam *bm;
};
  
int GeoLocCenter(struct RadarSite *site,int mag,float *lat,float *lon,
                 int chisham,int old_aacgm);
int GeoLocBeam(struct RadarSite *site,int year,
               struct GeoLocScan *geol,struct RadarBeam *bm,
               int chisham,int old_aacgm);
