/*
• Materia: Supercómputo
• Semestre: 2023-2024/II
• Nombre del alumno: Mikel Ignacio Barajas Martínez
• Clave del alumno: 336483
• Carrera: Ingeniería en Sistemas Inteligentes
• Nombre de tarea o programa: MPI - MPIBcast
• Periodo de evaluación: Parcial 2
• Descripición: Implementar un programa en MPI que realice el broadcast de un arreglo de tamaño N desde el proceso 0
• Avance logrado: 100%
• Comentarios adicionales: -
*/


#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

void fillArray(double *array, int N)
{
    srand(time(NULL));
    for(int i = 0; i < N; i++)
    {
        array[i] = rand() % 100 / 10.0;
    }
}

void printArray(double *array, int N)
{
    for(int i = 0; i < N; i++)
    {
        printf("%.1lf ", array[i]);
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

    double *A =  malloc(N*sizeof(double));

    if(idproc == 0)
    {
        fillArray(A, N);
    }

    MPI_Bcast(A, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    printf("Proc %d: ", idproc);
    printArray(A, N);

    free(A);

    MPI_Finalize();
}