#include <stdio.h>
#include <mpi.h>

void broadcast(int P, int rank, void *buff, int count, MPI_Datatype data_type, MPI_Comm comm)
{
    if (rank == 0)
    {
        for (int i = 1; i < P; i++)
        {
            MPI_Send(&buff, count, data_type, i, i, comm);
        }
    }
    else
    {
        MPI_Recv(&buff, count, data_type, 0, rank, comm, MPI_STATUS_IGNORE);
    }
}

void gather(int P, int rank, void *buff, int count, MPI_Datatype data_type, MPI_Comm comm)
{
    if (rank == 0)
    {
        for (int i = 1; i < P; i++)
        {
            MPI_Recv(&buff, count, data_type, i, i, comm, MPI_STATUS_IGNORE);
        }
    }
    else
    {
        MPI_Send(&buff, count, data_type, 0, 0, comm);
    }
}
