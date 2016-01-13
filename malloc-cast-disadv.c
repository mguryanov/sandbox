#include <stdlib.h>

/* 1- ANSI C standard, the cast is redundant
 * 2- some build-in function implementation may return int pointer, in case of
 *    asbsens "#include <stdlib.h>"
 */

int
main(int argc,char** argv)
{
    char* ptr=malloc(10*sizeof(char));
    free(ptr);
    ptr=0;
    return 0;
}
