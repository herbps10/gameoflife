#include <stdio.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <time.h>
#include <pthread.h>
#include "mpi.h"
#include <unistd.h>

#define WIDTH 		1024
#define HEIGHT		1024
#define DEPTH		32

#define CELLS_ACROSS 100
#define CELLS_DOWN	 100

#define WALL_WIDTH	2
#define WALL_HEIGHT 1

#define CELL_D		WIDTH/CELLS_ACROSS

#define FPS			1

#define DOMINANT 	1
#define RECESSIVE 	1

#define ALIVE		1
#define DEAD		0

#define true		1
#define false		0

#define NW			0
#define N			1
#define NE			2
#define W			3
#define E			4
#define SW			5
#define S			6
#define SE			7

#define X 			0
#define Y 			1

#define CELL_TAG	1
#define COORDINATES_TAG	2

char** wall_neighbors(int x, int y);
char mpi_get_cell(int wx, int wy, int x, int y);


/**
 * Where we are in the overall wall
 */
int wall_x, wall_y;

/**
 * Cell Data Structure
 * 
 * Each cell is three bits.
 * 3-2-1
 * Bit 3 -- cell state
 * Bit 2 -- allele 1
 * Bit 1 -- allele 2
 */
char grid[CELLS_ACROSS][CELLS_DOWN];
char grid_copy[CELLS_ACROSS][CELLS_DOWN];

#include "cell.c"
#include "grid.c"
#include "wall.c"

void draw(SDL_Surface *screen)
{
	for(int i = 0; i < CELLS_ACROSS; i++)
	{
		for(int j = 0; j < CELLS_DOWN; j++)
		{
			// Draw in the cells
			if(cell_state(grid[i][j]) == 0)
			{
				boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 0, 0, 0, 255);
			}
			else
			{
				// Homozygous Dominant
				if(cell_allele1(grid[i][j]) == 1 && cell_allele2(grid[i][j]) == 1)
				{
					boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 255, 255, 255, 255);
				}
				// Heterozygous
				else if((cell_allele1(grid[i][j]) == 1 && cell_allele2(grid[i][j]) == 0) || (cell_allele1(grid[i][j]) == 0 && cell_allele2(grid[i][j]) == 1))
				{
					boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 255, 100, 100, 255);
				}
				// Homozygous Recessive
				else if(cell_allele1(grid[i][j]) == 0 && cell_allele2(grid[i][j]) == 0)
				{
					boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 100, 255, 100, 255);
				}
			}

			// Draw the gridlines
			rectangleRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 100, 100, 100, 255);
		}
	}

	SDL_Flip(screen);
}

void advance()
{
	int live_neighbors;

	for(int i = 0; i < CELLS_ACROSS; i++)
	{
		for(int j = 0; j < CELLS_DOWN; j++)
		{
			live_neighbors = cell_count_live_neighbors(grid, i, j);

			if(cell_state(grid[i][j]) == 1)
			{
				if(live_neighbors < 2)
				{
					cell_set_state(grid_copy, i, j, 0);
				}
				else if(live_neighbors > 3)
				{
					cell_set_state(grid_copy, i, j, 0);
				}
			}
			else
			{
				if(live_neighbors == 3)
				{
					cell_set_state(grid_copy, i, j, 1);

					char *neighbors = cell_get_neighbors(grid, i, j);

					int parent1 = rand() % 3;
					int parent2;
					do
					{
						parent2 = rand() % 3;
					} while(parent1 == parent2);

					if(rand() % 2 == 0)
					{
						cell_set_allele1(grid_copy, i, j, cell_allele1(neighbors[parent1]));
					}
					else
					{
						cell_set_allele1(grid_copy, i, j, cell_allele2(neighbors[parent1]));
					}

	
					if(rand() % 2 == 0)
					{
						cell_set_allele2(grid_copy, i, j, cell_allele1(neighbors[parent1]));
					}
					else
					{
						cell_set_allele2(grid_copy, i, j, cell_allele2(neighbors[parent1]));
					}

				}
			}
		}
	}

	for(int i = 0; i < CELLS_ACROSS; i++)
	{
		for(int j = 0; j < CELLS_DOWN; j++)
		{
			cell_set_state(grid, i, j, cell_state(grid_copy[i][j]));
			cell_set_allele1(grid, i, j, cell_allele1(grid_copy[i][j]));
			cell_set_allele2(grid, i, j, cell_allele2(grid_copy[i][j]));
		}
	}
}

int main(int argc, char *argv[])
{
	srand((unsigned int)time(NULL));

	// ///////////////
	// Initialize MPI
	// ///////////////
	int nodes, rank, rc;

	rc = MPI_Init(&argc, &argv);

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

	wall_x = rank % WALL_WIDTH;
	wall_y = rank / WALL_HEIGHT;

	printf("Node %i of %i. I am at location %i, %i on the wall.\n", rank, nodes, wall_x, wall_y);

	// 
	// Initialize the listener
	//
	pthread_t listener_thread;
	pthread_create(&listener_thread, NULL, listener, NULL);


	// Initialize the grid
	grid_init(grid);

	sleep(1);


	if(rank == 1)
	{
		printf("%i\n", mpi_get_cell(0, 0, 0, 0));
		printf("%i\n", mpi_get_cell(0, 0, 1, 2));
		printf("%i\n", mpi_get_cell(0, 0, 1, 3));
	}

	return 0;
	
	// ///////////////
	// Initialize SDL
	// ///////////////
	SDL_Surface *screen;
	SDL_Event event;

	int quit = 0;
	int frame = 0;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		return 0;

	if(!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
	{
		SDL_Quit();
		return 1;
	}

	int t_prev = SDL_GetTicks();

	while(quit == 0)
	{
		int t = SDL_GetTicks();

		if((t - t_prev) > 1000/FPS)
		{

			draw(screen);
			advance();

			t_prev = t;
		}

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = 1;
					break;
			}
		}
	}

	pthread_exit(NULL);

	MPI_Finalize();

	return 0;
}
