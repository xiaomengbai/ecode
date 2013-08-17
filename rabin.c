#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include "rabin.h"

#define CFS_SZ 100
#define P 1048583

static FP_T MASK;
static int M;

FP_T pow_llu(int x, int y);
FP_T RF_INC(char *b, size_t len, FP_T fp, FP_T *coefs);
FP_T RF_CAL(char *b, size_t len, FP_T *coefs);
FP_T *init_CFS(size_t sz, int m);

unsigned *RF_BUF_N(char *b, size_t len, size_t win, int m, int *sz)
{
    FP_T *coefs;
    FP_T fp;
    int i;
    unsigned *fps;
    
    *sz = len - win + 1;
    fps = malloc(sizeof(unsigned) * (*sz));
    memset(fps, 1, sizeof(unsigned) * (*sz));
    if(win > len)
	return;
    coefs = init_CFS(win, m);

    fp = RF_CAL(b, win, coefs);
    fps[0] = (unsigned)fp;
    for(i = 0; i < len - win; i++){
    	fp = RF_INC(b+i, win, fp, coefs);
	fps[i + 1] = (unsigned)fp;
    }
    free(coefs);
    return fps;
}


void RF_BUF(char *b, size_t len, size_t win, int m, struct darray *da)
{
    FP_T *coefs;
    FP_T fp;
    int i;
    
    if(win > len)
	return;
    coefs = init_CFS(win, m);

    fp = RF_CAL(b, win, coefs);
    push_darray(da, fp);
    for(i = 0; i < len - win; i++){
    	fp = RF_INC(b+i, win, fp, coefs);
	push_darray(da, fp);
    }
    free(coefs);
}

FP_T pow_llu(int x, int y)
{
    if(y == 0)
	return 1;
    else
	return pow_llu(x, y-1) * x;
}


FP_T *init_CFS(size_t sz, int m)
{
    int i;
    FP_T *coefs;

    M = m;
    coefs = malloc(sizeof(FP_T) * sz);
    MASK = pow_llu(2, M) - 1;
    for(i = 0; i < sz; i++){
	if(i == 0)
	    coefs[i] = 1;
	else
	    coefs[i] = (coefs[i - 1] * P) & MASK;
    }
    return coefs;
}


FP_T RF_INC(char *b, size_t len, FP_T fp, FP_T *coefs)
{
    return (( fp - b[0] * coefs[len - 1] ) * P + b[len] ) & MASK;
}    

FP_T RF_CAL(char *b, size_t len, FP_T *coefs)
{
    int i, j; 
    FP_T FP = 0;
    for(i = 0, j = len - 1; i < len; i++, j--)
	FP += coefs[j] * b[i];
    return FP & MASK;
}


