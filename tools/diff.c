#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *f1, *f2;
    char c1, c2;
    unsigned long long i = 0;

    f1 = fopen(argv[1], "r");
    f2 = fopen(argv[2], "r");

    while(fread(&c1, 1, 1, f1)){
	fread(&c2, 1, 1, f2);
	if(c1 != c2)
	    //	    printf("%c : %c (%llu)\n", c1, c2, i);
	    i++;
    }
    printf("%llu difference\n", i);
    fclose(f1);
    fclose(f2);

    return 0;
}
