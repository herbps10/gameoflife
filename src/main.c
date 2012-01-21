#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <time.h>

#define WIDTH 1024
#define HEIGHT 1024
#define DEPTH 32

#define FPS 5

#define D 160
#define CELL_D 5

int grid[D][D];
int grid_copy[D][D];

void draw(SDL_Surface *screen)
{
	for(int i = 0; i < D; i++)
	{
		for(int j = 0; j < D; j++)
		{
			if(grid[i][j] == 0)
			{
				boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 0, 0, 0, 255);
			}
			else
			{
				boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 255, 255, 255, 255);
			}

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

	if(i != D - 1)
		sum += grid[i + 1][j];

	if(j != D - 1)
		sum += grid[i][j + 1];

	if(i != D - 1 && j != D - 1)
		sum += grid[i + 1][j + 1];

	if(i != 0 && j != D - 1)
		sum += grid[i - 1][j + 1];

	if(i != D - 1 && j != 0)
		sum += grid[i + 1][j - 1];

	return sum;
}

void advance() {
	int live_neighbors;

	for(int i = 0; i < D; i++)
	{
		for(int j = 0; j < D; j++)
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

	for(int i = 0; i < D; i++)
	{
		for(int j = 0; j < D; j++)
		{
			grid[i][j] = grid_copy[i][j];
		}
	}
}

int main()
{
	srand((unsigned int)time(NULL));

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


	// Initialize the grid
	for(int i = 0; i < D; i++)
	{
		for(int j = 0; j < D; j++)
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

	SDL_Quit();

	return 0;

	return 0;
}
