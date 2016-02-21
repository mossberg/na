#include <stdio.h>
#include "freelist.h"

void die(int x) {
    printf("got error %d\n", x);
    exit(1);
}

void test0(void)
{
    int rc;
    puts("hi");

    fl_init();

    rc = fl_append(1);
    if (rc)
        die(rc);

    rc = fl_append(2);
    if (rc)
        die(rc);

    rc = fl_append(3);
    if (rc)
        die(rc);

    /* struct flnode *x = fl_find(3); */
        
    fl_dump();


    fl_close();
}


int main(int argc, char **argv)
{
    test0();
    return 0;
}
