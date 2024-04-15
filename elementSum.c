#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int *createArray(int N)
{
    int *array = malloc(N*sizeof(int));
    return array;
}

void fillArray(int *array, int N)
{
    for(int i = 0; i < N; i++)
    {
        array[i] = rand() % 10;
        //array[i] = 1;
    }
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

int main(int argc, char** argv) 
{   
    int idproc, numproc;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &idproc);
    MPI_Comm_size(MPI_COMM_WORLD, &numproc);

    if(argc > 2 && idproc == 0)
    {
        printf("Error: Invalid Args (usage -> %s N)", argv[0]);
        return 1;
    }

    int N = argv[1] ? atoi(argv[1]) : 1000;
    int nData = N/(numproc-1);
    int nDataLast = nData + N%(numproc-1);

    if(idproc == 0)
    {
        srand(time(NULL));

        printf("\n> N: %d\n", N);
        printf("> numproc: %d\n", numproc);
        printf("> nData: %d\n", nData);
        printf("> nDataLast: %d\n\n", nDataLast);

        int *A = createArray(N);
        fillArray(A, N);

        int *B = createArray(N);
        fillArray(B, N);

        int *C = createArray(N);

        printf("Array A: ");
        printArray(A, N);

        printf("Array B: ");
        printArray(B, N);

        for(int iProc = 1; iProc < numproc - 1; iProc++)
        {
            MPI_Send(A + ((iProc - 1) * nData), nData, MPI_INT, iProc, 0, MPI_COMM_WORLD);
            MPI_Send(B + ((iProc - 1) * nData), nData, MPI_INT, iProc, 0, MPI_COMM_WORLD);
        }
        MPI_Send(A + ((numproc - 2) * nData), nDataLast, MPI_INT, numproc - 1, 0, MPI_COMM_WORLD);
        MPI_Send(B + ((numproc - 2) * nData), nDataLast, MPI_INT, numproc - 1, 0, MPI_COMM_WORLD);

        for(int iProc = 1; iProc < numproc - 1; iProc++) 
        {
            MPI_Recv(C + ((iProc - 1) * nData), nData, MPI_INT, iProc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        MPI_Recv(C + ((numproc - 2) * nData), nDataLast, MPI_INT, numproc - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Result C: ");
        printArray(C, N);
        printf("\n");

        freeArray(A);
        freeArray(B);
        freeArray(C);

    }
    else
    {

        if(idproc == numproc - 1)
        {
            nData = nDataLast;
        }

        int *A = createArray(nData);
        int *B = createArray(nData);
        int *C = createArray(nData);

        MPI_Recv(A, nData, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B, nData, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for(int i = 0; i < nData; i++)
        {
            C[i] = A[i] + B[i];
        }

        MPI_Send(C, nData, MPI_INT, 0, 0, MPI_COMM_WORLD);

        freeArray(A);
        freeArray(B);
        freeArray(C);
    }

    MPI_Finalize();
}