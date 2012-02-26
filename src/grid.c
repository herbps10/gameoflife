void grid_init(char g[CELLS_ACROSS][CELLS_DOWN])
{
	for(int i = 0; i < CELLS_ACROSS; i++)
	{
		for(int j = 0; j < CELLS_DOWN; j++)
		{
			g[i][j] = rand() % 8;
		}
	}
}
