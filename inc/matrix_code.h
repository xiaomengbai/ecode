#ifndef _M_CODE_MATRIX_H
#define _M_CODE_MATRIX_H

#include "matrix.h"
#include "galois.h"
#include <math.h>
#include <string.h>

#define _CODE_DEBUG 0

#if _CODE_DEBUG
#include <stdio.h>
#endif

struct matrix *cauchy_matrix(int n, int m, struct Gfield *gf);
struct matrix *rs_matrix(int n, int m, struct Gfield *gf);

#endif
