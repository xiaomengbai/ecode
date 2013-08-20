#include <stdlib.h>

#include "galois.h"

static unsigned char mul_table_4[16][16] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {0, 2, 4, 6, 8, 10, 12, 14, 3, 1, 7, 5, 11, 9, 15, 13},
    {0, 3, 6, 5, 12, 15, 10, 9, 11, 8, 13, 14, 7, 4, 1, 2},
    {0, 4, 8, 12, 3, 7, 11, 15, 6, 2, 14, 10, 5, 1, 13, 9},
    {0, 5, 10, 15, 7, 2, 13, 8, 14, 11, 4, 1, 9, 12, 3, 6},
    {0, 6, 12, 10, 11, 13, 7, 1, 5, 3, 9, 15, 14, 8, 2, 4},
    {0, 7, 14, 9, 15, 8, 1, 6, 13, 10, 3, 4, 2, 5, 12, 11},
    {0, 8, 3, 11, 6, 14, 5, 13, 12, 4, 15, 7, 10, 2, 9, 1},
    {0, 9, 1, 8, 2, 11, 3, 10, 4, 13, 5, 12, 6, 15, 7, 14},
    {0, 10, 7, 13, 14, 4, 9, 3, 15, 5, 8, 2, 1, 11, 6, 12},
    {0, 11, 5, 14, 10, 1, 15, 4, 7, 12, 2, 9, 13, 6, 8, 3},
    {0, 12, 11, 7, 5, 9, 14, 2, 10, 6, 1, 13, 15, 3, 4, 8},
    {0, 13, 9, 4, 1, 12, 8, 5, 2, 15, 11, 6, 3, 14, 10, 7},
    {0, 14, 15, 1, 13, 3, 2, 12, 9, 7, 6, 8, 4, 10, 11, 5},
    {0, 15, 13, 2, 9, 6, 4, 11, 1, 14, 12, 3, 8, 7, 5, 10}
};

static unsigned char div_table_4[16][16] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 9, 14, 13, 11, 7, 6, 15, 2, 12, 5, 10, 4, 3, 8},
    {0, 2, 1, 15, 9, 5, 14, 12, 13, 4, 11, 10, 7, 8, 6, 3},
    {0, 3, 8, 1, 4, 14, 9, 10, 2, 6, 7, 15, 13, 12, 5, 11},
    {0, 4, 2, 13, 1, 10, 15, 11, 9, 8, 5, 7, 14, 3, 12, 6},
    {0, 5, 11, 3, 12, 1, 8, 13, 6, 10, 9, 2, 4, 7, 15, 14},
    {0, 6, 3, 2, 8, 15, 1, 7, 4, 12, 14, 13, 9, 11, 10, 5},
    {0, 7, 10, 12, 5, 4, 6, 1, 11, 14, 2, 8, 3, 15, 9, 13},
    {0, 8, 4, 9, 2, 7, 13, 5, 1, 3, 10, 14, 15, 6, 11, 12},
    {0, 9, 13, 7, 15, 12, 10, 3, 14, 1, 6, 11, 5, 2, 8, 4},
    {0, 10, 5, 6, 11, 2, 3, 9, 12, 7, 1, 4, 8, 14, 13, 15},
    {0, 11, 12, 8, 6, 9, 4, 15, 3, 5, 13, 1, 2, 10, 14, 7},
    {0, 12, 6, 4, 3, 13, 2, 14, 8, 11, 15, 9, 1, 5, 7, 10},
    {0, 13, 15, 10, 14, 6, 5, 8, 7, 9, 3, 12, 11, 1, 4, 2},
    {0, 14, 7, 11, 10, 8, 12, 2, 5, 15, 4, 3, 6, 13, 1, 9},
    {0, 15, 14, 5, 7, 3, 11, 4, 10, 13, 8, 6, 12, 9, 2, 1}
};

/* used to generate GF(256) */
#define GF_RANGE_8 0x100 /* 256 */
#define GF_BITS_8 8
#define EXCEED_RANGE_8(r) ((r) & ~(GF_RANGE_8 - 1))

static int prim_8 = 0x11d; //x^8 + x^4 + x^3 + x^2 + 1 100011101
static unsigned char mul_table_8[256][256];
static unsigned char div_table_8[256][256];
/* GF(256) END */				      

