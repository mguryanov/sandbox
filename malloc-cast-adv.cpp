#include <cstdlib>


/* malloc g++ require explicit cast */

int
main(int argc,char** argv)
{
    char* ptr=(char*)malloc(10*sizeof(char));
    free(ptr);
    ptr=0;
    return 0;
}
