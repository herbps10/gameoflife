#include <stdio.h>
#include <pthread.h>

#include "mpi.h"

#define CELL_TAG 1
#define COORDINATES_TAG 2

void *listener()
{
	printf("Listener spawned\n");

	int message;
	MPI_Status status;

	while(1)
	{
		MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, COORDINATES_TAG, MPI_COMM_WORLD, &status);
		printf("Recieved request for coordinates %i\n", message);
		
		int cell = 8;

		MPI_Send(&cell, 1, MPI_CHAR, status.MPI_SOURCE, CELL_TAG, MPI_COMM_WORLD);

		printf("Sent state of coordinates\n");
	}
}

int main(int argc, char *argv[])
{
	int nodes, rank, rc;

	// 
	// Initialize MPI
	// 
	MPI_Init_thread( 0, 0, MPI_THREAD_MULTIPLE, &rc );

	if(rc != MPI_SUCCESS)
	{
		printf("There was an error initializing the MPI program; aborting.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	//
	// Get number of nodes and our rank
	// 
	MPI_Comm_size(MPI_COMM_WORLD, &nodes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Node %i of %i\n", rank, nodes);

	// 
	// Initialize the listener
	//
	pthread_t listener_thread;
	pthread_create(&listener_thread, NULL, listener, NULL);


	if(rank == 0)
	{
		MPI_Status status;
		MPI_Request request1;
	
		int test = 1;
		MPI_Irecv(&test, 1, MPI_CHAR, MPI_ANY_SOURCE, CELL_TAG, MPI_COMM_WORLD, &request1);

		MPI_Send(&test, 1, MPI_INT, 1, COORDINATES_TAG, MPI_COMM_WORLD);
		printf("Requested state of coordinates\n");

		MPI_Wait(&request1, &status);

		printf("Received answer\n");
	}
	/*
	if(rank == 1)
	{
		int message;
		MPI_Status status;

		MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, COORDINATES_TAG, MPI_COMM_WORLD, &status);
		printf("Recieved request for coordinates %i\n", message);
		
		int cell = 8;

		MPI_Send(&cell, sizeof(char), MPI_CHAR, status.MPI_SOURCE, CELL_TAG, MPI_COMM_WORLD);

		printf("Sent state of coordinates\n");
	}
	*/

	printf("Here");
	pthread_exit(NULL);

	MPI_Finalize();

	return 0;
}
