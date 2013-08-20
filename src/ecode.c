#include <stdlib.h>

#include "ecode.h"

#include <time.h>


static clock_t acc_m = 0;
static clock_t acc_a = 0;

#define ROLL_VOL 8

static void b_muladd_gf(register unsigned char *__restrict__ a, register unsigned char *__restrict__ b, unsigned char f, int sz, struct Gfield *gf)
{
    register char *__restrict__ mt = gf->mtable[f];
    unsigned char* lim = &b[sz - ROLL_VOL + 1];


#if (ROLL_VOL == 1)
    for(i = 0; i < sz; i++)
	 b[i] ^= mt[a[i]];
#endif

#if (ROLL_VOL > 1)
    for(;b < lim; a+=ROLL_VOL, b+=ROLL_VOL){
	b[0] ^= mt[a[0]];
	b[1] ^= mt[a[1]];
	b[2] ^= mt[a[2]];
	b[3] ^= mt[a[3]];
#if (ROLL_VOL > 4)
	b[4] ^= mt[a[4]];
	b[5] ^= mt[a[5]];
	b[6] ^= mt[a[6]];
	b[7] ^= mt[a[7]];
#if (ROLL_VOL > 8)
	b[8] ^= mt[a[8]];
	b[9] ^= mt[a[9]];
	b[10] ^= mt[a[10]];
	b[11] ^= mt[a[11]];
	b[12] ^= mt[a[12]];
	b[13] ^= mt[a[13]];
	b[14] ^= mt[a[14]];
	b[15] ^= mt[a[15]];
#endif	
#endif
    }

    lim += ROLL_VOL - 1;
    for(;b < lim; a++, b++)
	*b ^= mt[*a];
#endif

}

void b_mul_gf(unsigned char *a, int sz, unsigned char f, struct Gfield *gf, unsigned char *r)
{
    int i, j;
    clock_t st, ed;
    /* unsigned t[256]; */
    /* /\* st = clock(); *\/ */
    /* for(i = 0; i < 256; i++){ */
    /* 	for(j = 0; j < 256; j++){ */
    /* 	    if(j == f) */
    /* 		t[i] = gf->mtable[i][j]; */
    /* 	} */
    /* } */
    st = clock();
    for(i = 0; i < sz; i++){
	//r[i] = gf->mtable[(unsigned char)a[i]][(unsigned char)f];//gf->mul(a[i], f);
	r[i] = gf->mtable[a[i]][f];
	/* r[i] = gf->mtable[a[i]][f]; */
	/* r[i+1] = gf->mtable[a[i+1]][f]; */
	/* r[i+2] = gf->mtable[a[i+2]][f]; */
	/* r[i+3] = gf->mtable[a[i+3]][f]; */
	/* r[i] = t[a[i]]; */
    }
    ed = clock();
    acc_m += ( ed - st );
}

void b_add_gf(char *a, char *b, int sz, struct Gfield *gf, char*r)
{
    int i;
    clock_t st, ed;
    st = clock();
    for(i = 0; i < sz; i++)
	r[i] = (unsigned char)a[i] ^ (unsigned char)b[i];//gf->add(a[i], b[i]);
    ed = clock();
    acc_a += ( ed - st );
}
void _encode(struct ecode *e)
{
     int i, j;
     char *tmp, *s;
     clock_t st, ed;
     static clock_t acc = 0;
     size_t sz;


     /* source buff should be alighed to n */
     if(e->cmtrx == NULL)
          return;

     tmp = new_slot(&e->dst);

     sz = e->ssz / e->n;

     for(i = 0; i < e->m + e->n; i++){

	 s = new_slot(&e->dst);
	 st = clock();
	 for(j = 0; j < e->n; j++){
/* 	     b_mul_gf(e->src + sz * j, sz, element(e->cmtrx, i, j), e->gf, tmp); */
/* 	     b_add_gf(tmp, s, sz, e->gf, s); */
	     b_muladd_gf(e->src + sz * j, s, element(e->cmtrx, i, j), sz, e->gf);
	 }
	 enqueue_slot(&e->dst, s);
	 ed = clock();
	 acc += ( ed - st );
     }
     free(tmp);

/*      printf("%ld(%ld, %ld) ", acc, acc_a, acc_m); */
/*      printf("%ld(%.2f s) ", acc, acc * 1.0/ CLOCKS_PER_SEC); */
}

void _decode(struct ecode *e)
{
    int i, j, k;
    char *tmp;
    struct matrix *sub, *inv;
    struct darray valid;

    INIT_DARRAY(&valid);

    sub = init_matrix(e->n, e->n);
    for(i = 0, k = 0; i < e->dst.nr; i++){
	if(k == e->n) 
	    break;
	if(e->dst.slot[i] != NULL){
	    for(j = 0; j < e->cmtrx->column; j++)
		element(sub, k, j) = element(e->cmtrx, i, j);
	    push_darray(&valid, i);
	    k++;
	}
    }

    if(k != e->n){
	e->ssz = 0;
	destroy_matrix(sub);
	destroy_matrix(inv);
	clear_darray(&valid);
	return;
    }

    inv = inv_matrix_G(sub, e->gf);
    destroy_matrix(sub);

    e->src = realloc(e->src, e->dst.sz * e->n);
    e->ssz = e->dst.sz * e->n;
    tmp = malloc(e->dst.sz);
    memset(e->src, 0, e->ssz);

    for(i = 0; i < inv->row; i++){
	for(j = 0; j < e->n; j++){
/*                b_mul_gf(e->dst.slot[valid.e[j]], e->dst.sz, element(inv, i, j), e->gf, tmp); */
/*                b_add_gf(tmp, e->src + i * e->dst.sz, e->dst.sz, e->gf, e->src + i * e->dst.sz); */
	       b_muladd_gf(e->dst.slot[valid.e[j]], e->src + i * e->dst.sz, element(inv, i, j), e->dst.sz, e->gf);
          }
    }


    free(tmp);
    destroy_matrix(inv);
    clear_darray(&valid);
}

struct ecode *init_ecode(int w, int T, int n, int m)
{
     struct ecode *e;
     int i,j;

     e = malloc(sizeof(struct ecode));

     e->src = NULL;
     e->ssz = 0;

     e->dst.nr = 0;
     e->dst.slot = NULL;

     e->gf = init_Gfield(w);
     switch(T){
     case TYPE_RS:
	  e->cmtrx = rs_matrix(n, m, e->gf);
	  break;
     case TYPE_CAUCHY:
	  e->cmtrx = cauchy_matrix(n, m, e->gf);
	  break;
     default:
	  e->cmtrx = NULL;
     }

     e->m = m;
     e->n = n;
     e->encode = _encode;
     e->decode = _decode;

     return e;
}

void load_src_ecode(struct ecode *e, char *src, size_t ssz)
{
     int i;
     
     if(ssz % e->n != 0){
	  printf("buffer failed to aligh n \n");
	  exit(1);
     }

     if(e->dst.slot){
	  for(i = 0; i < e->dst.nr; i++)
	       free(e->dst.slot[i]);
	  free(e->dst.slot);
	  e->dst.slot = NULL;
	  e->dst.nr = 0;
     }

     init_slots(&e->dst, ssz / e->n);

     e->src = src;
     e->ssz = ssz;
}
