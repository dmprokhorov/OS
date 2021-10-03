#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <limits.h>
unsigned current_number, number;
pthread_t* threads;

typedef struct
{
    unsigned i, j;
    int *array;
} thread_data;

void* quicksort (void* arguments)
{
    thread_data* data = (thread_data*) arguments;
    unsigned i = data->i, j = data->j;
    int x = data->array[(i + j) / 2];
    while (i <= j)
    {
        while (data->array[i] < x)
        {
            i++;
        }
        while (data->array[j] > x)
        {
            j--;
        }
        if (i <= j)
        {
            if (data->array[i] > data->array[j])
            {
                data->array[i] += data->array[j];
                data->array[j] = data->array[i] - data->array[j];
                data->array[i] -= data->array[j];
            }
	    if (i == UINT_MAX)
	    {
                break;
	    }
	    i++;
            if (!j)
            {
                break;
            }
            j--;
        }
    }
    if ((i < data->j) && (data->i < j))
    {
        thread_data a = {i, data->j, data->array}, b = {data->i, j, data->array};
        if (current_number)
        {
            current_number--;
	    int index, local_number = current_number;
            if ((data->j - i) >= (j - data->i))
            {
                if (index = (pthread_create(&threads[number - current_number - 1], NULL, quicksort, &b)) != 0)
                {
                    printf("Can't create the thread\n");
		    printf("Code of error is %d\n", index);
                    exit(1);
                }
                quicksort(&a);
            }
            else
            {
                if (index = (pthread_create(&threads[number - current_number - 1], NULL, quicksort, &a)) != 0)
                {
                    printf("Can't create the thread\n");
		    printf("Code of error is %d\n", index);
		    exit(1);
		}
                quicksort(&b);
	    }
            if ((index = pthread_join(threads[number - local_number - 1], NULL)) != 0)
            {
                printf("Can't join the thread\n");
		printf("Code of error is %d\n", index);
		exit(1);
            } 
        }
        else
        {
            quicksort(&a);
            quicksort(&b);
        }
    }
    else
    {
        if (i < data->j)
        {
            thread_data a = {i, data->j, data->array};
	    quicksort(&a);
        }
        else if (data->i < j)
        {
            thread_data a = {data->i, j, data->array};
	    quicksort(&a);
        }
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if ((argc != 3) || (atoi(argv[1]) < 1) || (atoi(argv[2]) < 1))
    {
        printf("Syntax should be like this: ./[executable_file_name] [(positive) number_of_threads] [(positive) size_of_array]\n");
        exit(1);
    }
    number = strtol(argv[1], NULL, 10);
    unsigned size = strtol(argv[2], NULL, 10);
    if (number > size)
    {
        printf("The size of array is less than number of threads, but it can't be with parallel quick sort, so number of threads equals size of array now\n");
        number = size;
    }
    current_number = number;
    printf("Input elements of the array\n");
    int *array = (int*) malloc(size * sizeof(int));
    bool sorted = true;
    for (int i = 0; i < size; i++)
    {
        scanf("%i", &array[i]);
        if ((i) && (sorted) && (array[i] < array[i - 1]))
        {
            sorted = false;
        }
    }
    if (sorted)
    {
        printf("Array is sorted yet, this is the end of the program\n");
        for (int i = 0; i < size; i++)
        {
            printf("%i ", array[i]);
        }
        printf("\n");
        exit(1);
    }
    threads = (pthread_t*)malloc(number * sizeof(pthread_t));
    thread_data a = {0, size - 1, array};
    quicksort(&a);
    printf("Sorted array:\n");
    for (int i = 0; i < size; i++)
    {
        printf("%i ", array[i]);
    }
    printf("\n");
    free(array);
    free(threads);
    return 0;
}


