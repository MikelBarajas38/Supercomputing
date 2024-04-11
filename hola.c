#include <stdio.h>
#include <stdio.h>
#include "mpi.h"

int main(int argc, char** argv) 
{
    int idproc, numproc;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &idproc);
    MPI_Comm_size(MPI_COMM_WORLD, &numproc);

    printf("Hello world from processor %d out of %d processors\n", idproc, numproc);

    MPI_Finalize();
}