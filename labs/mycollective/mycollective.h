#include <stdio.h>
#include <mpi.h>

void broadcast(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

void gather(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

int scatter(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

int min(int P, int rank, int *array, int count, MPI_Comm comm);

int max(int P, int rank, int *array, int count, MPI_Comm comm);