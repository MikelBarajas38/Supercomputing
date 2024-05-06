/*
• Materia: Supercómputo
• Semestre: 2023-2024/II
• Nombre del alumno: Mikel Ignacio Barajas Martínez
• Clave del alumno: 336483
• Carrera: Ingeniería en Sistemas Inteligentes
• Nombre de tarea o programa: MPI - MPIReduce
• Periodo de evaluación: Parcial 3
• Descripición: Implementar un programa en MPI que utilice MPIReduce para producir dos arreglos: uno con la suma por elemento de los arreglos de cada proceso y otro con el máximo por elemento de los arreglos de cada proceso.
• Avance logrado: 100%
• Comentarios adicionales: -
*/

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
    int idproc;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &idproc);

    if(argc > 2 && idproc == 0)
    {
        printf("Error: Invalid Args (usage -> %s N)", argv[0]);
        return 1;
    }

    int N = argv[1] ? atoi(argv[1]) : 1000;

    srand(time(NULL) * idproc);

    int *sumA, *maxA;
    if(idproc == 0)
    {
        sumA =  createArray(N);
        maxA =  createArray(N);
    }

    int *A =  createArray(N);
    fillArray(A, N);

    printf("Original de proceso %d: \n", idproc);
    printArray(A, N);

    MPI_Reduce(A, sumA, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(A, maxA, N, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if(idproc == 0)
    {
        printf("Suma: \n");
        printArray(sumA, N);
        printf("Máximo: \n");
        printArray(maxA, N);

        free(sumA);
        free(maxA);
    }

    free(A);

    MPI_Finalize();
}