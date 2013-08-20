#ifndef _M_HASH_H
#define _M_HASH_H

#include "md5.h"
#include "sha.h"
#include <stdio.h>

static unsigned long long get_hash(char *b, size_t sz){
    unsigned long long fp;

/*     ShaBuffer sha; */
/*     char n[17]; */

/*     sha_reset(sha); */
/*     sha_buffer(b, sz, sha); */
/*     memcpy(n, sha, 16); */
/*     n[17] = '\0'; */
/*     fp = strtoull(n, NULL, 16); */

    MD5_CTX md5;
    char m[16], n[17];
    int i;
    
    MD5_Init(&md5);
    MD5_Update(&md5, b, sz);
    MD5_Final(m, &md5);

    for(i = 0; i < 8; i++)
    	sprintf(n+i*2, "%2.2x", (unsigned char)m[i]);
    n[16] = '\0';
    fp = strtoull(n, NULL, 16);

    return fp;
}

#endif
