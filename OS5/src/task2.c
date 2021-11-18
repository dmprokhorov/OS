#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdbool.h>

const char* path1 = "libfunctions1.so";
const char* path2 = "libfunctions2.so";
void (*swap)(int*, int*);
int (*PrimeCount)(int, int);
int (*GCF)(int, int);
void *handle = NULL;
bool first = true;

void Load()
{
        const char *path;
        if(first)
        {
                path = path1;
        }   
        else
        {
        	path = path2;
        }
        handle = dlopen(path, RTLD_LAZY);
        if(!handle)
        {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
        }
}

void Unload()
{
        dlclose(handle);
}

void loadContext()
{
        Load();
        swap = dlsym(handle, "swap");
        PrimeCount = dlsym(handle, "PrimeCount");
        GCF = dlsym(handle, "GCF");
        char* error;
        if((error = dlerror())) 
        {
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
        }
}

void Change()
{
        Unload();
        first ^= true;
        loadContext();
}

int main()
{
        loadContext();
        int ind = 0;
        while (scanf("%d", &ind) != EOF)
        {
		if(!ind)
		{
			Change();
			printf("Contract was changed. ");
			if(first)
			{
				printf("Now context is first\n");
			}    
			else
			{
				printf("Now context is second\n");
			}
			continue;
		}
		if(ind == 1) 
		{
			int A, B, result;
			scanf("%d %d", &A, &B);
			result = PrimeCount(A, B);
			if (result == -1)
			{
				printf("A and B must be non-negtive whole numbers\n");
			}
			else
			{
				printf("%d\n", result);
			}
		}
		else if (ind == 2)
		{
			int A, B;
			scanf("%d %d", &A, &B);
			printf("%d\n", GCF(A, B));
		}
        }
        Unload();
	return 0;
}
