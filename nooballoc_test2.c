#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "nooballoc2.h"
#include "hexdump.h"



int main(int argc, char **argv)
{
    int ret;
    if ((ret = na_init())) {
        perror(NULL);
        return ret;
    }

    char *a = na_alloc(8);
    memset(a, 'A', 8);

    char *b = na_alloc(8);
    memset(b, 'B', 8);

    char *c = na_alloc(16);
    memset(c, 'C', 16);

    na_dump();
    hexdump(na_start, 0x50);

    na_free(a);

    na_dump();
    hexdump(na_start, 0x50);

    char *d = na_alloc(5);
    memset(d, 'D', 5);


    na_dump();
    hexdump(na_start, 0x50);



    na_close();
    return 0;
}
