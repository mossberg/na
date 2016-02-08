#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "nooballoc.h"
#include "hexdump.h"



int main(int argc, char **argv)
{
    int ret;
    if ((ret = na_init())) {
        perror(NULL);
        return ret;
    }

    char *mem = na_alloc(8);
    strcpy(mem, "asdfasd");
    mem = na_alloc(8);
    strcpy(mem, "qwerqweoverflow");
    hexpoop(na_start, 20);
    mem = na_alloc(8);
    strcpy(mem, "hiimmakr");

    printf("%p\n", na_start);
    hexpoop(na_start, 20);

    na_close();
    return 0;
}
