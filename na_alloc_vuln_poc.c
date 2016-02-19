/* mark's dumb constrainted heap write, applies to at least git hash 2c80411
 * constraints:
 *  - write to higher heap memory than the chunk you overflow
 *  - payload you get to write is the argument to na_alloc and like 12 other
 *    bytes you can't control well
 *  - the argument to na_alloc to trigger the exploit must be < the offset
 *    to where you want to write
 */

#include <stdio.h>
#include "nooballoc2.h"

int main(int argc, const char *argv[])
{
    na_init();

    void *a = na_alloc(16);
    void *b = na_alloc(16);
    void *c = na_alloc(16);
    void *d = na_alloc(16);
    strcpy(d, "SENSITIVE");
    printf("My sensitive data at %p: %s\n", d, d);

    gets(a);

    void *x = na_alloc(68);
    printf("My sensitive data at %p: %s\n", d, d);

    na_close();
    return 0;
}
