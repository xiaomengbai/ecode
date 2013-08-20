
#include "meta.h"
#include <assert.h>
  
struct chunk *new_chunk(size_t pos, size_t sz, unsigned long long fp)
{
     struct chunk *ck;
     ck = malloc(sizeof(struct chunk));
     ck->pos = pos;
     ck->sz = sz;
     ck->fp = fp;
     ck->cnr = 0;
     ck->cfp = NULL;
     return ck;
}

void _destroy_chunk(struct chunk *ck)
{
     if(ck->cnr)
	  free(ck->cfp);
     free(ck);
}

void push_cfp(struct chunk *ck, unsigned long long fp)
{
     ck->cnr++;
     ck->cfp = realloc(ck->cfp, sizeof(unsigned long long) * ck->cnr);
     ck->cfp[ck->cnr - 1] = fp;
}

void clear_cfp(struct chunk *ck)
{
    if(ck->cnr){
	  free(ck->cfp);
	  ck->cfp = NULL;
	  ck->cnr = 0;
    }
}

void push_chunk(struct chunks *cks, struct chunk *ck)
{
     cks->nr++;
     cks->index = realloc(cks->index, sizeof(struct chunk *) * cks->nr);
     cks->index[cks->nr - 1] = ck;
}

void clear_chunks(struct chunks *cks)
{
     int i;
     if(cks->nr){
	  for(i = 0; i < cks->nr; i++)
	       _destroy_chunk(cks->index[i]);
	  free(cks->index);
	  cks->nr = 0;
     }
}


void _p_chunk(struct chunk *ck)
{
     int i;
     puts("-------------------------");
     printf("pos: %15lu\n", ck->pos);
     printf("sz: %15lu\n", ck->sz);
     printf("fingerprint: %15llu\n", ck->fp);
     printf("coded fingerprint: ");
     for(i = 0; i < ck->cnr; i++)
	  printf("%llu ", ck->cfp[i]);
     printf("\n");
}

void _p_meta(struct meta *mt)
{
     int i;

     puts("==========META==========");
     printf("erasure code args:\n");
     printf("\tn: %d\n", mt->ea.n);
     printf("\tm: %d\n", mt->ea.m);
     printf("rabin fingerprint args:\n");
     printf("\tmask bit: %d\n", mt->ra.mask);
     printf("\twindow size: %lu\n", mt->ra.wsz);
     printf("\tflag: %llu\n", mt->ra.flag);
     printf("file map:\n");
     printf("\tchunk number: %lu\n", mt->map.nr);
     for(i = 0; i < 2; i++)
	  _p_chunk(mt->map.index[i]);
     printf("...\n");
     for(i = mt->map.nr - 2; i < mt->map.nr; i++)
	  _p_chunk(mt->map.index[i]);
     puts("========================");
}

struct meta *init_meta(char *name, int n, int m, size_t wsz)
{
     struct meta *mt;

     mt = malloc(sizeof(struct meta));

     strcpy(mt->name, name);

     mt->ea.n = n;
     mt->ea.m = m;

     mt->ra.mask = 0;
     mt->ra.wsz = wsz;
     mt->ra.flag = -1;

     mt->map.nr = 0;
     mt->map.index = NULL;
     
     return mt;
}

void write_meta(struct meta *mt, char *fname)
{
     FILE *f;
     int i, j;

     f = fopen(fname, "w");
     if(!f){
	  printf("open %s failed\n", fname);
	  exit(1);
     }

     fwrite(mt->name, 1, 100, f);
     fwrite(&mt->ea, 1, sizeof(struct ecode_args), f);
     fwrite(&mt->ra, 1, sizeof(struct rabin_args), f);
     for(i = 0; i < mt->map.nr; i++){
	  fwrite(mt->map.index[i], 1, sizeof(size_t) + sizeof(size_t) + sizeof(unsigned long long) + sizeof(int), f);
	  fwrite(mt->map.index[i]->cfp, mt->map.index[i]->cnr, sizeof(unsigned long long), f);
     }
	
     fclose(f);
}

struct meta *read_meta(char *fname)
{
     struct meta *mt;
     FILE *f;
     int i, j;
     struct chunk tmp;
     struct chunk *ck;
     size_t s;

     f = fopen(fname, "r");
     if(!f){
	  printf("open %s failed\n", fname);
	  return NULL;
     }

     mt = malloc(sizeof(struct meta));
     
     s = fread(mt->name, 1, 100, f);
     assert(s == 100);
     s = fread(&mt->ea, 1, sizeof(struct ecode_args), f);
     assert(s == sizeof(struct ecode_args));
     s = fread(&mt->ra, 1, sizeof(struct rabin_args), f);
     assert(s == sizeof(struct rabin_args));
     while(fread(&tmp, 1, sizeof(size_t) + sizeof(size_t) + sizeof(unsigned long long) + sizeof(int), f)){
	  ck = new_chunk(tmp.pos, tmp.sz, tmp.fp);
	  ck->cnr = tmp.cnr;
	  ck->cfp = calloc(tmp.cnr, sizeof(unsigned long long));
	  s = fread(ck->cfp, sizeof(unsigned long long), tmp.cnr, f);
	  assert(s == tmp.cnr);
	  push_chunk(&mt->map, ck);
     }

     fclose(f);
     return mt;
}

