
#include "index.h"

#include <time.h>
#include <limits.h>
#include <pthread.h>

#define GET_MASK(sz, csz, wz) ({					\
	    int n = 1, m = 0;						\
	    while((n << m) < ((sz - wz + 1) / ceil(sz * 1.0 / csz)))	\
		m++;							\
	    m;})

static double get_var(struct darray *fp, FP_T target, size_t avg);
/* static FP_T opt_fp(struct darray *fp, size_t sz, int m); */
static FP_T opt_fp_t(unsigned *fps, int fps_nr, unsigned *tps, size_t sz, int m);
static FP_T opt_fp(unsigned *fps, int fps_nr, unsigned *tps, long /* size_t */ sz, int m);
/* int main(int argc, char *argv[]) */
/* { */
/*     return 0; */
/* } */

static clock_t acc_1 = 0;
static clock_t acc_2 = 0;
static clock_t acc_3 = 0;
static clock_t acc_4 = 0;

struct thread_args{
    unsigned *fps;
    int fps_nr;
    size_t esz;
    int r;
    int s; 
    double minvar;
    unsigned cand;
};

void *thread_cal_var(void *arg)
{
    struct thread_args *tap;
    double *vs;
    unsigned *vsnr, *last;
    int i;
    double var;

    tap = arg;
    /* printf(">>Thread %lu: \n", pthread_self()); */
    /* printf("  >>Candidates Range: %d --> %d\n", tap->s, tap->s + tap->r); */
    tap->minvar = -1;

    vs = malloc(sizeof(double) * tap->r);
    last = malloc(sizeof(unsigned) * tap->r);
    vsnr = malloc(sizeof(unsigned) * tap->r);
    assert(vs != NULL && last != NULL && vsnr != NULL);
    
    memset(vs, 0, sizeof(double) * tap->r);
    memset(last, 0, sizeof(unsigned) * tap->r);
    memset(vsnr, 0, sizeof(unsigned) * tap->r);

    for(i = 0; i < tap->fps_nr; i++){
	if(tap->fps[i] < tap->s || tap->fps[i] >= tap->s + tap->r)
	    continue;
	vsnr[tap->fps[i] - tap->s]++;
	vs[tap->fps[i] - tap->s] += powl((long)(i - last[tap->fps[i] - tap->s]) - (long)tap->esz, 2);
	last[tap->fps[i] - tap->s] = i;
    }

    for(i = tap->s; i < tap->s + tap->r; i++){
	vsnr[i - tap->s]++;
	vs[i - tap->s] += pow((long)(tap->fps_nr - last[i - tap->s]) - (long)tap->esz, 2);

	var = sqrt(vs[i - tap->s]) / vsnr[i - tap->s];

	if(tap->minvar == -1 || var < tap->minvar){
	    tap->minvar = var;
	    tap->cand = i;
	}

    }

    free(last);
    free(vs);
    free(vsnr);
    pthread_exit(tap);

}

static int _window_size;

