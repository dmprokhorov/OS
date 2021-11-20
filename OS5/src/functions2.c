#include <stdlib.h>
#include <stdbool.h>

extern void swap(int* a, int* b);
extern int PrimeCount(int A, int B);
extern int GCF(int A, int B);

void swap (int *a, int *b)
{
        *a += *b;
        *b = *a - *b;
        *a -= *b;
}

int PrimeCount (int A, int B)
{
	if ((A < 0) || (B < 0))
	{
		return -1;
	}
	if (A > B)
	{
		swap(&A, &B);
	}
	if (A < 0)
        {
                A = 2;
        }
	bool* sieve = (bool*)calloc((B + 1), sizeof(bool));
        int amount = 0;
	sieve[0] = true;
	if (B > 0)
	{
		sieve[1] = true;
	}
	for (int i = 2; i <= B; i++)
	{
		if (sieve[i])
		{
			continue;
		}
		else
		{
			if (i >= A)
			{
			        amount++;
			}
			for (int j = i * 2; j <= B; j += i)
			{
				sieve[j] = true;
			}
		}
	}
	free(sieve);
	return amount;	
}

int GCF (int A, int B)
{
	A = abs(A);
	B = abs(B);
	if (!A && !B)
	{
		return -1;
	}
	if (A > B)
	{
		swap(&A, &B);
	}
	if (!A)
	{
		return B;
	}
	int g = 1;
	for (int i = 2; i <= A; i++)
	{
		if ((!(A % i)) && (!(B % i)))
		{
			g = i;
		}
	}
	return g;
}

