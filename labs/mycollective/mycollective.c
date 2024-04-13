#include "mycollective.h"
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

void i_broadcast(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm)
{
    MPI_Request request = MPI_REQUEST_NULL;
    if (rank == 0)
    {
        for (int i = 1; i < P; i++)
        {
            MPI_Isend(buf, count, data_type, i, i, comm, &request);
        }
    }
    else
    {
        MPI_Irecv(buf, count, data_type, 0, rank, comm, &request);
        MPI_Wait(&request, MPI_STATUS_IGNORE);
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

void i_gather(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm)
{
    if (rank == 0)
    {
        MPI_Request requests[P - 1];
        for (int i = 0; i < P - 1; i++)
        {
            requests[i] = MPI_REQUEST_NULL;
        }

        for (int i = 1; i < P; i++)
        {
            MPI_Irecv(buf, count, data_type, i, 0, comm, &(requests[i - 1]));
        }
        wait_all(requests, P - 1);
    }
    else
    {
        MPI_Request request = MPI_REQUEST_NULL;
        MPI_Isend(buf, count, data_type, 0, 0, comm, &request);
    }
}

int scatter(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm)
{
    int leftover = count % (P - 1);
    int num_items = count / (P - 1);

    if (rank == 0)
    {
        int *int_p;
        char *char_p;
        if (data_type == MPI_INT)
        {
            int_p = buf;
        }
        else
        {
            char_p = buf;
        }

        for (int i = 1; i < P; i++)
        {
            int count = i - 1 < leftover ? num_items + 1 : num_items;
            MPI_Send(data_type == MPI_INT ? int_p : char_p, count, data_type, i, i, MPI_COMM_WORLD);
            if (data_type == MPI_INT)
            {
                int_p += count;
            }
            else
            {
                char_p += count;
            }
        }
        return count;
    }
    else
    {
        int count = rank - 1 < leftover ? num_items + 1 : num_items;
        MPI_Recv(buf, count, data_type, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        return count;
    }
}

int i_scatter(int P, int rank, void *buf, int count, MPI_Datatype data_type, MPI_Comm comm)
{
    MPI_Request request = MPI_REQUEST_NULL;

    int leftover = count % (P - 1);
    int num_items = count / (P - 1);

    if (rank == 0)
    {
        int *int_p;
        char *char_p;
        if (data_type == MPI_INT)
        {
            int_p = buf;
        }
        else
        {
            char_p = buf;
        }

        for (int i = 1; i < P; i++)
        {
            int count = i - 1 < leftover ? num_items + 1 : num_items;
            MPI_Isend(data_type == MPI_INT ? int_p : char_p, count, data_type, i, i, MPI_COMM_WORLD, &request);
            if (data_type == MPI_INT)
            {
                int_p += count;
            }
            else
            {
                char_p += count;
            }
        }
        return count;
    }
    else
    {
        int count = rank - 1 < leftover ? num_items + 1 : num_items;
        MPI_Irecv(buf, count, data_type, 0, rank, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, MPI_STATUS_IGNORE);
        return count;
    }
}

int min(int P, int rank, int *array, int count, MPI_Comm comm)
{
    count = scatter(P, rank, array, count, MPI_INT, MPI_COMM_WORLD);
    int min = array[0];

    if (rank == 0)
    {
        int l_min;
        for (int i = 1; i < P; i++)
        {
            MPI_Recv(&l_min, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (l_min < min)
            {
                min = l_min;
            }
        }
    }
    else
    {
        for (int i = 1; i < count; i++)
        {
            if (array[i] < min)
            {
                min = array[i];
            }
        }

        MPI_Send(&min, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    return min;
}

int i_min(int P, int rank, int *array, int count, MPI_Comm comm)
{
    count = i_scatter(P, rank, array, count, MPI_INT, MPI_COMM_WORLD);
    int min = array[0];

    if (rank == 0)
    {
        MPI_Request requests[P - 1];
        for (int i = 0; i < P - 1; i++)
        {
            requests[i] = MPI_REQUEST_NULL;
        }

        int l_min[P - 1];
        for (int i = 1; i < P; i++)
        {
            MPI_Irecv(&(l_min[i - 1]), 1, MPI_INT, i, 0, MPI_COMM_WORLD, &(requests[i - 1]));
        }

        for (int p = 0; p < P - 1; p++)
        {
            MPI_Wait(&(requests[p]), MPI_STATUS_IGNORE);
            if (l_min[p] < min)
            {
                min = l_min[p];
            }
        }
    }
    else
    {
        for (int i = 1; i < count; i++)
        {
            if (array[i] < min)
            {
                min = array[i];
            }
        }

        MPI_Request request = MPI_REQUEST_NULL;
        MPI_Isend(&min, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
    }

    return min;
}

int max(int P, int rank, int *array, int count, MPI_Comm comm)
{
    count = scatter(P, rank, array, count, MPI_INT, MPI_COMM_WORLD);
    int max = array[0];

    if (rank == 0)
    {
        int l_max;
        for (int i = 1; i < P; i++)
        {
            MPI_Recv(&l_max, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (l_max > max)
            {
                max = l_max;
            }
        }
    }
    else
    {
        for (int i = 1; i < count; i++)
        {
            if (array[i] > max)
            {
                max = array[i];
            }
        }

        MPI_Send(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    return max;
}

int i_max(int P, int rank, int *array, int count, MPI_Comm comm)
{
    count = i_scatter(P, rank, array, count, MPI_INT, MPI_COMM_WORLD);
    int max = array[0];

    if (rank == 0)
    {
        MPI_Request requests[P - 1];
        for (int i = 0; i < P - 1; i++)
        {
            requests[i] = MPI_REQUEST_NULL;
        }

        int l_max[P - 1];
        for (int i = 1; i < P; i++)
        {
            MPI_Irecv(&(l_max[i - 1]), 1, MPI_INT, i, 0, MPI_COMM_WORLD, &(requests[i - 1]));
        }

        for (int p = 0; p < P - 1; p++)
        {
            MPI_Wait(&(requests[p]), MPI_STATUS_IGNORE);
            if (l_max[p] > max)
            {
                max = l_max[p];
            }
        }
    }
    else
    {
        for (int i = 1; i < count; i++)
        {
            if (array[i] > max)
            {
                max = array[i];
            }
        }

        MPI_Request request = MPI_REQUEST_NULL;
        MPI_Isend(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
    }

    return max;
}

void wait_all(MPI_Request requests[], int count)
{
    for (int i = 0; i < count; i++)
    {
        MPI_Wait(&requests[i], MPI_STATUS_IGNORE);
    }
}