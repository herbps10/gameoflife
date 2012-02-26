/**
 *	An array of the MPI indices of the sorrounding nodes
 *	Goes in clockwise order starting at NW corner
 *	First element of each index is the x coordinate, second is y coordinate
 */
char** wall_neighbors(int x, int y)
{
	char** neighbors = malloc(8 * sizeof(char *));

	for(int i = 0; i < 8; i++)
	{
		neighbors[i] = malloc(2 * sizeof(char));
	}

	// Calculate North West
	neighbors[NW][X] = x - 1;
	neighbors[NW][Y] = y - 1;
	
	if(neighbors[NW][X] < 0) neighbors[NW][X] = WALL_WIDTH - 1;
	if(neighbors[NW][Y] < 0) neighbors[NW][Y] = WALL_HEIGHT - 1;

	// Calculate North
	neighbors[N][X] = x;
	neighbors[N][Y] = y - 1;

	if(neighbors[N][Y] < 0) neighbors[N][Y] = WALL_HEIGHT - 1;

	// Calculate North East
	neighbors[NE][X] = x + 1;
	neighbors[NE][Y] = y - 1;

	if(neighbors[NE][X] == WALL_WIDTH) neighbors[NE][X] = 0;
	if(neighbors[NE][Y] < 0) neighbors[NE][Y] = WALL_HEIGHT - 1;

	// Calculate West
	neighbors[W][X] = x - 1;
	neighbors[W][Y] = y;
	
	if(neighbors[W][X] < 0) neighbors[W][X] = WALL_WIDTH - 1;

	// Calculate East
	neighbors[E][X] = x + 1;
	neighbors[E][Y] = y;

	if(neighbors[E][X] == WALL_WIDTH) neighbors[E][X] = 0;

	// Calculate South West
	neighbors[SW][X] = x - 1;
	neighbors[SW][Y] = y + 1;

	if(neighbors[SW][X] < 0) neighbors[SW][X] = WALL_WIDTH - 1;
	if(neighbors[SW][Y] == WALL_HEIGHT) neighbors[SW][Y] = 0;


	// Calculate South
	neighbors[S][X] = x;
	neighbors[S][Y] = y + 1;

	if(neighbors[S][Y] == WALL_HEIGHT) neighbors[S][Y] = 0;


	// Calculate South East
	neighbors[SE][X] = x + 1;
	neighbors[SE][Y] = y + 1;

	if(neighbors[SE][X] == WALL_WIDTH) neighbors[SE][X] = 0;
	if(neighbors[SE][Y] == WALL_HEIGHT) neighbors[SE][Y] = 0;

	return neighbors;
}



char mpi_get_cell(int wx, int wy, int x, int y)
{
	int packed_coordinates = x;
	packed_coordinates = packed_coordinates << 3;
	packed_coordinates += y;

	int destination = wy * WALL_HEIGHT + wx;

	char cell = -1;

	MPI_Status status;
	
	MPI_Request request1, request2;
	MPI_Isend(&packed_coordinates, sizeof(int), MPI_INT, destination, COORDINATES_TAG, MPI_COMM_WORLD, &request1);
	
	printf("Sent\n");

	MPI_Irecv(&cell, sizeof(char), MPI_CHAR, destination, CELL_TAG, MPI_COMM_WORLD, &request2);
	MPI_Wait(&request2, &status);

	printf("Recieved: %i\n", cell);

	return cell;
}

void *listener()
{
	int x, y, coordinates;
	char cell;
	MPI_Status status;

	printf("Started listener\n");

	MPI_Request request1, request2;
	while(1)
	{
		printf("Waiting...");
		MPI_Irecv(&coordinates, sizeof(int), MPI_INT, MPI_ANY_SOURCE, COORDINATES_TAG, MPI_COMM_WORLD, &request1);

		MPI_Wait(&request1, &status);

		x = coordinates >> 3;
		y = coordinates & 4;

		printf("Received %i, %i\n", x, y);

		cell = grid[x][y];

		printf("Cell to send: %i\n", cell);

		MPI_Isend(&cell, sizeof(char), MPI_CHAR, status.MPI_SOURCE, CELL_TAG, MPI_COMM_WORLD, &request2);
	}
}
