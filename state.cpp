#include "state.h"


state::state() {
	CELL_PATH_N = false;
	CELL_PATH_E = false;
	CELL_PATH_S = false;
	CELL_PATH_W = false;
	CELL_VISITED = false;
	CELL_SOLVED = false;
	MAZE_SOL = false;

	COST = 0;
}
void state::reset()
{
	CELL_SOLVED = false;
	MAZE_SOL = false;
	COST = 0;
}

void state::visit()
{
	CELL_VISITED = true;
}


void state::set_Path_N()
{
	CELL_PATH_N = true;
}

void state::set_Path_E()
{
	CELL_PATH_E = true;
}

void state::set_Path_S()
{
	CELL_PATH_S = true;
}

void state::set_Path_W()
{
	CELL_PATH_W = true;
}

void state::clear()
{
	state();
}

bool state::get_Path_N()const
{
	return CELL_PATH_N;
}

bool state::get_Path_E()const
{
	return CELL_PATH_E;
}

bool state::get_Path_S()const
{
	return CELL_PATH_S;
}

bool state::get_Path_W()const
{
	return CELL_PATH_W;
}

bool state::get_visit()const
{
	return CELL_VISITED;
}





// solving
void state::set_Solved()
{
	CELL_SOLVED = true;
	set_Sol();
}

void state::set_SoftSolved()
{
	CELL_SOLVED = true;
}

bool state::get_Solved()const
{
	return CELL_SOLVED;
}

void state::set_Sol()
{
	MAZE_SOL = true;
}

void state::unset_Sol()
{
	MAZE_SOL = false;
}

bool state::get_Sol()const
{
	return MAZE_SOL;
}


void state::set_Cost(int x)
{
	COST = x;
}
void state::incrementCost()
{
	COST++;
}

int state::get_Cost()const
{
	return COST;
}