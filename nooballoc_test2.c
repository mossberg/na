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
    hexdump(na_start, 50);

    printf("%lu\n", sizeof(struct na_chunk_hdr));
    char *mem = na_alloc(8);
    printf("%p\n", mem);
    /* strcpy(mem, "asdfasd"); */
    memset(mem, 'A', 8);

    /* mem = na_alloc(8); */
    /* strcpy(mem, "qwerqweoverflow"); */
    /* hexdump(na_start, 50); */
    mem = na_alloc(8);
    /* strcpy(mem, "hiimmak"); */
    memset(mem, 'B', 8);

    mem = na_alloc(16);
    /* strcpy(mem, "hiimmak"); */
    memset(mem, 'C', 16);

    /* mem = na_alloc(0x1000); */

    /* /1* printf("%p\n", na_start); *1/ */

    na_dump();

    hexdump(na_start, 0x50);

    na_close();
    return 0;
}
