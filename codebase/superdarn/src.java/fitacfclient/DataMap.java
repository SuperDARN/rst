/* DataMap.java
   ============ 
   Author: R.J.Barnes
*/

/*
 $License$
*/



import java.util.*;

class DataMapScalar {
    String name;
    int type;
    Object value;

    public DataMapScalar(String name,int type,Object value) {
	this.name=name;
        this.type=type;
        this.value=value;
    }
}

class DataMapArray {
    String name;
    int type;
    int dim;
    int rng[];
    Object value[];

    public DataMapArray(String name,int type,int dim,int rng[],
			 Object value[]) {
	this.name=name;
        this.type=type;
        this.dim=dim;
        this.rng=rng;
        this.value=value;                   
    }
}




public class DataMap {
    int snum;
    int anum;

    Vector scalar=new Vector(100);
    Vector array=new Vector(100);

    public static final int DATACHAR=1;
    public static final int DATASHORT=2;
    public static final int DATALONG=3;
    public static final int DATAFLOAT=4;
    public static final int DATADOUBLE=8;
    public static final int DATASTRING=9;

   public boolean decode(byte buf[],int off)  {
       int c,n,t,x;       
       int size;
       String name;
       int type,dim;
       int rng[];
       DataMapScalar scl;
       DataMapArray arr;
       Object obj[];
       size=buf.length;

       off+=8; /* skip header */
      
       snum=MathConvert.lng(buf,off); off+=4;
       anum=MathConvert.lng(buf,off); off+=4;
  
       scalar.removeAllElements();
       array.removeAllElements();

       for (c=0;c<snum;c++) {
           n=0;
	   while ((buf[off+n] !=0) && (off+n<size)) n++;
           if (off+n>=size) break;
           name=new String(buf,off,n);
           off+=n+1;
           type=buf[off];
           off++;
           switch (type) {
           case DATACHAR:
               scl=new DataMapScalar(name,type,
                                     new Character((char) buf[off]));
	       scalar.addElement(scl);
               off++;
               break;
           case DATASHORT:
               scl=new DataMapScalar(name,type,
				     new Short(MathConvert.shrt(buf,off)));
	       scalar.addElement(scl);
               off+=2;
               break;
           case DATALONG:
               scl=new DataMapScalar(name,type,
				     new Integer(MathConvert.lng(buf,off)));
	       scalar.addElement(scl);

               off+=4;
               break;

           case DATAFLOAT:
               scl=new DataMapScalar(name,type,
				     new Float(MathConvert.flt(buf,off)));
	       scalar.addElement(scl);
               off+=4;
               break;

           case DATADOUBLE:
               scl=new DataMapScalar(name,type,
				     new Double(MathConvert.dbl(buf,off)));
	       scalar.addElement(scl);
               off+=8;
               break;
           default:
               n=0;
	       while ((buf[off+n] !=0) && (off+n<size)) n++;
               if (off+n>=size) break;
               scl=new DataMapScalar(name,type,
				     new String(buf,off,n));
	       scalar.addElement(scl);

               off+=n+1;
	   } 
       }
       if (c !=snum) return false;
       for (c=0;c<anum;c++) {
         n=0;
	 while ((buf[off+n] !=0) && (off+n<size)) n++;
         if (off+n>=size) break;
         name=new String(buf,off,n);
         off+=n+1;
         type=buf[off];
         off++;
         dim=MathConvert.lng(buf,off);
         off+=4;
         rng=new int[dim];
         t=1;
         for (n=0;n<dim;n++) {
	     rng[n]=MathConvert.lng(buf,off);
             t=t*rng[n];
             off+=4;
	 }
         obj=new Object[t];
         switch (type) {
         case DATACHAR:
	     for (n=0;n<t;n++) {
                obj[n]=new Character((char) buf[off]);
                off++;
	     }
             break;
         case DATASHORT:
	     for (n=0;n<t;n++) {
		obj[n]=new Short(MathConvert.shrt(buf,off));
                off+=2;
	     }
             break;
	
         case DATALONG:
	     for (n=0;n<t;n++) {
		obj[n]=new Integer(MathConvert.lng(buf,off));
                off+=4;
	     }
             break;
         case DATAFLOAT:
	     for (n=0;n<t;n++) {
		obj[n]=new Float(MathConvert.flt(buf,off));
                off+=4;
	     }
             break;
	 case DATADOUBLE:
	     for (n=0;n<t;n++) {
		obj[n]=new Double(MathConvert.dbl(buf,off));
                off+=8;
	     }
             break;
	 default:
	     for (n=0;n<t;n++) {
               x=0;
	       while ((buf[off+x] !=0) && (off+x<size)) x++;
               if (off+x>=size) break;
	       obj[n]=new String(buf,off,x);
               off+=x+1;
	     }
             if (n !=t) break;
             break;


	 }
         arr=new DataMapArray(name,type,dim,rng,obj);
	 array.addElement(arr);
       }
       if (c !=anum) return false;
       return true;
   }
    
}
