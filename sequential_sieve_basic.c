#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void freeArray(int *array)
{
    free(array);
}

void printArray(int *array, int N)
{
    for(int i = 0; i < N; i++)
    {
        printf("%d\n", array[i]);
    }
    printf("\n");
}

int *secuential_sieve(int N) {

    int *primes = malloc(N * sizeof(int));

    for (long int i = 0; i <= N; i++)
    {
        primes[i] = 1;
    }
    primes[0] = primes[1] = 0;
    
    for(long int i = 2; i <= N; i++)
    {
        if(primes[i])
        {
            for(long int j = i * i; j > 0 && j <= N; j += i)
            {
                primes[j] = 0;
            }
        }
    }

    return primes;
}

int main(int argc, char** argv) 
{   

    if(argc > 2)
    {
        printf("Error: Invalid Args (usage -> %s N)", argv[0]);
        return 1;
    }
 
    int N = argv[1] ? atoi(argv[1]) : 1000;

    int *primes = secuential_sieve(N);
        
    //printf("\nSieve: \n");
    //printArray(primes, N + 1);

    printf("Primes: \n");
    for(int i = 0; i <= N; i++)
    {
        if(primes[i])
        {
            printf("%d\n", i);
        }
    }
    printf("\n\n");
}