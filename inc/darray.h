#ifndef _M_DARRAY_H
#define _M_DARRAY_H

#include <stdlib.h>
// Dynamic array
#ifndef ELEMENT_TYPE
#define ELEMENT_TYPE typeof(unsigned long long)
#endif

struct darray{
    int nr;
    ELEMENT_TYPE *e;
};

#define INIT_DA(da) ({ \
	       da->nr = 0; \
	       da->e = NULL; \
	  })

/* #define INSERT_DA(da, a, pos) ({ \ */
/* 	       int i = da->nr; \ */
/* 	       da->nr++; \ */
/* 	       da->e = realloc(da->e, sizeof(ELEMENT_TYPE) * da->nr); \ */

static inline void INIT_DARRAY(struct darray *da)
{
    da->nr = 0;
    da->e = NULL;
}

static void insert_darray(struct darray *da, ELEMENT_TYPE a, int pos)
{
     int i = da->nr;
     if(pos >= da->nr)
	  return;
     da->nr++;
     da->e = realloc(da->e, sizeof(ELEMENT_TYPE) * da->nr);
     while(i != pos){
	  da->e[i] = da->e[i - 1];
	  i--;
     }
     da->e[pos] = a;
}

static ELEMENT_TYPE delete_darray(struct darray *da, int pos)
{
     int ret, i = pos;
     if(pos >= da->nr)
	  return;
     ret = da->e[pos];
     da->nr--;
     while(i != da->nr){
	  da->e[i] = da->e[i + 1];
	  i++;
     }
     da->e = realloc(da->e, sizeof(ELEMENT_TYPE) * da->nr);
     return ret;
}

static inline void push_darray(struct darray *da, ELEMENT_TYPE a)
{
    da->nr++;
    da->e = realloc(da->e, sizeof(ELEMENT_TYPE) * da->nr);
    da->e[da->nr - 1] = a;
}

static ELEMENT_TYPE shift_darray(struct darray *da)
{
    ELEMENT_TYPE ret;
    int i;
    da->nr--;
    ret = da->e[0];
    for(i = 0; i < da->nr; i++)
	da->e[i] = da->e[i+1];
    da->e = realloc(da->e, sizeof(ELEMENT_TYPE) * da->nr);
    return ret;
}

static ELEMENT_TYPE pop_darray(struct darray *da)
{
    ELEMENT_TYPE ret;
    da->nr--;
    ret = da->e[da->nr];
    da->e = realloc(da->e, sizeof(ELEMENT_TYPE) * da->nr);
    return ret;
}


static inline void clear_darray(struct darray *da)
{
    /* if(da && da->e) */
    /* 	free(da->e); */
    if(da->nr){
	free(da->e);
	da->e = NULL;
	da->nr = 0;
    }
}

#endif
