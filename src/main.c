#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <time.h>
#include "mpi.h"

#define WIDTH 1280
#define HEIGHT 1024
#define DEPTH 32

#define FPS 5

#define DW 128
#define DH 102
#define CELL_D 10

int grid[DW][DH];
int grid_copy[DW][DH];

int rank;

Uint32 colors[16][3];

void draw(SDL_Surface *screen)
{
	for(int i = 0; i < DW; i++)
	{
		for(int j = 0; j < DH; j++)
		{
			// Draw in the cells
			if(grid[i][j] == 0)
			{
				boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 0, 0, 0, 255);
			}
			else
			{
				boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, colors[rank][0], colors[rank][1], colors[rank][2], 255);
			}

			// Draw the gridlines
			rectangleRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 100, 100, 100, 255);
		}
	}

	SDL_Flip(screen);
}

int count_live_neighbors(int i, int j)
{
	int sum = 0;

	if(i != 0)
		sum += grid[i - 1][j];

	if(j != 0)
		sum += grid[i][j - 1];

	if(j != 0 && i != 0)
		sum += grid[i - 1][j - 1];

	if(i != DW - 1)
		sum += grid[i + 1][j];

	if(j != DH - 1)
		sum += grid[i][j + 1];

	if(i != DW - 1 && j != DH - 1)
		sum += grid[i + 1][j + 1];

	if(i != 0 && j != DH - 1)
		sum += grid[i - 1][j + 1];

	if(i != DW - 1 && j != 0)
		sum += grid[i + 1][j - 1];

	return sum;
}

void advance() {
	int live_neighbors;

	for(int i = 0; i < DW; i++)
	{
		for(int j = 0; j < DH; j++)
		{
			live_neighbors = count_live_neighbors(i, j);	

			if(grid[i][j] == 1)
			{
				if(live_neighbors < 2)
				{
					grid_copy[i][j] = 0;
				}
				else if(live_neighbors > 3)
				{
					grid_copy[i][j] = 0;
				}
			}
			else
			{
				if(live_neighbors == 3)
				{
					grid_copy[i][j] = 1;
				}
			}
		}
	}

	for(int i = 0; i < DW; i++)
	{
		for(int j = 0; j < DH; j++)
		{
			grid[i][j] = grid_copy[i][j];
		}
	}
}

int main(int argc, char *argv[])
{
	srand((unsigned int)time(NULL));

	// ///////////////
	// Initialize MPI
	// ///////////////
	//
	
	int nodes, rc;

	rc = MPI_Init(&argc, &argv);

	if(rc != MPI_SUCCESS)
	{
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &nodes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Number of nodes: %d, my rank: %d\n", nodes, rank);

	// ///////////////
	// Initialize SDL
	// ///////////////
	SDL_Surface *screen;
	SDL_Event event;

	int quit = 0;
	int frame = 0;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		return 0;

	if(!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN | SDL_HWSURFACE)))
	{
		SDL_Quit();
		return 1;
	}


	colors[0][0]  = 255; colors[0][1]  = 255; colors[0][2]   = 255;
	colors[1][0]  = 0; 	 colors[1][1]  = 0;   colors[1][2]   = 255;
	colors[2][0]  = 0;   colors[2][1]  = 255; colors[2][2]   = 0;
	colors[3][0]  = 255; colors[3][1]  = 255; colors[3][2]   = 255;
	colors[4][0]  = 255; colors[4][1]  = 255; colors[4][2]   = 255;
	colors[5][0]  = 255; colors[5][1]  = 255; colors[5][2]   = 255;
	colors[6][0]  = 255; colors[6][1]  = 255; colors[6][2]   = 255;
	colors[7][0]  = 255; colors[7][1]  = 255; colors[7][2]   = 255;
	colors[8][0]  = 255; colors[8][1]  = 255; colors[8][2]   = 255;
	colors[9][0]  = 255; colors[9][1]  = 255; colors[9][2]   = 255;
	colors[10][0] = 255; colors[10][1] = 255; colors[10][2] = 255;
	colors[11][0] = 255; colors[11][1] = 255; colors[11][2] = 255;
	colors[12][0] = 255; colors[12][1] = 255; colors[12][2] = 255;
	colors[13][0] = 255; colors[13][1] = 255; colors[13][2] = 255;
	colors[14][0] = 255; colors[14][1] = 255; colors[14][2] = 255;
	colors[15][0] = 255; colors[15][1] = 255; colors[15][2] = 255;


	// Initialize the grid
	for(int i = 0; i < DW; i++)
	{
		for(int j = 0; j < DH; j++)
		{
			grid[i][j] = rand() % 2;
		}
	}

	int t_prev = SDL_GetTicks();

	while(!quit)
	{
		int t = SDL_GetTicks();

		if((t - t_prev) > 1000/FPS)
		{
			//MPI_Barrier(MPI_COMM_WORLD);
			draw(screen);
			//MPI_Barrier(MPI_COMM_WORLD);
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

	SDL_Quit();

	MPI_Finalize();

	return 0;
}
