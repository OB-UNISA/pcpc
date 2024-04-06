#include "lab4_4_mycollective.h"
#include <stdio.h>
#include <mpi.h>

void broadcast(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm)
{
    if (rank == 0)
    {
        for (int i = 1; i < P; i++)
        {
            MPI_Send(buf, count, data_type, i, i, comm);
        }
    }
    else
    {
        MPI_Recv(buf, count, data_type, 0, rank, comm, MPI_STATUS_IGNORE);
    }
}

void gather(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm)
{
    if (rank == 0)
    {
        for (int i = 1; i < P; i++)
        {
            MPI_Recv(buf, count, data_type, i, 0, comm, MPI_STATUS_IGNORE);
        }
    }
    else
    {
        MPI_Send(buf, count, data_type, 0, 0, comm);
    }
}

void scatter(int P, int rank, void *buf, int N, MPI_Datatype data_type, MPI_Comm comm)
{
    int leftover = N % (P - 1);
    int num_items = N / (P - 1);

    if (rank == 0)
    {
        void *p;
        for (int i = 1; i < P; i++)
        {
            int count = i - 1 < leftover ? num_items + 1 : num_items;
            MPI_Send(buf, count, MPI_INT, i, i, MPI_COMM_WORLD);
            p += count;
        }
    }
    else
    {
        int count = rank - 1 < leftover ? num_items + 1 : num_items;
        MPI_Recv(buf, count, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

