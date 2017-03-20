/* dmap.h
   ====== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _DMAP_H
#define _DMAP_H

#define DATACHAR 1
#define DATASHORT 2
#define DATAINT 3
#define DATAFLOAT 4
#define DATADOUBLE 8
#define DATASTRING 9
#define DATALONG 10

#define DATAUCHAR 16
#define DATAUSHORT 17
#define DATAUINT 18
#define DATAULONG 19

#define DATAMAP 255

/* Define an unique code value for this version of the library. */

#define DATACODE  0x00010001

struct DataMapFp {
  int zflg,sflg;
  int size;
  union {
    FILE *f;
    gzFile *z;
  } fp;
};


union DataMapPointer {
  char *cptr;
  int16 *sptr;
  int32 *iptr;
  int64 *lptr;

  unsigned char *ucptr;
  uint16 *usptr;
  uint32 *uiptr;
  uint64 *ulptr;

  float *fptr;
  double *dptr;
  
  struct DataMap **mptr;

  void *vptr;

};

struct DataMapScalar {
  char *name;
  unsigned char type;
  unsigned char mode;
  union DataMapPointer data;
};

struct DataMapArray {
    char *name;
    unsigned char type;
    unsigned char mode;
    int32 dim;
    int32 *rng;
    union DataMapPointer data;
};

struct DataMap {
  int snum;
  int anum;
  struct DataMapScalar **scl;
  struct DataMapArray **arr;
}; 


struct DataMap *DataMapMake();

struct DataMapScalar* DataMapMakeScalar(char *name,int mode,int type,
                                                void *data);
 
int DataMapTestScalar(struct DataMapScalar *ptr,char *name,int type);

void DataMapFreeScalar(struct DataMapScalar *ptr);

int DataMapAddScalar(struct DataMap *ptr,
                         char *name,int type,void *data);


void *DataMapStoreScalar(struct DataMap *ptr,
		       char *name,int type,void *data);


int DataMapRemoveScalar(struct DataMap *ptr,char *name,int type);

void *DataMapFindScalar(struct DataMap *ptr,char *name,int type);


int DataMapSetFreeScalar(struct DataMap *ptr,char *name,int type);


struct DataMapArray* DataMapMakeArray(char *name,int mode,int type,int dim,
                                      int32 *rng,void *data);


int DataMapTestArray(struct DataMapArray *ptr,char *name,int type,int dim);

void DataMapFreeArray(struct DataMapArray *ptr);
 
int DataMapAddArray(struct DataMap *ptr,
                    char *name,int type,int dim,
                    int32 *rng,void *data);
 
void *DataMapStoreArray(struct DataMap *ptr,
			char *name,int type,int dim,int32 *rng,void *data);


int DataMapRemoveArray(struct DataMap *ptr,char *name,int type,int dim);

void *DataMapFindArray(struct DataMap *ptr,char *name,int type,int dim,
		       int **rng);


int DataMapSetFreeArray(struct DataMap *ptr,char *name,int type,int dim);

int DataMapSize(struct DataMap *write);

char *DataMapEncodeBuffer(struct DataMap *ptr,int *size);

int DataMapWrite(int fid,struct DataMap *ptr);
 
void DataMapFree(struct DataMap *ptr);
struct DataMap *DataMapRead(int fid);

int DataMapFwrite(FILE *fp,struct DataMap *ptr);


struct DataMap *DataMapFread(FILE *fp);

struct DataMap *DataMapDecodeBuffer(char *buf,int size);

struct DataMap *DataMapReadBlock(int fid,int *s);
struct DataMap *DataMapFreadBlock(FILE *fp,int *s);


struct DataMap *DataMapReadBlockZ(gzFile file,int *s);
struct DataMap *DataMapReadZ(gzFile file);
int DataMapWriteZ(gzFile file,struct DataMap *ptr);


struct DataMapArray *DataMapMergeArray(char *name,
              struct DataMapArray *x,struct DataMapArray *y);

struct DataMap *DataMapMerge(int num,struct DataMap **in);


struct DataMapFp *DataMapOpen(char *fname,int zflg,char *mode);
void DataMapClose(struct DataMapFp *fp);
struct DataMap *DataMapGet(struct DataMapFp *fp);
int DataMapPut(struct DataMapFp *fp,struct DataMap *ptr);


#endif
