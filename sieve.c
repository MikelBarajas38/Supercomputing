/*
• Materia: Supercómputo
• Semestre: 2023-2024/II
• Nombres de los alumnos: Mikel Ignacio Barajas Martínez y Giacinto Carmelo Castagnetto Ibarra
• Clave de los alumnos: 336483, 282468
• Carrera: Ingeniería en Sistemas Inteligentes
• Nombre de tarea o programa: MPI - Sieve
• Periodo de evaluación: Parcial 3
• Descripición: Implementar la criba de Eratóstenes de forma paralela utilizando MPI.
• Avance logrado: 100%
• Comentarios adicionales: -
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi.h"

int idproc, numproc;

int *createArray(int N)
{
    int *array = malloc(N * sizeof(int));
    return array;
}

void freeArray(int *array)
{
    free(array);
}

void printArray(int *array, int N)
{
    for(int i = 0; i < N; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}

int *secuential_sieve(int N) {

    int *primes = malloc(N * sizeof(int));

    for (int i = 0; i <= N; i++)
    {
        primes[i] = 1;
    }
    primes[0] = primes[1] = 0;
    
    for(int i = 2; i <= N; i++)
    {
        if(primes[i])
        {
            for(int j = i * i; j > 0 && j <= N; j += i)
            {
                primes[j] = 0;
            }
        }
    }

    return primes;
}

int main(int argc, char** argv) 
{   

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &idproc);
    MPI_Comm_size(MPI_COMM_WORLD, &numproc);

    if(argc > 2 && idproc == 0)
    {
        printf("Error: Invalid Args (usage -> %s N)", argv[0]);
        return 1;
    }
 
    int N = argv[1] ? atoi(argv[1]) : 1000;
    int sqrtN = sqrt(N);

    int nData = (N - sqrtN) / (numproc - 1);
    int nDataLast = nData + (N - sqrtN) % (numproc - 1);

    int *displs = createArray(numproc);
    int *counts = createArray(numproc);

    displs[0] = 0;
    counts[0] = sqrtN + 1;
    for(int i = 1; i < numproc; i++)
    {
        displs[i] = counts[i - 1] + displs[i - 1];
        counts[i] = nData;
    }
    counts[numproc - 1] = nDataLast;

    int *primes, *primesProc;

    if(idproc == 0) 
    {

        //printf("\n> N: %d\n", N);
        //printf("> numproc: %d\n", numproc);
        //printf("> nData: %d\n", nData);
        //printf("> nDataLast: %d\n\n", nDataLast);

        //printf("counts: ");
        //printArray(counts, numproc);
        //printf("displs: ");
        //printArray(displs, numproc);

        primes = createArray(N + 1);

    } 

    primesProc = createArray(counts[idproc]);
    for(int i = 0; i < counts[idproc]; i++)
    {
        primesProc[i] = 1;
    }
    if(idproc == 0) primesProc[0] = primesProc[1] = 0;

    long int start = displs[idproc];
    long int end = counts[idproc] - 1;
    long int prime = 2;

    while(prime * prime <= N && prime * prime > 0)
    { 

        long int firstValue = prime * prime;
        long int closestValue;

        if(start > firstValue)
        {
            closestValue = start % prime && start > 0 ? prime - start % prime : 0;
        }
        else
        {
            closestValue = firstValue - start;
        }

        //printf("P(%d) -> prime - %d: start - %d, end - %d, closest value - %d, index of cv - %d\n", idproc, prime, start, end + start, closestValue + start, closestValue);
        
        for(int j = closestValue; j <= end; j += prime) 
        {
            primesProc[j] = 0;
        }

        if(idproc == 0)
        {
            while (!primesProc[++prime]);
            //printf("next prime: %d\n", prime);
            //printArray(primesProc, counts[idproc]);
        }

        MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Gatherv(primesProc, counts[idproc], MPI_INT, primes, counts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if(idproc == 0)
    {
        printf("\nSieve: \n");
        printArray(primes, N + 1);

        printf("Primes: \n");
        for(int i = 0; i <= N; i++)
        {
            if(primes[i])
            {
                printf("%d ", i);
            }
        }
        printf("\n\n");
        
        freeArray(primes);
    }

    freeArray(primesProc);
    freeArray(displs);
    freeArray(counts);

    MPI_Finalize();
}