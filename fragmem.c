#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include <time.h>


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

/*
 * malloc.c
 *
 * 1-
 * line:673 Overreliance on memalign is a sure way to fragment space.
 *
 * 2-
 * line:845 M_MXFAST is the maximum request size used for "fastbins",
 * special bins that hold returned chunks without consolidating their spaces.
 * This enables future requests for chunks of the same size to be handled
 * very quickly, but can increase fragmentation, and thus increase the overall
 * memory footprint of a program.
 * This malloc manages fastbins very conservatively yet still efficiently,
 * so fragmentation is rarely a problem for values less than or equal to
 * the default. The maximum supported value of MXFAST is 80. (default 64)
 * ...
 * Using fastbins for larger objects normally worsens fragmentation without
 * improving speed.
 *
 *
 * 3-
 * line:4485 Strategy: find a spot within that chunk that meets the alignment
 * request, and then possibly free the leading and trailing space.
 *
 */

int
main(int argc,char** argv)
{
    const int deep=50000;

    int* aalloc[deep];
    memset (aalloc,0,sizeof(int*)*deep);

    for (int i=0;i<deep*10;++i) {

        free(aalloc[i%deep]);
        aalloc[i%deep]=memalign(64,0x220);

        if (0==(i%1000)) {
            malloc_stats ();
        }
    }

    display_mallinfo ();
    malloc_stats ();

    return 0;
}

