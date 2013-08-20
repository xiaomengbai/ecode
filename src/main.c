#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


#include "rabin.h"
#include "meta.h"
#include "index.h"
#include "ecode.h"
#include "fileop.h"
#include "col.h"
/* #include "sha.h" */
/* #include "md5.h" */
#include "hash.h"


void aligh_encode(struct ecode *e, FILE *f, size_t offset, size_t bsz);
void check_coded_blk(struct meta *mt, char *dname);

/* unsigned long long get_hash(char *b, size_t sz); */

int main(int argc, char *argv[])
{
    char *metafile = NULL;
    char *filename = NULL;
    char *output = NULL;
    char *process = NULL;
    char *rfile = NULL;
    size_t csz = 8;
    int n, m;
    FILE *f;

    struct ecode *e;
    struct meta *mt;
    struct darray efp;
    unsigned long long tfp, bfp;
    int i, j;
    char name[256];
    int recode_nr;
    char *cb;
    size_t cbsz;
    clock_t start, end;
    clock_t st, ed, acc = 0, acc2 = 0, acc3 = 0;
   struct timeval tm_st, tm_ed;
   struct timeval tm_st2, tm_ed2;
   double acc_tm1 = 0, acc_tm2 = 0, acc_tm3 = 0;
#define DIFF_TIME_VAL(t1, t2) ({double diff = 0; diff = t2.tv_sec - t1.tv_sec; diff += (t2.tv_usec - t1.tv_usec) * 0.000001; diff;})

    if(argc != 3)
	 return 1;

    process = argv[1];
    rfile = "a.out";
    metafile = "m";
    filename = "TMP";
    output = "output";
    n = 4;
    m = 2;
    csz = atol(argv[2]);//1024 * 1024 * 10;

    printf("*** Expected Chunk Size: %lu ***\n", csz);

    if(!strcmp(process, "decode")){
	printf("Decoding...\n");
	mt = read_meta(metafile);

	printf("Check if coded blocks exist...\n");
	start = clock();
	check_coded_blk(mt, output);
	end = clock();
	printf("done!(%.2f s)\n", (end - start) * 1.0/ CLOCKS_PER_SEC);

	for(i = 0; i < mt->map.nr; i++){
	    if(!mt->map.index[i]->cnr){
		printf("fail to decode: no coded file!");
		return 1;
	    }
	}

	e = init_ecode(8, TYPE_RS, mt->ea.n, mt->ea.m);
	f = fopen(rfile, "w");
	e->dst.nr = mt->ea.n + mt->ea.m;
	e->dst.slot = calloc(e->dst.nr, sizeof(char *));

	printf("Try decoding files...\n");
	start = clock();

	for(i = 0; i < mt->map.nr; i++){
	    if(mt->map.index[i]->cnr != e->dst.nr){
		printf("fail to decode: n/m is not consistent\n");
		return 1;
	    }
	    for(j = 0; j < mt->map.index[i]->cnr; j++){
		search_blk(mt->map.index[i]->cfp[j], output, &cb, &cbsz);
		if(!e->dst.sz && cbsz)
		    e->dst.sz = cbsz;
		e->dst.slot[j] = cb;
	    }
	    e->decode(e);
	    fwrite(e->src, 1, mt->map.index[i]->sz, f);
	    e->dst.sz = 0;
	    for(j = 0; j < e->dst.nr; j++)
		free(e->dst.slot[j]);
	}

	end = clock();
	printf("done!(%.2f s)\n", (end - start) * 1.0 / CLOCKS_PER_SEC);

	fclose(f);
    }else if(!strcmp(process, "encode")){
	printf("Encoding...\n");
	INIT_DARRAY(&efp);

	if(!filename){
	    printf("need input file!\n");
	    exit(1);
	}

	gettimeofday(&tm_st, NULL);
	if(!metafile || !(mt = read_meta(metafile)))
	    mt = init_meta(filename, n, m, 8);
	gettimeofday(&tm_ed, NULL);
	printf("Read meta file: %.2fs\n", DIFF_TIME_VAL(tm_st, tm_ed));


	printf("Try indexing the whole file...\n");fflush(stdout);
	gettimeofday(&tm_st, NULL);
	start = clock();
	index_file(mt, filename, csz);
	end = clock();
	gettimeofday(&tm_ed, NULL);
	printf("done!(%.2fs, %.2fs)\n", (end - start) * 1.0 / CLOCKS_PER_SEC, DIFF_TIME_VAL(tm_st, tm_ed));

/* 	_p_meta(mt); */
/* 	exit(1); */
	printf("Check if coded blocks exist...");fflush(stdout);
	start = clock();
	gettimeofday(&tm_st, NULL);
	check_coded_blk(mt, output);
	gettimeofday(&tm_ed, NULL);
	end = clock();
	printf("done!(%.2fs, %.2fs)\n", (end - start) * 1.0/ CLOCKS_PER_SEC, DIFF_TIME_VAL(tm_st, tm_ed));


	printf("Init encode matrix...");fflush(stdout);
	start = clock();
	gettimeofday(&tm_st, NULL);
	f = fopen(filename, "r");
	e = init_ecode(8, TYPE_RS, n, m);
	gettimeofday(&tm_ed, NULL);
	end = clock();
	printf("done!(%.2fs, %.2fs)\n", (end - start) * 1.0/ CLOCKS_PER_SEC, DIFF_TIME_VAL(tm_st, tm_ed));
	printf("Try encoding all chunks...\n");
	start = clock();
	gettimeofday(&tm_st, NULL);
	for(i = 0, recode_nr = 0; i < mt->map.nr; i++){
	    if(!mt->map.index[i]->cnr){
		 gettimeofday(&tm_st2, NULL);
		st = clock();
		aligh_encode(e, f, mt->map.index[i]->pos, mt->map.index[i]->sz);
		ed = clock();
		gettimeofday(&tm_ed2, NULL);
		acc += ( ed - st );
		acc_tm1 += DIFF_TIME_VAL(tm_st2, tm_ed2);
		
		for(j = 0; j < e->dst.nr; j++){
		    st = clock();
		    gettimeofday(&tm_st2, NULL);
/* 		    RF_BUF(e->dst.slot[j], e->dst.sz, e->dst.sz, 60, &efp); */
/* 		    push_cfp(mt->map.index[i], pop_darray(&efp)); */
		    bfp = get_hash(e->dst.slot[j], e->dst.sz);
		    push_cfp(mt->map.index[i], bfp);
		    gettimeofday(&tm_ed2, NULL);
		    ed = clock();
		    acc2 += (ed - st);
		    acc_tm2 += DIFF_TIME_VAL(tm_st2, tm_ed2);


		    st = clock();
		    gettimeofday(&tm_st2, NULL);
		    sprintf(name, "%llu.blk", mt->map.index[i]->cfp[j]);
/* 		    printf("write blk to %s\n", name); */
		    recode_nr++;
		    write_coded(mt->map.index[i]->cfp[j], e->dst.slot[j], e->dst.sz, output, name);
		    gettimeofday(&tm_ed2, NULL);
		    ed = clock();
		    acc3 += (ed - st);
		    acc_tm3 += DIFF_TIME_VAL(tm_st2, tm_ed2);
		}
	    }
	}
	gettimeofday(&tm_ed, NULL);
	end = clock();
	printf("done!(%.2fs, %.2fs)\n", (end - start) * 1.0/ CLOCKS_PER_SEC, DIFF_TIME_VAL(tm_st, tm_ed));
	printf("[performance: encode(%.2fs, %.2fs), hash(%.2fs, %.2fs), write blocks(%.2fs, %.2fs)\n", 
	       acc * 1.0 / CLOCKS_PER_SEC, acc_tm1,
	       acc2 * 1.0 / CLOCKS_PER_SEC, acc_tm2,
	       acc3 * 1.0 / CLOCKS_PER_SEC, acc_tm3);
	/* _p_meta(mt); */
	write_meta(mt, metafile);

	printf("recode %d blks\n", recode_nr);
    }
    return 0;
}

void aligh_encode(struct ecode *e, FILE *f, size_t offset, size_t bsz)
{
     char *tmp;
     size_t tsz, nr;
     int i;

     fseek(f, offset, SEEK_SET);

     tsz = bsz + (e->n - bsz % e->n) % e->n;

     tmp = malloc(tsz);
     memset(tmp, 0, tsz);
     nr = fread(tmp, 1, tsz, f);

     load_src_ecode(e, tmp, tsz);
     e->encode(e);
     free(tmp);
     e->src = NULL;
     e->ssz = 0;
}

void check_coded_blk(struct meta *mt, char *dname)
{
    int i, j, k;
    char *b;
    size_t sz;

    for(i = 0; i < mt->map.nr; i++){
	if(mt->map.index[i]->cnr){
	    for(j = 0, k = 0; j < mt->map.index[i]->cnr; j++){
		search_blk(mt->map.index[i]->cfp[j], dname, &b, &sz);
		if(sz == 0)
		    k++;
//		printf("%d blks not found in %d chunk\n", k, i);
		if(k > mt->ea.m){
		    clear_cfp(mt->map.index[i]);
		    break;
		}
	    }
	}
    }
}
