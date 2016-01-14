#include <stdio.h>
int a[3][3] = {{1,2,3}, {4,5,6}, {7,8,9}};
int *pa[3] = {a[0], a[1], a[2]};
int *p = a[0];

int main()
{
  int i;
  for (i = 0; i< 3; i++)
    printf ("%d %d %d %d %d ", a[i][2-i], *a[i], *(*(a+i)+i), *pa[i], p[i]);
  return 0;
}
