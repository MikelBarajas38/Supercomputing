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

    int cve;
    int tipo;
    char nombre[200];
    float sueldo_base;
    float sueldo_horas_extra;
    char fecha_contrato[10];

} EMPLEADO;

typedef struct {

    char c1;
    char c2;
    char c3;
    char c4;

} CVE;

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

    MPI_Datatype MPI_EMPLEADO;

    int blocklens[6] = {1, 1, 200, 1, 1, 10};
    MPI_Aint displacements[6];

    displacements[0] = offsetof(EMPLEADO, cve);
    displacements[1] = offsetof(EMPLEADO, tipo);
    displacements[2] = offsetof(EMPLEADO, nombre);
    displacements[3] = offsetof(EMPLEADO, sueldo_base);
    displacements[4] = offsetof(EMPLEADO, sueldo_horas_extra);
    displacements[5] = offsetof(EMPLEADO, fecha_contrato);

    MPI_Datatype types[6] = {MPI_INT, MPI_INT, MPI_CHAR, MPI_FLOAT, MPI_FLOAT, MPI_CHAR};

    MPI_Type_create_struct(6, blocklens, displacements, types, &MPI_EMPLEADO);
    MPI_Type_commit(&MPI_EMPLEADO);

    MPI_Datatype MPI_CVE;
    MPI_Type_contiguous(4, MPI_CHAR, &MPI_CVE);
    MPI_Type_commit(&MPI_CVE);

    MPI_Type_free(&MPI_EMPLEADO);
    MPI_Type_free(&MPI_CVE);

    MPI_Finalize();
}