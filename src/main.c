#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <time.h>

#define WIDTH 1024
#define HEIGHT 1024
#define DEPTH 32

#define FPS 1000

#define DW 1000
#define DH 1000
#define CELL_D 2

typedef struct {
	int status;

	int allele1;
	int allele2;
} cell;

cell *grid[DW][DH];
cell *grid_copy[DW][DH];

int rank;

void write_stats(FILE *file)
{
	int hdom = 0;
	int hrec = 0;
	int het = 0;

	for(int i = 0; i < DW; i++)
	{
		for(int j = 0; j < DH; j++)
		{
			if(grid[i][j]->status == 0) continue;

			if(grid[i][j]->allele1 == 1 && grid[i][j]->allele2 == 1)
				hdom++;

			if((grid[i][j]->allele1 == 1 && grid[i][j]->allele2 == 0) || (grid[i][j]->allele1 == 0 && grid[i][j]->allele2 == 1))
				het++;

			if(grid[i][j]->allele1 == 0 && grid[i][j]->allele2 == 0)
				hrec++;
		}
	}

	fprintf(file, "%i, %i, %i\n", hdom, het, hrec);
}

void draw(SDL_Surface *screen)
{
	for(int i = 0; i < DW; i++)
	{
		for(int j = 0; j < DH; j++)
		{
			// Draw in the cells
			if(grid[i][j]->status == 0)
			{
				boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 0, 0, 0, 255);
			}
			else
			{
				// Homozygous Dominant
				if(grid[i][j]->allele1 == 1 && grid[i][j]->allele2 == 1)
				{
					boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 255, 255, 255, 255);
				}
				// Heterozygous
				else if((grid[i][j]->allele1 == 1 && grid[i][j]->allele2 == 0) || (grid[i][j]->allele1 == 0 && grid[i][j]->allele2 == 1))
				{
					boxRGBA(screen, i * CELL_D, j * CELL_D, i * CELL_D + CELL_D, j * CELL_D + CELL_D, 255, 100, 100, 255);
				}
				// Homozygous Recessive
				else if(grid[i][j]->allele1 == 0 && grid[i][j]->allele2 == 0)
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

int count_live_neighbors(int i, int j)
{
	int sum = 0;

	if(i != 0)
		sum += grid[i - 1][j]->status;

	if(j != 0)
		sum += grid[i][j - 1]->status;

	if(j != 0 && i != 0)
		sum += grid[i - 1][j - 1]->status;

	if(i != DW - 1)
		sum += grid[i + 1][j]->status;

	if(j != DH - 1)
		sum += grid[i][j + 1]->status;

	if(i != DW - 1 && j != DH - 1)
		sum += grid[i + 1][j + 1]->status;

	if(i != 0 && j != DH - 1)
		sum += grid[i - 1][j + 1]->status;

	if(i != DW - 1 && j != 0)
		sum += grid[i + 1][j - 1]->status;

	return sum;
}

void get_neighbors(int i, int j, cell **neighbors)
{
	int index = 0;

	if(i != 0 && grid[i - 1][j]->status == 1)
	{
		neighbors[index] = grid[i - 1][j];
		index++;
	}

	if(j != 0 && grid[i][j - 1]->status == 1)
	{
		neighbors[index] = grid[i][j - 1];
		index++;
	}

	if(j != 0 && i != 0 && grid[i - 1][j - 1]->status == 1)
	{
		neighbors[index] = grid[i - 1][j - 1];
		index++;
	}

	if(i != DW - 1 && grid[i + 1][j]->status == 1)
	{
		neighbors[index] = grid[i + 1][j];
		index++;
	}

	if(j != DH - 1 && grid[i][j + 1]->status == 1)
	{
		neighbors[index] = grid[i][j + 1];
		index++;
	}

	if(i != DW - 1 && j != DH - 1 && grid[i + 1][j + 1]->status == 1)
	{
		neighbors[index] = grid[i + 1][j + 1];
		index++;
	}

	if(i != 0 && j != DH - 1 && grid[i - 1][j + 1]->status == 1)
	{
		neighbors[index] = grid[i - 1][j + 1];
		index++;
	}

	if(i != DW - 1 && j != 0 && grid[i + 1][j - 1]->status == 1)
	{
		neighbors[index] = grid[i + 1][j - 1];
		index++;
	}
}

void advance() {
	int live_neighbors;

	for(int i = 0; i < DW; i++)
	{
		for(int j = 0; j < DH; j++)
		{
			live_neighbors = count_live_neighbors(i, j);	

			if(grid[i][j]->status == 1)
			{
				if(live_neighbors < 2)
				{
					grid_copy[i][j]->status = 0;
				}
				else if(live_neighbors > 3)
				{
					grid_copy[i][j]->status = 0;
				}
			}
			else
			{
				if(live_neighbors == 3)
				{
					grid_copy[i][j]->status = 1;

					cell *neighbors[3];
					get_neighbors(i, j, neighbors);

					int parent1 = rand() % 3;
					int parent2;
					do
					{
						parent2 = rand() % 3;
					} while(parent1 == parent2);

					if(rand() % 2 == 0)
					{
						grid_copy[i][j]->allele1 = neighbors[parent1]->allele1;
					}
					else
					{
						grid_copy[i][j]->allele1 = neighbors[parent1]->allele2;
					}

	
					if(rand() % 2 == 0)
					{
						grid_copy[i][j]->allele2 = neighbors[parent2]->allele1;
					}
					else
					{
						grid_copy[i][j]->allele2 = neighbors[parent2]->allele2;
					}

				}
			}
		}
	}

	for(int i = 0; i < DW; i++)
	{
		for(int j = 0; j < DH; j++)
		{
			grid[i][j]->status = grid_copy[i][j]->status;
			grid[i][j]->allele1 = grid_copy[i][j]->allele1;
			grid[i][j]->allele2 = grid_copy[i][j]->allele2;
		}
	}
}

int main(int argc, char *argv[])
{
	srand((unsigned int)time(NULL));

	FILE *file = fopen("stats.csv", "w");
	fprintf(file, "hdom, het, hrec\n");

	// ///////////////
	// Initialize SDL
	// ///////////////
	/*
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
	*/

	// Initialize the grid
	for(int i = 0; i < DW; i++)
	{
		for(int j = 0; j < DH; j++)
		{
			grid[i][j] = malloc(sizeof(cell));

			grid[i][j]->status = 0;
			if(rand() % 5 == 1)
			{
				grid[i][j]->status = 1;
			}

			grid[i][j]->allele1 = 1; //rand() % 2;
			grid[i][j]->allele2 = 2; // rand() % 2;

			grid_copy[i][j] = malloc(sizeof(cell));
			grid_copy[i][j]->status = 0;
			grid_copy[i][j]->allele1 = 0;
			grid_copy[i][j]->allele2 = 0;
		}
	}

	/*
	int hrec = 0;
	int hdom = 0;
	int het = 0;
	for(int i = 0; i < 10000; i++) {
		grid[4][3]->status = 0;

		grid[3][3]->status = 1;
		grid[3][3]->allele1 = 1;
		grid[3][3]->allele2 = 1;

		grid[3][4]->status = 1;
		grid[3][4]->allele1 = 1;
		grid[3][4]->allele2 = 1;

		grid[4][4]->status = 1;
		grid[4][4]->allele1 = 0;
		grid[4][4]->allele2 = 0;

		advance();

		if(grid[4][3]->allele1 == 1 && grid[4][3]->allele2 == 1)
		{
			hdom++;
		}
		else if(grid[4][3]->allele1 == 0 && grid[4][3]->allele2 == 0)
		{
			hrec++;
		}
		else
		{
			het++;
		}
	}

	printf("Dominant: %i\n", hdom);
	printf("Recessive: %i\n", hrec);
	printf("Heterozygous: %i\n", het);


	return 0;
	*/

	//int t_prev = SDL_GetTicks();

	int generation = 0;

	while(generation < 1000)
	{
		write_stats(file);
		advance();

		generation += 1;

		if(generation % 10 == 0)
		{
			printf("%i\n", generation);
		}
	}

	/*
	while(!quit)
	{
		int t = SDL_GetTicks();

		if((t - t_prev) > 1000/FPS)
		{
			write_stats(file);

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
	*/

	fclose(file);

	SDL_Quit();

	return 0;
}
