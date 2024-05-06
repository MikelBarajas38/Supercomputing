/*
• Materia: Supercómputo
• Semestre: 2023-2024/II
• Nombre del alumno: Mikel Ignacio Barajas Martínez
• Clave del alumno: 336483
• Carrera: Ingeniería en Sistemas Inteligentes
• Nombre de tarea o programa: MPI - MPI Types (Contiguous)
• Periodo de evaluación: Parcial 3
• Descripición: Implementar un programa en MPI que realice la suma de dos arreglos de coordenadas (x, y, z) de tamaño N, utilizando MPI_Type_contiguous para definir un tipo de dato MPI_COORD que contenga 3 floats.
• Avance logrado: 100%
• Comentarios adicionales: -
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

typedef struct
{

    float x;
    float y;
    float z;

} COORD;


COORD* createCoordArray(int N)
{
    return (COORD*)malloc(N * sizeof(COORD));
}

void fillCoordArray(COORD *A, int N)
{
    for(int i = 0; i < N; i++)
    {
        A[i].x = (float)rand() / (float)RAND_MAX * 10.0;
        A[i].y = (float)rand() / (float)RAND_MAX * 10.0;
        A[i].z = (float)rand() / (float)RAND_MAX * 10.0;
        //A[i].x = A[i].y = A[i].z = 1;
    }
}

void printVerboseCoordSum(COORD *A, COORD *B, COORD *C, int N)
{
    for(int i = 0; i < N; i++)
    {
        printf("[%d]: (%.2f, %.2f, %.2f) + (%.2f, %.2f, %.2f) = (%.2f, %.2f, %.2f)\n", i, A[i].x, A[i].y, A[i].z, B[i].x, B[i].y, B[i].z, C[i].x, C[i].y, C[i].z);
    }
}

void printCoordArray(COORD *A, int N)
{
    for(int i = 0; i < N; i++)
    {
        printf("(%.2f, %.2f, %.2f) ", A[i].x, A[i].y, A[i].z);
    }
    printf("\n");
}

COORD addCoords(COORD A, COORD B)
{
    COORD C;
    C.x = A.x + B.x;
    C.y = A.y + B.y;
    C.z = A.z + B.z;
    return C;
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

    MPI_Datatype MPI_COORD;
    MPI_Type_contiguous(3, MPI_FLOAT, &MPI_COORD);
    MPI_Type_commit(&MPI_COORD);
 
    int N = argv[1] ? atoi(argv[1]) : 1000;
    int nData = N / (numproc - 1);
    int nDataLast = nData + N % (numproc - 1);

    int *displs = malloc(numproc * sizeof(int));
    int *counts = malloc(numproc * sizeof(int));

    displs[0] = 0;
    counts[0] = 0;
    for(int i = 1; i < numproc - 1; i++)
    {
        displs[i] = (i - 1) * nData;
        counts[i] = nData;
    }
    displs[numproc - 1] = (numproc - 2) * nData;
    counts[numproc - 1] = nDataLast;

    COORD *A, *B, *C;

    if(idproc == 0)
    {
        srand(time(NULL));

        A = createCoordArray(N);
        fillCoordArray(A, N);

        B = createCoordArray(N);
        fillCoordArray(B, N);

        C = createCoordArray(N);

        printf("A: ");
        printCoordArray(A, N);

        printf("B: ");
        printCoordArray(B, N);
    } 
    else 
    {
        A = createCoordArray(counts[idproc]);
        B = createCoordArray(counts[idproc]);
        C = createCoordArray(counts[idproc]);
    }

    MPI_Scatterv(A, counts, displs, MPI_COORD, A, counts[idproc], MPI_COORD, 0, MPI_COMM_WORLD);
    MPI_Scatterv(B, counts, displs, MPI_COORD, B, counts[idproc], MPI_COORD, 0, MPI_COMM_WORLD);

    for(int i = 0; i < counts[idproc]; i++)
    {
        C[i] = addCoords(A[i], B[i]);
    }

    MPI_Gatherv(C, counts[idproc], MPI_COORD, C, counts, displs, MPI_COORD, 0, MPI_COMM_WORLD);

    if(idproc == 0)
    {
        printf("C: ");
        printCoordArray(C, N);

        printf("\nVerbose sum: \n");
        printVerboseCoordSum(A, B, C, N);
    } 
    
    free(A);
    free(B);
    free(C);
    free(displs);
    free(counts);
    
    MPI_Type_free(&MPI_COORD);
    MPI_Finalize();
}