/*
• Materia: Supercómputo
• Semestre: 2023-2024/II
• Nombre del alumno: Mikel Ignacio Barajas Martínez
• Clave del alumno: 336483
• Carrera: Ingeniería en Sistemas Inteligentes
• Nombre de tarea o programa: MPI - Suma de dos arreglos (con Scatterv y Gatherv)
• Periodo de evaluación: Parcial 2
• Descripición: Implementar un programa en MPI que realize la suma de los elementos de dos arreglos de tamaño N
• Avance logrado: 100%
• Comentarios adicionales: -
*/

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int *createArray(int N)
{
    int *array = malloc(N * sizeof(int));
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
    int nData = N / (numproc - 1);
    int nDataLast = nData + N % (numproc - 1);

    int *A, *B, *C;
    int *Aproc, *Bproc, *Cproc;

    int stride = nData;
    int *displs = createArray(numproc);
    int *counts = createArray(numproc);

    displs[0] = 0;
    counts[0] = 0;
    for(int i = 1; i < numproc - 1; i++)
    {
        displs[i] = (i - 1) * stride;
        counts[i] = nData;
    }
    displs[numproc - 1] = (numproc - 2) * stride;
    counts[numproc - 1] = nDataLast;

    if(idproc == 0)
    {
        srand(time(NULL));

        printf("\n> N: %d\n", N);
        printf("> numproc: %d\n", numproc);
        printf("> nData: %d\n", nData);
        printf("> nDataLast: %d\n\n", nDataLast);

        printf("counts: ");
        printArray(counts, numproc);
        printf("displs: ");
        printArray(displs, numproc);
        printf("\n");

        A = createArray(N);
        fillArray(A, N);

        B = createArray(N);
        fillArray(B, N);

        C = createArray(N);

        printf("A: ");
        printArray(A, N);

        printf("B: ");
        printArray(B, N);
    } 
    else 
    {
        Aproc = createArray(counts[idproc]);
        Bproc = createArray(counts[idproc]);
        Cproc = createArray(counts[idproc]);
    }

    MPI_Scatterv(A, counts, displs, MPI_INT, Aproc, counts[idproc], MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(B, counts, displs, MPI_INT, Bproc, counts[idproc], MPI_INT, 0, MPI_COMM_WORLD);

    for(int i = 0; i < counts[idproc]; i++)
    {
        Cproc[i] = Aproc[i] + Bproc[i];
    }

    MPI_Gatherv(Cproc, counts[idproc], MPI_INT, C, counts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if(idproc == 0)
    {
        printf("C: ");
        printArray(C, N);

        freeArray(A);
        freeArray(B);
        freeArray(C);
    } 
    else 
    {
        freeArray(Aproc);
        freeArray(Bproc);
        freeArray(Cproc);
    }

    freeArray(displs);
    freeArray(counts);
    
    MPI_Finalize();
}