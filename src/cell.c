int cell_set_bit(char g[CELLS_ACROSS][CELLS_DOWN], int x, int y, char bit, char val)
{
	char num = pow(2, bit - 1);
	
	if(val == 1)
	{
		if(g[x][y] < num) g[x][y] += num;
	}
	else
	{
		if(g[x][y] >= num) g[x][y] -= num;
	}
}

int cell_get_bit(char cell, int bit)
{
	char num = pow(2, bit - 1);
	return ((cell & num) == num);
}


int cell_state(char cell)
{
	return cell_get_bit(cell, 3);
}

int cell_allele1(char cell)
{
	return cell_get_bit(cell, 2);
}

int cell_allele2(char cell)
{
	return cell_get_bit(cell, 1);
}

void cell_set_state(char g[CELLS_ACROSS][CELLS_DOWN], int x, int y, char state)
{
	cell_set_bit(g, x, y, 3, state);
}

void cell_set_allele1(char g[CELLS_ACROSS][CELLS_DOWN], int x, int y, char allele1)
{
	cell_set_bit(g, x, y, 2, allele1);
}

void cell_set_allele2(char g[CELLS_ACROSS][CELLS_DOWN], int x, int y, char allele2)
{
	cell_set_bit(g, x, y, 1, allele2);
}


/**
 * Retrieve Moore Neighborhood Functions
 */

char *cell_get_neighbors(char g[CELLS_ACROSS][CELLS_DOWN], int x, int y)
{
	char* neighbors = malloc(8 * sizeof(char));
	char** nodes = wall_neighbors(wall_x, wall_y);

	// Southwest
	if(x == 0 || y == 0)
	{
		neighbors[SW] = mpi_get_cell(nodes[NW][X], nodes[NW][Y], CELLS_ACROSS - 1, CELLS_DOWN - 1);
	}
	else
	{
		neighbors[SW] = g[x - 1][y - 1];
	}

	// North
	if(y == 0)
	{
		neighbors[N] = mpi_get_cell(nodes[N][X], nodes[N][Y], x, CELLS_DOWN - 1);
	}
	else
	{
		neighbors[N] = g[x][y - 1];
	}

	// Northeast
	if(x == CELLS_ACROSS - 1 || y == 0)
	{
		neighbors[NE] = mpi_get_cell(nodes[NE][X], nodes[NE][Y], 0, CELLS_DOWN - 1);
	}
	else
	{
		neighbors[NE] = g[x + 1][y - 1];
	}

	// West
	if(x == 0)
	{
		neighbors[W] = mpi_get_cell(nodes[W][X], nodes[W][Y], CELLS_ACROSS - 1, y);
	}
	else
	{
		neighbors[W] = g[x - 1][y];
	}

	// East
	if(x == CELLS_ACROSS - 1)
	{
		neighbors[E] = mpi_get_cell(nodes[W][X], nodes[W][Y], 0, y);
	}
	else
	{
		neighbors[E] = g[x + 1][y];
	}

	// Southwest
	if(x == 0 || y == WALL_HEIGHT - 1)
	{
		neighbors[SW] = mpi_get_cell(nodes[SW][X], nodes[SW][Y], CELLS_ACROSS - 1, 0);
	}
	else
	{
		neighbors[SW] = g[x - 1][y + 1];
	}

	// South
	if(y == CELLS_DOWN - 1)
	{
		neighbors[S] = mpi_get_cell(nodes[S][X], nodes[S][Y], x, 0);
	}
	else
	{
		neighbors[S] = g[x][y + 1];
	}

	// Southeast
	if(x == WALL_WIDTH - 1 || y == 0)
	{
		neighbors[SE] =  mpi_get_cell(nodes[SE][X], nodes[SE][Y], 0, 0); 
	}
	else
	{
		neighbors[SE] =  g[x + 1][y + 1];
	}
}

char cell_get_neighbor(char g[CELLS_ACROSS][CELLS_DOWN], int x, int y, char neighbor)
{
	char *neighbors = cell_get_neighbors(g, x, y);

	return neighbors[neighbor];
}

int cell_count_live_neighbors(char g[CELLS_ACROSS][CELLS_DOWN], int x, int y)
{
	int sum = 0;
	for(int neighbor = 0; neighbor < 8; neighbor++)
	{
		sum += cell_state(cell_get_neighbor(g, x, y, neighbor));
	}

	return sum;

}
