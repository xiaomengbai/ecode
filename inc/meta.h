#ifndef _M_META_H
#define _M_META_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct chunk{
    size_t pos;
    size_t sz;
    unsigned long long fp;     
    int cnr;
    unsigned long long *cfp;
};
     
struct chunks {
    size_t nr;
    struct chunk **index;
};

struct chunk *new_chunk(size_t pos, size_t sz, unsigned long long fp);
void _destroy_chunk(struct chunk *ck);
void push_chunk(struct chunks *cks, struct chunk *ck);
void clear_chunks(struct chunks *cks);

void push_cfp(struct chunk *ck, unsigned long long fp);
void clear_cfp(struct chunk *ck);

struct rabin_args {
    int mask;
    size_t wsz;
    unsigned long long flag;
};

struct ecode_args {
    int n;
    int m;
};

struct meta{
    char name[100];
    struct ecode_args ea;
    struct rabin_args ra;
    struct chunks map;
};

void _p_meta(struct meta *mt);

struct meta *init_meta(char* name, int n, int m, size_t wz);
void write_meta(struct meta *mt, char *fname);
struct meta *read_meta(char *fname);

#endif