/* 
 * used to do polynominal multiplication, whose addition is in GF(2)
 *
 * e.g. (1100) * (0111) = (x^3 + x^2)(x^2 + x + 1)
 *                      = x^5 + 2 * x^4 + 2 * x^3 + x^2
 *                      = x^5 + x^2 = 100100
 */
static int _mul_poly(int a, int b)
{
    int r, t;
    for(t = 0, r = 0; t < sizeof(int) * 8; t++)
	if((a >> t) & 0x1)
	    r ^= (b << t);
    return r;
}

static unsigned char add_8(unsigned char a, unsigned char b)
{
    return a ^ b;
}

static unsigned char sub_8(unsigned char a, unsigned char b)
{
    return a ^ b;
}

static unsigned char mul_8(unsigned char a, unsigned char b)
{
    return mul_table_8[a][b];
}

static unsigned char div_8(unsigned char a, unsigned char b)
{
    return div_table_8[a][b];
}

static unsigned char pow_8(unsigned char a, unsigned char b)
{
    char i, r;

    //pow(0, 0) should not happen
    if(a == 0 && b == 0)
	return 0;

    for(r = 1, i = 0; i < b; i++)
	r = mul_8(a, r);
    
    return r;
}

static unsigned char add_4(unsigned char a, unsigned char b)
{
    return a ^ b;
}

static unsigned char sub_4(unsigned char a, unsigned char b)
{
    return a ^ b;
}

static unsigned char mul_4(unsigned char a, unsigned char b)
{
    char a_h, a_l, b_h, b_l;

    a_l = a & 0xf;
    a_h = (a & 0xf0) >> 4;
    b_l = b & 0xf;
    b_h = (b & 0xf0) >> 4;

    return mul_table_4[a_l][b_l] | (mul_table_4[a_h][b_h] << 4);
}

static unsigned char div_4(unsigned char a, unsigned char b)
{
    char a_h, a_l, b_h, b_l;

    a_l = a & 0xf;
    a_h = (a & 0xf0) >> 4;
    b_l = b & 0xf;
    b_h = (b & 0xf0) >> 4;

    return div_table_4[a_l][b_l] | (div_table_4[a_h][b_h] << 4);
}

static unsigned char pow_4(unsigned char a, unsigned char b)
{
    char i, r;

    //pow(0, 0) should not happen
    if(a == 0 && b == 0)
	return 0;

    for(r = 1, i = 0; i < b; i++)
	r = mul_4(a, r);
    
    return r;
}

static void _init_gf_8_table(struct Gfield *gf)
{
    int i, j, k;
    int r, a, t;

    for(i = 0; i < GF_RANGE_8; i++)
	for(j = 0; j < GF_RANGE_8; j++){
	    r = _mul_poly(i, j);
	    for(t = 0; ; t++){
		a = _mul_poly(t, prim_8);
		if(!EXCEED_RANGE_8(r ^ a))
		    break;
	    }
	    mul_table_8[i][j] = r ^ a;
	    gf->mtable[i][j] = r ^ a;
	}

    for(i = 0; i < GF_RANGE_8; i++)
	for(j = 0; j < GF_RANGE_8; j++)
	    for(k = 0; k < GF_RANGE_8; k++){
		if(mul_table_8[k][j] == i){
		    div_table_8[i][j] = k;
		    gf->dtable[i][j] = k;
		    break;
		}
	    }
}

struct Gfield *init_Gfield(int w)
{
    struct Gfield *Gf;
    
    Gf = malloc(sizeof(struct Gfield));

    Gf->w = w;
    switch(w){
    case 4:
	Gf->add = add_4;
	Gf->sub = sub_4;
	Gf->mul = mul_4;
	Gf->div = div_4;
	Gf->pow = pow_4;
	break;
    case 8:
	_init_gf_8_table(Gf);
	Gf->add = add_8;
	Gf->sub = sub_8;
	Gf->mul = mul_8;
	Gf->div = div_8;
	Gf->pow = pow_8;
	break;
    default:
	return NULL;
    }

    return Gf;
}

void destroy_Gfield(struct Gfield *gf)
{
    free(gf);
}