void index_file(struct meta *mt, char *fname, size_t csz)
{
    FP_T flag, tfp;
    size_t sz, offset;
    int mask, i, j;
    struct darray fp, bfp;
    unsigned *fps;
    int fps_nr;
    unsigned long long hfp;
    char *b;
    struct chunks cks;
    struct chunk *tck;
    clock_t st, ed;
    clock_t sto, edo;

    unsigned *tps;

    INIT_DARRAY(&bfp);
    INIT_DARRAY(&fp);


    sz = file_size(fname);
    mask = GET_MASK(sz, csz, mt->ra.wsz);
    b = malloc(sz);
    printf("load file...");
    load_file(fname, b);
    printf("done\n");
    printf("Rabin Fingerprinting...");fflush(stdout);
    st = clock();
    if(mt->ra.flag == -1 || mask != mt->ra.mask){
	mt->ra.mask = mask;
	/* RF_BUF(b, sz, mt->ra.wsz, mt->ra.mask, &fp); */
	fps = RF_BUF_N(b, sz, mt->ra.wsz, mt->ra.mask, &fps_nr);
	_window_size = mt->ra.wsz;
	tps = malloc(sizeof(unsigned) * fps_nr);
	memset(tps, 1, sizeof(unsigned) * fps_nr);
	ed = clock();
	acc_1 += (ed - st);
	printf("done\n");
	printf("Calculating new flag...\n");
	st = clock();
	/* mt->ra.flag = opt_fp(&fp, csz, mt->ra.mask); */

	mt->ra.flag = opt_fp(fps, fps_nr, tps, csz, mt->ra.mask);
	/* mt->ra.flag = 0; */

	/* mt->ra.flag = opt_fp_t(fps, fps_nr, tps, csz, mt->ra.mask); */
	ed = clock();
	acc_4 += (ed - st);
	printf("done\n");
    }else{
	/* RF_BUF(b, sz, mt->ra.wsz, mt->ra.mask, &fp); */
	fps = RF_BUF_N(b, sz, mt->ra.wsz, mt->ra.mask, &fps_nr);
	_window_size = mt->ra.wsz;
	ed = clock();
	acc_1 += (ed - st);
	printf("done\n");
    }

    printf("flag is %llu\n", mt->ra.flag);
    
    printf("Hash(MD5/SHA1) all chunks...");
    st = clock();
    /* for(i = 0, offset = 0, cks.nr = 0, cks.index = NULL; i < fp.nr; i++){ */
    for(i = 0, offset = 0, cks.nr = 0, cks.index = NULL; i < fps_nr; i++){
	/* if(fp.e[i] != mt->ra.flag) */
	if(fps[i] != mt->ra.flag)
	    continue;
/* 	RF_BUF(b + offset, i + mt->ra.wsz - offset, i + mt->ra.wsz - offset, 60, &bfp); */
/* 	tfp = pop_darray(&bfp); */
/* 	tck = new_chunk(offset, i + mt->ra.wsz - offset, tfp); */
	hfp = get_hash(b + offset, i + mt->ra.wsz - offset);
	tck = new_chunk(offset, i + mt->ra.wsz - offset, hfp);
	push_chunk(&cks, tck);
	offset = i + mt->ra.wsz;
	i += mt->ra.wsz;
    }
    /* RF_BUF(b + offset, sz - offset, sz - offset, 60, &bfp); */
    /* tfp = pop_darray(&bfp); */
    /* tck = new_chunk(offset, sz - offset, tfp); */
    hfp = get_hash(b + offset, sz - offset);
    tck = new_chunk(offset, sz - offset, hfp);
    push_chunk(&cks, tck);
    ed = clock();
    acc_2 += (ed - st);
    printf("done\n");
    printf(" -Plan to encode into %lu files\n", cks.nr * (mt->ea.n + mt->ea.m));

    printf("Match hash to original ones...");
    st = clock();
    for(i = 0; i < cks.nr; i++){
	for(j = 0; j < mt->map.nr; j++){
	    if(cks.index[i]->fp == mt->map.index[j]->fp){
		if(mt->map.index[j]->cnr == mt->ea.n + mt->ea.m){
		    cks.index[i]->cnr = mt->map.index[j]->cnr;
		    cks.index[i]->cfp = malloc(sizeof(unsigned long long) * cks.index[i]->cnr);
		    memcpy(cks.index[i]->cfp, mt->map.index[j]->cfp, sizeof(unsigned long long) * cks.index[i]->cnr);
		}
		break;
	    }
	}
    }
    ed = clock();
    acc_3 += (ed - st);
    printf("done\n");

    clear_chunks(&mt->map);
    mt->map.nr = cks.nr;
    mt->map.index = malloc(sizeof(struct chunk *) * cks.nr);
    memcpy(mt->map.index, cks.index, sizeof(struct chunk *) * cks.nr);

    free(cks.index);
    
    clear_darray(&bfp);
    clear_darray(&fp);
    free(b);

    printf("STAT: [(rabin)%lu[%.2f] (opt_fp)%lu[%.2f] (hashing)%lu[%.2f] (matching)%lu[%.2f]]\n", 
	   acc_1, acc_1 * 1.0 / CLOCKS_PER_SEC, 
	   acc_4, acc_4 * 1.0 / CLOCKS_PER_SEC,
	   acc_2, acc_2 * 1.0 / CLOCKS_PER_SEC,
	   acc_3, acc_3 * 1.0 / CLOCKS_PER_SEC);
    /* exit(1); */
}

