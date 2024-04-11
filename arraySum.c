#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

int *createArray(int N)
{
    int *array = (int *)malloc(N*sizeof(int));
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
        int *A = createArray(N);
        fillArray(A, N);

        printArray(A, N);

        for(int iProc = 1; iProc < numproc - 1; iProc++)
        {
            MPI_Send(A + ((iProc - 1) * nData), nData, MPI_INT, iProc, 0, MPI_COMM_WORLD);
        }
        MPI_Send(A + ((numproc - 2) * nData), nDataLast, MPI_INT, numproc - 1, 0, MPI_COMM_WORLD);

        int sum = 0;
        for(int iProc = 1; iProc < numproc; iProc++)
        {
            int sumProc;
            MPI_Recv(&sumProc, 1, MPI_INT, iProc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += sumProc;
        }

        printf("Sum = %d\n", sum);

        freeArray(A);

    }
    else
    {

        if(idproc == numproc - 1)
        {
            nData = nDataLast;
        }

        int *A = createArray(nData);

        MPI_Recv(A, nData, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //printf("Processor %d ready to work with N = %d\n", idproc, nData);

        int sum = 0;
        for(int i = 0; i < nData; i++)
        {
            sum += A[i];
        }

        MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        freeArray(A);
    }

    MPI_Finalize();
}