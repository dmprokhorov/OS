#include <stdio.h>

extern void swap(int*, int*);
extern int PrimeCount(int, int);
extern int GCF(int, int);

int main()
{
	int ind, A, B, result;
	while ((scanf("%d", &ind)) != EOF)
	{
		if ((scanf ("%d %d", &A, &B)) != EOF)
		{
			if (ind == 1)
			{
				result = PrimeCount(A, B);
				if (result == -1)
				{
					printf("At least A or B must be non-negative numbers\n");
				}
				else
				{
					printf("%d\n", result);
				}
			}
			else if (ind == 2)
			{
				result = GCF (A, B);
				if (result == -1)
				{
					printf("Infinity\n");
				}
				else
				{
				        printf("%d\n", result);
				}
			}
		}
	}
	return 0;
}
