#include <stdlib.h>
#include <stdbool.h>

extern void swap(int* a, int* b);
extern int PrimeCount(int A, int B);
extern int GCF (int A, int B);

void swap (int *a, int *b)
{
        *a += *b;
        *b = *a - *b;
        *a -= *b;
}

int PrimeCount (int A, int B)
{
	if ((A < 0) && (B < 0))
	{
		return -1;
	}
	if (A > B)
	{
		swap(&A, &B);
	}
	if (A < 0)
	{
		A = 0;
	}
	int amount = 0;
	bool prime = true;
	for (int i = A; i <= B; i++)
	{
		if (i > 1)
		{
			for (int j = 2; j * j <= i; j++)
			{
				if (!(i % j))
				{
					prime = false;
					break;
				}
			}
			if (prime)
			{
				amount++;
			}
			prime = true;
		}
	}
	return amount;
}

int GCF (int A, int B)
{
	if (!A && !B)
	{
		return -1;
	}
	A = abs(A);
	B = abs(B);
	if (A < B)
	{
		swap(&A, &B);
	}
	while (B)
	{
		A %= B;
		swap(&A, &B);
	}
	return A;
}
