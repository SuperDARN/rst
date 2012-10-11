/* StationID.java
   ============== 
   Author: R.J.Barnes
*/

/*
 $License$
*/




public class StationID {

   static String station[]={
      "Unused",
      "Goose Bay",
      "Schefferville",
      "Kapuskasing",
      "Halley",
      "Saskatoon",
      "British Columbia",
      "Alaska",
      "Stokkseyri",
      "Iceland East",
      "Finland",
      "Sanae",
      "Syowa",
      "Syowa East",
      "TIGER",
      "Kerguelen",
      "King Salmon",
      "Unknown"};


   static String stationmid[]={ 
	"Wallops Island",
        "Unknown"};

   static String stationpolar[]={ 
        "Inuvik",
	"Rankin Inlet",
        "Unknown"};



    public static String toString(int id) {
      if (id < 0) return station[station.length-1];

      if ((id>=32) && (id<64)) {
        if ((id-32)>stationmid.length-1) return stationmid[station.length-1];
        return stationmid[id-32];
      }
      if ((id>=64) && (id<128)) {
        if ((id-64)>stationpolar.length-1) 
          return stationpolar[station.length-1];
        return stationpolar[id-64];
      }


      if (id > station.length-1) return station[station.length-1];
      return station[id];
    }

}

   


