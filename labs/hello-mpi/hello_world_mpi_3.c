#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int process_Rank, size_Of_Cluster, message_Item;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);

    if (process_Rank == 0)
    {
        message_Item = 42;
        MPI_Send(&message_Item, 1, MPI_INT, 2, 2, MPI_COMM_WORLD);
        printf("Message Sent: %d\n", message_Item);
    }

    else if (process_Rank == 2)
    {
        MPI_Recv(&message_Item, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Message Received: %d\n", message_Item);
    }

    else
    {
        // Get the name of the processor
        char processor_name[MPI_MAX_PROCESSOR_NAME];
        int name_len;
        MPI_Get_processor_name(processor_name, &name_len);
        printf("Message not received, processor: %s, rank: %d\n", processor_name, process_Rank);
    }

    MPI_Finalize();
    return 0;
}