static FP_T opt_fp_t(unsigned *fps, int fps_nr, unsigned *tps, size_t sz, int m)
{
#define T_NR 2
    int i, r, ret;
    pthread_t t[T_NR];
    struct thread_args ta[T_NR];
    unsigned range = 1, cand;
    double minvar = -1;

    clock_t st, ed;

    for(i = 1; i <= m; i++)
	range *= 2;

    r = ceil(range * 1.0 / T_NR);
    printf(" -Candidate Range (0 - %u)\n", range);
    
    for(i = 0; i < T_NR; i++){
	ta[i].fps = fps;
	ta[i].fps_nr = fps_nr;
	ta[i].esz = sz;
	ta[i].s = i * r;
	if((i+1) * r > range)
	    ta[i].r = range - ta[i].s;
	else
	    ta[i].r = r;
	ta[i].cand = 0;
    }

    st = clock();
    for(i = 0; i < T_NR; i++){
	if(ret = pthread_create(&t[i], NULL, thread_cal_var, &ta[i])){
	    printf("Creat Thread %d error: %s\n", i, strerror(ret));
	    exit(1);
	}
    }
	    
    for(i = 0; i < T_NR; i++){
	if(ret = pthread_join(t[i], NULL)){
	    printf("Join Thread %d error: %s\n", i, strerror(ret));
	    exit(1);
	}
	printf("Thread %d: %u[%f]\n", i, ta[i].cand, ta[i].minvar);
	if(minvar == -1 || ta[i].minvar < minvar){
	    minvar = ta[i].minvar;
	    cand = ta[i].cand;
	}
    }
    ed = clock();
    printf(" -Total time: %lu(%.2f s)\n", ed - st, (ed - st) * 1.0 / CLOCKS_PER_SEC);
    printf(" -Optimal flag: %u(%f)\n", cand, minvar);


    return cand;
}
/* static FP_T opt_fp(struct darray *fp, size_t sz, int m) */
static FP_T opt_fp(unsigned *fps, int fps_nr, unsigned *tps, /* size_t */long sz, int m)
{
    unsigned range = 1, cand, j, loop, step, s;
    long i;
    double minvar = -1, var;
    /* int minvar=-1, var; */
    /* unsigned *vs; */
    double *vs;
    unsigned *last, *vsnr;
    int r_st, r_ed;

    clock_t st, ed;
    unsigned long long t;

    int r;
    clock_t tst, ted;

    for(i = 1; i <= m; i++)
	range *= 2;

#define SHARE 131072 /* 262144 */ /* 256k */
    r = range / 1;
/*     r = (range > SHARE ? SHARE : range); */
    printf(" -Candidate Range (0 - %u)\n", range);
    
    vs = malloc(sizeof(double) * range);
    last = malloc(sizeof(unsigned) * range);
    vsnr = malloc(sizeof(unsigned) * range);


    assert(vs != NULL && last != NULL && vsnr != NULL);
    
    memset(vs, 0, sizeof(double) * range);
    memset(last, 0, sizeof(unsigned) * range);
    memset(vsnr, 0, sizeof(unsigned) * range);

    tst = clock();
    for(j = 0; j < ceil(range * 1.0 / r); j++){

/*        srand48(time(NULL)); */

    /* printf(" -Fingerprint size: %d(%d) %llu(%d)--%llu(%d)--%llu(%d)\n", fp->nr, INT_MAX, fp->e[0], 0, fp->e[fp->nr / 2], fp->nr / 2, fp->e[fp->nr - 1], fp->nr - 1); */    

    /* loop = (range * 4) > fps_nr ? fps_nr : (range * 4); */
    loop = fps_nr;
    step = fps_nr / (range * 9);
    if(step == 0) step = 1;
    step = 1;
    printf("  --sample fps in step %u\n", step);
    s = 0;
    st = clock();
    r_st = j * r;
    r_ed = (j + 1) * r;
    for(i = 0; i < loop; i+=step){
	/* t = fp->e[i]; */
	/* t = fps[i]; */
	s++;
	t = fps[i];
/* 	t = i % range; */
/* 	t = drand48() * range; */
	if(t < r_st || t >= r_ed)
	    continue;

	if(i - last[t] < _window_size)
	    continue;

	vsnr[t]++;
/* 	vsnr[fps[i+1]]++; */
/* 	vsnr[fps[i+2]]++; */
/* 	vsnr[fps[i+3]]++; */
	/* vs[fp->e[i]] += labs((long)(i - last[fp->e[i]]) - (long)sz); */
/* 	vs[t] += labs((i - last[t]) - sz); */
	/* vs[t] += ((long)((i - last[t]) -sz) * ((i - last[t]) - sz)); */
	vs[t] += pow((i - (long)last[t] - (long)sz), 2);
/* 	vs[fps[i+1]] += ((long)((i - last[fps[i+1]]) -sz) * ((i - last[fps[i+1]]) - sz)); */
/* 	vs[fps[i+2]] += ((long)((i - last[fps[i+2]]) -sz) * ((i - last[fps[i+2]]) - sz)); */
/* 	vs[fps[i+3]] += ((long)((i - last[fps[i+3]]) -sz) * ((i - last[fps[i+3]]) - sz)); */
	last[t] = i;
/* 	last[fps[i+1]] = i+1; */
/* 	last[fps[i+2]] = i+2; */
/* 	last[fps[i+3]] = i+3; */
    }
    ed = clock();
    printf("  --Sample times: %u(%u)\n", s, loop);
    printf("  --Calculate(%d) all variances %lu(%.2f s)\n", j, ed - st,(ed - st) * 1.0 / CLOCKS_PER_SEC);

    st = clock();
    /* for(i = 0; i < range; i++){ */
    for(i = j * r; i < ((j + 1) * r < range ? (j + 1) * r : range); i++){
	 vsnr[i]++;
	 /* vs[i] += labs((long)(fp->nr - last[i]) - (long)sz); */
	 vs[i] += pow((long)fps_nr - (long)last[i] - (long)sz, 2);

	if(vsnr[i] == 0)
	    continue;
	var = sqrt(vs[i] * 1.0 / vsnr[i]);

	if(minvar == -1 || var < minvar){
	      minvar = var;
	      cand = i;
	 }

    }

    ed = clock();
    printf("  --Retrieve optimal variance in %lu(%.2f s)\n", ed - st, (ed - st) * 1.0 / CLOCKS_PER_SEC);
    printf("  --Optimal flag until now: %u(%f)\n", cand, minvar);
    }
    ted = clock();
    printf(" -Total time: %lu(%.2f s)\n", ted - tst, (ted - tst) * 1.0 / CLOCKS_PER_SEC);
    printf(" -Optimal flag: %u(%f)\n", cand, minvar);
    printf(" -Random flag: 0(%f)\n", sqrt(vs[0]/vsnr[0]));

    /* printf("Flag position[%u]: ", vsnr[cand]); */
    /* for(i = 0; i < fps_nr; i++){ */
    /* 	if(fps[i] == cand){ */
    /* 	    printf("%ld, ", i); */
    /* 	} */
    /* } */
    /* printf("\n"); */

    free(last);
    free(vs);
    free(vsnr);
    printf(" -Free memory done\n");

    return cand;
}


static double get_var(struct darray *fp, FP_T target, size_t avg)
{
    double var = 0, av;
    int last = 0;
    int i, nr = 0;

    /* if(target == 0){ */
    /*  printf("-------DEBUG------\n"); */
    /*  printf("expected size is %lu\n", avg); */
    /* } */

    for(i = 0; i < fp->nr; i++){
	if(fp->e[i] != target)
	    continue;
	av = pow((i - last) - (long)avg, 2);
	/* if(target == 0) */
	/*     printf("var is %f\n", av); */
	var += av;
	nr++;
	last = i;
    }

    av = pow((fp->nr - last) - (long)avg, 2);
    /* if(target == 0) */
    /*  printf("var is %f\n", av); */
    var += av;
    nr++;

    /* printf("  { var of %llu in mid is %f }", target, var); */

    return sqrt(var) / nr;
}
