#include <stdio.h>
#include <mpi.h>

void broadcast(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

void i_broadcast(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

void gather(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

void i_gather(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

int scatter(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

int i_scatter(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm);

int min(int P, int rank, int *array, int count, MPI_Comm comm);

int i_min(int P, int rank, int *array, int count, MPI_Comm comm);

int max(int P, int rank, int *array, int count, MPI_Comm comm);

int i_max(int P, int rank, int *array, int count, MPI_Comm comm);

void wait_all(MPI_Request requests[], int count);