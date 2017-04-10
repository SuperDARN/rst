/* locate.h
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




struct file_list {
  int cnt;
  char **fname;
};


void free_locate(struct file_list *ptr);
int test_file_epoch(char *fname,double stime,double etime);

struct file_list *locate_files(char *fpath,char *prefix,
                               double stime,double etime);
