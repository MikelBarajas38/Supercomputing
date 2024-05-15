/*
• Materia: Supercómputo
• Semestre: 2023-2024/II
• Nombre del alumno: Mikel Ignacio Barajas Martínez
• Clave del alumno: 336483
• Carrera: Ingeniería en Sistemas Inteligentes
• Nombre de tarea o programa: MPI - MPI_Type_Create_Struct
• Periodo de evaluación: Parcial 3

• Descripición: Impplementar un programa en MPI que defina un tipo de dato MPI_STUDENT con datos de un estudiante. 
                El programa deberá inicializar un arreglo de N estudiantes con datos aleatorios y calcular de forma paralela el promedio de las calificaciones de cada estudiante. 
                Posteriormente, deberá contar cuántos estudiantes tienen un promedio mayor o igual a 60.

• Avance logrado: 100%
• Comentarios adicionales: -
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "mpi.h"

#define NAME_LEN 200
#define LEN 4

typedef struct
{

    int id;
    int majorId;
    char name[NAME_LEN];
    float grades[LEN];
    float gpa;

} STUDENT;

char *firstNameList[28] = {"John", "Jane", "Alice", "Bob", "Charlie", "David", "Eve", "Frank", "Grace", "Hank", "Ivy", "Jack", "Kate", "Liam", "Mia", "Nate", "Olivia", "Paul", "Quinn", "Rose", "Sam", "Tina", "Ulysses", "Violet", "Will", "Xena", "Yuri", "Zack"};
char *lastNameList[23] = {"Smith", "Johnson", "Williams", "Jones", "Brown", "Davis", "Miller", "Wilson", "Moore", "Taylor", "Anderson", "Thomas", "Jackson", "White", "Harris", "Martin", "Thompson", "Garcia", "Martinez", "Robinson", "Clark", "Rodriguez", "Lewis"};

void initStudent(STUDENT *s, int id)
{
    s->id = id;
    s->majorId = rand() % 3;

    char *name = malloc(200 * sizeof(char));

    strcpy(name, firstNameList[rand() % 28]);
    strcat(name, " ");
    strcat(name, lastNameList[rand() % 23]);

    strcpy(s->name, name);

    free(name);

    for(int j = 0; j < LEN; j++)
    {
        s->grades[j] = (float)rand() / (float)RAND_MAX * 100.0;
    }

}

void fillStudentArray(STUDENT *S, int N)
{
    for(int i = 0; i < N; i++)
    {
        initStudent(&S[i], i);
    }
}

void printStudent(STUDENT *s)
{
    printf("ID: %d\n", s->id);
    printf("Major ID: %d\n", s->majorId);
    printf("Name: %s\n", s->name);

    printf("Grades: ");
    for(int j = 0; j < LEN; j++)
    {
        printf("%.2f ", s->grades[j]);
    }
    printf("\n");

    printf("GPA: %.2f\n", s->gpa);
    printf("\n");
}

void printStudentArray(STUDENT *S, int N)
{
    for(int i = 0; i < N; i++)
    {
        printStudent(&S[i]);
    }
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

    MPI_Datatype MPI_STUDENT;

    MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_CHAR, MPI_FLOAT, MPI_FLOAT};
    int blocklens[5] = {1, 1, NAME_LEN, LEN, 1};
    MPI_Aint blockdispls[5];

    blockdispls[0] = offsetof(STUDENT, id);
    blockdispls[1] = offsetof(STUDENT, majorId);
    blockdispls[2] = offsetof(STUDENT, name);
    blockdispls[3] = offsetof(STUDENT, grades);
    blockdispls[4] = offsetof(STUDENT, gpa);

    MPI_Type_create_struct(5, blocklens, blockdispls, types, &MPI_STUDENT);
    MPI_Type_commit(&MPI_STUDENT);

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

    STUDENT *S;

    if(idproc == 0)
    {
        srand(time(NULL));

        S = malloc(sizeof(STUDENT) * N);
        fillStudentArray(S, N);
        //printStudentArray(S, N);

    } 
    else 
    {
        S = malloc(sizeof(STUDENT) * counts[idproc]);
    }
    
    MPI_Scatterv(S, counts, displs, MPI_STUDENT, S, counts[idproc], MPI_STUDENT, 0, MPI_COMM_WORLD);

    int passed = 0, total = 0;
    for(int i = 0; i < counts[idproc]; i++)
    {
        float sum = 0.0;
        for(int j = 0; j < LEN; j++)
        {
            sum += S[i].grades[j];
        }
        S[i].gpa = sum / (float) LEN;

        if(S[i].gpa >= 60.0)
        {
            passed++;
        }
    }

    MPI_Gatherv(S, counts[idproc], MPI_STUDENT, S, counts, displs, MPI_STUDENT, 0, MPI_COMM_WORLD);
    MPI_Reduce(&passed, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(idproc == 0)
    {
        printf("Students: \n\n");
        printStudentArray(S, N);

        printf("Passed: %d\n", total);
    }

    free(S);
    MPI_Type_free(&MPI_STUDENT);
    MPI_Finalize();
}