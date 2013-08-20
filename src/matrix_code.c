#include "matrix_code.h"

struct matrix *cauchy_matrix(int n, int m, struct Gfield *gf)
{
    struct matrix *mtrx;
    int i, j;
    char *X, *Y;
    
    if(n + m > pow(2, gf->w))
	return NULL;

    X = malloc(sizeof(int) * m);
    Y = malloc(sizeof(int) * n);

    i = 0;

    for(;i < m; i++)
	X[i] = i;
    for(j = 0; j < n; j++, i++)
	Y[j] = i;

#if _CODE_DEBUG
    printf("X:{");
    for(i = 0; i < m ; i++)
	printf("%d, ", X[i]);
    printf("}\n");

    printf("Y:{");
    for(i = 0; i < n ; i++)
	printf("%d, ", Y[i]);
    printf("}\n");
#endif

    mtrx = init_matrix(n + m, n);

    for(i = 0; i < mtrx->row; i++){
	for(j = 0; j < mtrx->column; j++){
	    if(i < n){
		if(j == i){
		    element(mtrx, i, j) = 1;
		}else{
		    element(mtrx, i, j) = 0;
		}
	    }else{
		 element(mtrx, i, j) = (unsigned char)X[i-n] ^ (unsigned char)Y[j];//gf->add(X[i-n], Y[j]);
		 element(mtrx, i, j) = gf->dtable[(unsigned char)1][(unsigned char)element(mtrx, i, j)];//gf->div(1, element(mtrx, i, j));
	    }
	}
    }

#if _CODE_DEBUG
    printf("Cauchy Matrix:\n");
    for(i = 0; i < mtrx->row; i++){
	for(j = 0; j < mtrx->column; j++)
	    printf("%3d ", element(mtrx, i, j));
	printf("\n");
    }
#endif

    return mtrx;
}

struct matrix *rs_matrix(int n, int m, struct Gfield *gf)
{
    struct matrix *mtrx;
    int i, j;
 
    mtrx = init_matrix(n + m, n);
    for(i = 0; i < mtrx->row; i++)
	for(j = 0; j < mtrx->column; j++){
	    element(mtrx, i, j) = gf->pow(j+1, i);
	}

    return mtrx;
}
