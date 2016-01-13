#include <stdlib.h>
#include <malloc.h>


static void
display_mallinfo(void)
{
   struct mallinfo mi=mallinfo();

   printf("\n===============START=====================\n");

   printf("Total non-mmapped bytes (arena):       %d\n", mi.arena);
   printf("# of free chunks (ordblks):            %d\n", mi.ordblks);
   printf("# of free fastbin blocks (smblks):     %d\n", mi.smblks);
   printf("# of mapped regions (hblks):           %d\n", mi.hblks);
   printf("Bytes in mapped regions (hblkhd):      %d\n", mi.hblkhd);
   printf("Max. total allocated space (usmblks):  %d\n", mi.usmblks);
   printf("Free bytes held in fastbins (fsmblks): %d\n", mi.fsmblks);
   printf("Total allocated space (uordblks):      %d\n", mi.uordblks);
   printf("Total free space (fordblks):           %d\n", mi.fordblks);
   printf("Topmost releasable block (keepcost):   %d\n", mi.keepcost);

   printf("=================END=====================\n");
}


int
main(void)
{
    const int deep=5000;

    for (int i=0;i<deep;++i) {

        /*
         * according malloc.c comment minumum-sized chunk
         * equals 32 bytes on 64bit systems.
         *
         * how mach total allocated in cycle of 5000?
         *  5000 * 32 bytes = 160 000 bytes
         *
         * check it on programm exit!!!
         *
         */

        int* ptr=malloc(0);
        ptr=NULL;
    }

    malloc_stats ();
    exit(0);
}

