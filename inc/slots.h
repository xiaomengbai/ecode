#ifndef _M_SLOTS_H
#define _M_SLOTS_H
#include <stdlib.h>

struct slots{
    int nr;
    int sz;
    char **slot;
};

static void init_slots(struct slots *sls, int sz)
{
    sls->nr = 0;
    sls->sz = sz;
    sls->slot = NULL;
}

static char *new_slot(struct slots *sls)
{
    char *s;
    s = calloc(1, sls->sz);
    return s;
}

static void _insert_slot(struct slots *sls, int idx, char *s)
{
    int i;

    if(idx > sls->nr)
	return;
    sls->nr++;
    sls->slot = realloc(sls->slot, sls->nr * sizeof(char *));
    for(i = sls->nr - 1; i > idx; i--)
	sls->slot[i] = sls->slot[i - 1];
    sls->slot[idx] = s;
}
static char *_delete_slot(struct slots *sls, int idx)
{
    int i;
    char *r;
    
    if(idx >= sls->nr)
	return NULL;
    r = sls->slot[idx];
    sls->nr--;
    for(i = idx; i < sls->nr; i++)
	sls->slot[i] = sls->slot[i+1];
    sls->slot = realloc(sls->slot, sls->nr * sizeof(char *));

    return r;
}

static void enqueue_slot(struct slots *sls, char *s)
{
    _insert_slot(sls, sls->nr, s);
}

static char *dequeue_slot(struct slots *sls)
{
    return _delete_slot(sls, 0);
}


#endif
