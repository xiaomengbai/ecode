#include "col.h"
#include <assert.h>

static void push_names(struct files *fs, char *name);
static void clear_names(struct files *fs);

void search_blk(unsigned long long fp, char *dname, char **b, size_t *sz)
{
    /* int i; */
    char name[256];
    /* struct files fs; */
    unsigned long long tfp;
    FILE *f;

    *b = NULL;
    *sz = 0;
    
    sprintf(name, "%s/%llu.blk", dname, fp);
    f = fopen(name, "r");
    if(!f){
/* 	 printf("%s :", name);fflush(stdout); */
/* 	 perror("open file error"); */
	return;
    }
    
    *sz = file_size(name) - sizeof(unsigned long long);
    *b = malloc(*sz);
    fclose(f);
    sprintf(name, "%llu.blk", fp);
    read_coded(&tfp, *b, *sz, dname, name);
    
    /* INIT_FILES(&fs); */
    /* load_names(dname, &fs); */

    /* for(i = 0; i < fs.nr; i++){ */
    /* 	tfp = read_fp(dname, fs.names[i]); */
    /* 	if(tfp == fp){ */
    /* 	    sprintf(name, "%s/%s", dname,fs.names[i]); */
    /* 	    *sz = file_size(name) - sizeof(unsigned long long); */
    /* 	    *b = malloc(*sz); */
    /* 	    read_coded(&tfp, *b, *sz, dname, fs.names[i]); */
    /* 	    clear_names(&fs); */
    /* 	    return; */
    /* 	} */
    /* } */

    /* clear_names(&fs); */
    /* *b = NULL; */
    /* *sz = 0; */
}

void load_names(char *dirname, struct files *fs)
{
    DIR *dp;
    struct dirent *ep;
    //    char name[512];

    dp = opendir(dirname);
    if(!dp){
	perror("can't open directory");
	exit(1);
    }

    while(ep = readdir(dp)){
	if(!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, ".."))
	    continue;
	/* sprintf(name, "%s/%s", dirname, ep->d_name); */
	/* push_names(fs, name); */
	push_names(fs, ep->d_name);
    }

    closedir(dp);
}


static void push_names(struct files *fs, char *name)
{
    fs->nr++;
    fs->names = realloc(fs->names, sizeof(char *) * fs->nr);
    fs->names[fs->nr - 1] = malloc(256);
    strcpy(fs->names[fs->nr - 1], name);
}

static void clear_names(struct files *fs)
{
    int i;
    if(fs->names){
	for(i = 0; i < fs->nr; i++)
	    free(fs->names[i]);
	free(fs->names);
	fs->names = NULL;
	fs->nr = 0;
    }
}

void write_coded(unsigned long long fp, char *b, size_t sz, char *dname, char *fname)
{
    char name[512];
    FILE *f;

    sprintf(name, "%s/%s", dname, fname);
    f = fopen(name, "w");
    if(!f){
	perror("write coded block fail!");
	return;
    }
    fwrite(&fp, 1, sizeof(unsigned long long), f);
    fwrite(b, 1, sz, f);
    fclose(f);
}

void read_coded(unsigned long long *fp, char *b, size_t sz, char *dname, char *fname)
{
    char name[512];
    FILE *f;
    size_t s;

    sprintf(name, "%s/%s", dname, fname);
    f = fopen(name, "r");
    if(!f){
	perror("read coded block fail!");
	return;
    }
    s = fread(fp, 1, sizeof(unsigned long long), f);
    assert(s == sizeof(unsigned long long));
    s = fread(b, 1, sz, f);
    assert(s == sz);
    fclose(f);
}

unsigned long long read_fp(char *dname, char *fname)
{
    char name[512];
    FILE *f;
    unsigned long long fp;
    size_t s;

    sprintf(name, "%s/%s", dname, fname);
    f = fopen(name, "r");
    if(!f){
	perror("read coded block fail!");
	return;
    }
    s = fread(&fp, 1, sizeof(unsigned long long), f);
    assert(s == sizeof(unsigned long long));
    fclose(f);
    return fp;
}
