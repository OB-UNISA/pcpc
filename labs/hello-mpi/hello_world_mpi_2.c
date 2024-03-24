#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int process_Rank, size_Of_Cluster;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    for (int i = 0; i < size_Of_Cluster; i++)
    {
        if (i == process_Rank)
        {
            printf("Hello World from process %s, rank %d of %d\n", processor_name, process_Rank, size_Of_Cluster);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
