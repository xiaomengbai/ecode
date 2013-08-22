#ifndef _M_ARGS_H
#define _M_ARGS_H

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


struct MARGS{
    char proc[10]; /* encode/decode */
    int n;         /* data blocks */
    int m;         /* parity blocks */
    char output[100]; /* for encode, it is output
			 for decode, it is input */
    char input[100]; /* for encode, it is input 
			for decode, it is output */
    char meta[100]; /* meta file
		       for encode, it can be missing
		       for decode, it must exists */
    int ec_size; /* expected chunk size */
};

static struct MARGS _args;

#define OPTS "n:m:o:deM:i:c:"

static void store_args(int argc, char *argv[], struct MARGS *args)
{
    int c;
    
    while((c = getopt(argc, argv, OPTS)) != -1){
	switch(c){
	case 'n':
	    args->n = atoi(optarg);
	    break;
	case 'm':
	    args->m = atoi(optarg);
	    break;
	case 'd':
	    strcpy(args->proc, "decode");
	    break;
	case 'e':
	    strcpy(args->proc, "encode");
	    break;
	case 'o':
	    strcpy(args->output, optarg);
	    break;
	case 'i':
	    strcpy(args->input, optarg);
	    break;
	case 'M':
	    strcpy(args->meta, optarg);
	    break;
	case 'c':
	    args->ec_size = atoi(optarg);
	    break;
	case '?':
	    exit(1);
	}
    }
}

static void check_args(struct MARGS *args)
{
    int ret;

    if(!strcmp(args->proc, "encode")){
	if(!strlen(args->meta)){
	    if(args->n <= 0 || args->m <= 0){
		printf("invalid n & m: %d %d\n", args->n, args->m);
		exit(1);
	    }
	}
	if(args->ec_size < 1024 || args->ec_size > 1024 * 1024 * 10){
	    printf("invalid expected chunk size [%d-->%d]: %d\n", 1024, 1024 * 1024 * 10, args->ec_size);
	    exit(1);
	}
	if(!strlen(args->output)){
	    printf("missing output directory\n");
	    exit(1);
	}
	if(!strlen(args->input)){
	    printf("missing input file\n");
	    exit(1);
	}
	if(!strlen(args->meta)){
	    printf("missing meta file!\n");
	    exit(1);
	}

	{
	    struct stat st;
	    mode_t mask;
	    ret = stat(args->output, &st);
	    if(ret){
		/* mask = umask(0000); */
		/* printf("old mask %o\n", mask); */
		ret = mkdir(args->output, 0777);
		if(ret){
		    perror(args->output);
		    exit(1);
		}
	    }else{
		if(!S_ISDIR(st.st_mode)){
		    printf("%s is not a directory", args->output);
		    exit(1);
		}
	    }
	}

	{
	    struct stat st;
	    ret = stat(args->input, &st);
	    if(ret){
		perror(args->input);
		exit(1);
	    }
	}

    }else if(!strcmp(args->proc, "decode")){
	if(!strlen(args->meta)){
	    printf("missing meta file!\n");
	    exit(1);
	}
	if(!strlen(args->output)){
	    printf("missing decode directory\n");
	    exit(1);
	}
	if(!strlen(args->input)){
	    printf("specify recovery file\n");
	    exit(1);
	}
	{
	    struct stat st;
	    ret = stat(args->output, &st);
	    if(ret){
		perror(args->output);
		exit(1);
	    }
	    if(!S_ISDIR(st.st_mode)){
		printf("%s is not a directory", args->output);
		exit(1);
	    }
	}
	{
	    struct stat st;
	    ret = stat(args->meta, &st);
	    if(ret){
		perror(args->meta);
		exit(1);
	    }
	}

    }else{
	printf("Decide [e]ncode/[d]ecode in option\n");
	exit(1);
    }
}
#endif
