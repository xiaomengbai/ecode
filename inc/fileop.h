#ifndef _M_FILEOP_H
#define _M_FILEOP_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static size_t file_size(char *fname)
{
    FILE *f;
    size_t sz;

    f = fopen(fname, "r");
    if(!f){
	printf("file_size: open file %s error\n", fname);
	return 0;
    }

    fseek(f, 0, SEEK_END);
    sz = ftell(f);

    fclose(f);
    return sz;
}

static void load_file(char *fname, char *b)
{
    FILE *f;
    size_t sz, s;

    f = fopen(fname, "r");

    if(!f){
	printf("load_file: open %s failed\n", fname);
	exit(1);
    }

    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    rewind(f);

    s = fread(b, 1, sz, f);
    assert(s == sz);
    fclose(f);
}

#endif
