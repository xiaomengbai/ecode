#ifndef _M_COL_H
#define _M_COL_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "meta.h"
#include "fileop.h"

struct files{
    size_t nr;
    char **names;
};

#define INIT_FILES(mfs) ({(mfs)->nr = 0; (mfs)->names = NULL;})

void load_names(char *dirname, struct files *fs);
void write_coded(unsigned long long fp, char *b, size_t sz, char *dname, char *fname);
void read_coded(unsigned long long *fp, char *b, size_t sz, char *dname, char *fname);
unsigned long long read_fp(char *dname, char *fname);
void search_blk(unsigned long long fp, char *dname, char **b, size_t *sz);


#endif
