#include <stdio.h>

extern void swap(int*, int*);
extern int PrimeCount(int, int);
extern int GCF(int, int);

int main()
{
	int ind, A, B;
	while ((scanf("%d", &ind)) != EOF)
	{
		if (ind == 1)
		{
			scanf("%d %d", &A, &B);
			int result = PrimeCount(A, B);
			if (result == -1)
			{
				printf("A and B must be non-negative numbers\n");
			}
			else
			{
				printf("%d\n", result);
			}
		}
		else if (ind == 2)
		{
			scanf("%d %d", &A, &B);
                        printf("%d\n", GCF(A, B));
		}
	}
	return 0;
}
