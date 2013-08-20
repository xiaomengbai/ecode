#ifndef _M_ECODE_H
#define _M_ECODE_H

#include "galois.h"
#include "matrix_code.h"
#include "slots.h"
#include "darray.h"
#include "gmatrix.h"

#define TYPE_RS 0
#define TYPE_CAUCHY 1


struct ecode {
     int n;
     int m;
     char *src;
     size_t ssz;
     struct matrix *cmtrx;
     struct Gfield *gf;
     struct slots dst;
     void (*encode)(struct ecode *e);
     void (*decode)(struct ecode *e);
};

struct ecode *init_ecode(int w, int T, int n, int m);
void load_src_ecode(struct ecode *e, char *src, size_t ssz);
#endif
