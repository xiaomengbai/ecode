#ifndef _M_INDEX_H
#define _M_INDEX_H
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "fileop.h"
#include "darray.h"
#include "rabin.h"
#include "meta.h"
#include "hash.h"

void index_file(struct meta *mt, char *fname, size_t csz);


#endif
