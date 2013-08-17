#ifndef _M_GALOIS_H
#define _M_GALOIS_H

struct Gfield{
    int w;
    unsigned char mtable[256][256];
    unsigned char dtable[256][256];
    unsigned char (*add)(unsigned char a, unsigned char b);
    unsigned char (*sub)(unsigned char a, unsigned char b);
    unsigned char (*mul)(unsigned char a, unsigned char b);
    unsigned char (*div)(unsigned char a, unsigned char b);
    unsigned char (*pow)(unsigned char a, unsigned char b);
};


struct Gfield *init_Gfield(int w);
void destroy_Gfield(struct Gfield *gf);
#endif
