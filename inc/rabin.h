#ifndef _M_RABIN_H
#define _M_RABIN_H

#define ELEMENT_TYPE typeof(unsigned long long)
//#define ELEMENT_TYPE typeof(unsigned)
#include "darray.h"

//typedef unsigned long long FP_T;
typedef unsigned FP_T;

void RF_BUF(char *b, size_t len, size_t win, int m, struct darray *da);
unsigned *RF_BUF_N(char *b, size_t len, size_t win, int m, int *sz);

#endif
