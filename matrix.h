#ifndef _M_MATRIX_H
#define _M_MATRIX_H

#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <math.h>


#ifndef LARGE_ELEMENTS
#define LARGE_ELEMENTS 0
#endif

#if LARGE_ELEMENTS
typedef int m_elem;
#else
typedef char m_elem;
#endif

struct matrix{
    size_t row;
    size_t column;
    m_elem* content;
};

static struct matrix* init_matrix(size_t r, size_t c)
{
    struct matrix *m;

    m = malloc(sizeof(struct matrix));
    m->row = r;
    m->column = c;

    m->content = malloc(sizeof(m_elem) * r * c);

    bzero(m->content, sizeof(m_elem) * r * c);

    return m;
}

#define element(m, r, c) m->content[r * m->column + c]
#define element_point(m, r, c) (m->content + r * m->column + c)

static void destroy_matrix(struct matrix *mtrx)
{
    free(mtrx->content);
    free(mtrx);
}

static struct matrix *sub_matrix(struct matrix *mtrx, int x, int y)
{
    struct matrix *sub;
    int i, j;

    sub = init_matrix(mtrx->row - 1, mtrx->column - 1);
    for(i = 0; i < mtrx->row; i++)
	for(j = 0; j < mtrx->column; j++){
	    if(i == x || j == y)
		continue;
	    element(sub, (i < x ? i : i - 1), (j < y ? j : j - 1)) = element(mtrx, i, j);
	}

    return sub;
}

static m_elem det_matrix(struct matrix *mtrx)
{
    int i;
    m_elem p, s = 0;
    struct matrix *sub;

    if(mtrx->row == 1 && mtrx->column == 1)
	return element(mtrx, 0, 0);

    for(i = 0; i < mtrx->column; i++){
	sub = sub_matrix(mtrx, 0, i);
	p = pow(-1, i % 2);
	p *= element(mtrx, 0, i) * det_matrix(sub);
	s += p;
	destroy_matrix(sub);
    }

    return s;
    

}

static struct matrix *inv_matrix(struct matrix *mtrx)
{
    int i, j;
    struct matrix *inv, *sub;
    m_elem det;

    det = det_matrix(mtrx);
    inv = init_matrix(mtrx->column, mtrx->row);
    for(i = 0; i < inv->row; i++)
	for(j = 0; j < inv->column; j++){
	    sub = sub_matrix(mtrx, i, j);
	    element(inv, j, i) = det_matrix(sub) * pow(-1, (i + j) % 2);
	    element(inv, j, i) = element(inv, j, i) / det;
	    destroy_matrix(sub);
	}

    return inv;

}
static struct matrix* multiply_matrix(struct matrix *a, struct matrix *b)
{
    struct matrix* p;
    int i, j, k;

    if(a->column != b->row)
	return NULL;

    p = init_matrix(a->row, b->column);

    for(i = 0; i < a->row; i++)
	for(j = 0; j < b->column; j++)
	    for(k = 0; k < a->column; k++)
		element(p, i, j) += element(a, i, k) * element(b, k, j);

    return p;
}

static void set_matrix(struct matrix *a, m_elem *v)
{
    int i, j;

    for(i = 0; i < a->row; i++)
	for(j = 0; j < a->column; j++)
	    element(a, i, j) = v[i * a->column + j];
}



/* static struct matrix *reverse(struct matrix *m) */
/* { */
/*     struct matrix *om, *im; */

/*     if(m->row != m->column) */
/* 	return NULL; */

    
/* } */

#define SET_MATRIX(M, V) set_matrix(M, (m_elem *)V);

#endif
