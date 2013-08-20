#ifndef _M_GMATRIX_H
#define _M_GMATRIX_H

#include "matrix.h"
#include "galois.h"

static struct matrix* multiply_matrix_G(struct matrix *a, struct matrix *b, struct Gfield *gf)
{
    struct matrix* p;
    int i, j, k;
    char r;

    if(a->column != b->row)
	return NULL;

    p = init_matrix(a->row, b->column);

    for(i = 0; i < a->row; i++)
	for(j = 0; j < b->column; j++)
	    for(k = 0; k < a->column; k++){
		 r = gf->mtable[(unsigned char)element(a, i, k)][(unsigned char)element(b, k, j)];//gf->mul(element(a, i, k), element(b, k, j));
		 element(p, i, j) = (unsigned char)element(p, i, j) ^ (unsigned char)r;//gf->add(element(p, i, j), r);
	    }

    return p;
}

static m_elem det_matrix_G(struct matrix *mtrx, struct Gfield *gf)
{
    int i;
    m_elem p, s = 0;
    struct matrix *sub;

    if(mtrx->row == 1 && mtrx->column == 1)
	return element(mtrx, 0, 0);

    for(i = 0; i < mtrx->column; i++){
	sub = sub_matrix(mtrx, 0, i);
	p = gf->mtable[(unsigned char)element(mtrx, 0, i)][(unsigned char)det_matrix_G(sub, gf)];//gf->mul(element(mtrx, 0, i), det_matrix_G(sub, gf));
	s = (unsigned char)s ^ (unsigned char)p; //gf->add(s, p);
	destroy_matrix(sub);
    }

    return s;
}

static struct matrix *inv_matrix_G(struct matrix *mtrx, struct Gfield *gf)
{
    int i, j;
    struct matrix *inv, *sub;
    m_elem det;

    det = det_matrix_G(mtrx, gf);
    inv = init_matrix(mtrx->column, mtrx->row);
    for(i = 0; i < inv->row; i++)
	for(j = 0; j < inv->column; j++){
	    sub = sub_matrix(mtrx, i, j);
	    element(inv, j, i) = det_matrix_G(sub, gf);
	    element(inv, j, i) = gf->dtable[(unsigned char)element(inv, j, i)][(unsigned char)det];//gf->div(element(inv, j, i), det);
	    destroy_matrix(sub);
	}

    return inv;
}


#endif
