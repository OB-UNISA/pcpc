#include <stdio.h>
#include <mpi.h>

void broadcast(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

void gather(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

void scatter(int P, int rank, void *buf, int N, MPI_Datatype data_type, MPI_Comm comm);
