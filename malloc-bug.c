#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define LSIZE (2*65536-4096)
#define SSIZE 257
#define NL 500
#define NS 70000

int
main(void)
{
        int i;
        void * bigalloc[NL],
             * smallalloc[NS];

        memset (smallalloc,0,NS*sizeof(void*));
        memset (bigalloc,0,NL*sizeof(void*));

        for (i = 0; i < (16*1024*1024*1024ULL)/65536; i++) {
                int bidx = i % NL, sidx = i % NS;
                int ssz = SSIZE + rand() % SSIZE;   /* obj size: SSIZE .. 2*SSIZE */
                int bsz = LSIZE + rand() % 8192;    /* obj size: LSIZE .. LSIZE + 8192 */
                /* total objs size < 2*SSIZE*NS + (LSIZE+8192)*NL = 103564000 = ~100MB */

                /* free current small and big object ... */
                free(bigalloc[bidx]);
                free(smallalloc[sidx]);

                /* ... and allocate a new one */
                smallalloc[sidx] = malloc(ssz);
                bigalloc[bidx] = malloc(bsz);
                memset(smallalloc[sidx], 0, ssz);
                memset(bigalloc[bidx], 0, bsz);


                if (i%5000==0) {
                    printf("index: %d\n",i);
                    malloc_stats();
                }
        }

        system("ps axv|fgrep stressalloc");
}
