
#include <iostream>
#include <fstream>;
#include <stack>
#include "maze.h"
#include "state.h"

// Define our static variables
std::atomic<bool> olcConsoleGameEngine::m_bAtomActive(false);
std::condition_variable olcConsoleGameEngine::m_cvGameFinished;
std::mutex olcConsoleGameEngine::m_muxGame;

Maze::Maze()
{
	m_sAppName = L"MAZE";
}

bool Maze::OnUserCreate()
{
	// Maze Parameters
	m_nMazeWidth = 40;
	m_nMazeHeight = 25;
	//m_nMazeWidth = 10;
	//m_nMazeHeight = 10;

	m_maze = new state[m_nMazeWidth * m_nMazeHeight];

	memset(m_maze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));

	m_stack.push(make_pair(0, 0));
	m_maze[0].visit();
	m_nVisitedCells = 1;
	m_nSolvedCells = 0;


	m_nPathWidth = 3;

	srand(time(0));
	mazeSolved = false;
	differenceTimer = -1; // intilize difference timer
	algCalls = 0;

	return true;
}

bool Maze::OnUserUpdate(float fElapsedTime)
{
	bool continueflag = true;
	if (m_nVisitedCells < m_nMazeHeight * m_nMazeWidth) {
		generate();
	}
	else { // maze has been successfully generated
		if (nCall == 0) {
			m_stack.pop();
			m_stack.push(make_pair(0, 0));
			m_maze[0].set_Solved();
			m_maze[0].set_Cost(0);
			m_nSolvedCells = 1; // for dijkstra's alg

			nCall++;

			// set timer1 
			start_timer1();
		}
		mazeSolved = solver(userChoice);
		if (mazeSolved) {
			// moved timer stuff to compare()
			// set timer2
			start_timer2();
			// take the difference to get the time
			set_timerDuration();
		}
	}


	// == Drawing Stuff == 
	// Clear Screen 
	Fill(0, 0, m_nScreenWidth, m_nScreenHeight, L' ');



	if (GetKey(' ').bHeld) {
		options();

		if (GetKey('R').bPressed) { // reset
			hardRestart();
		}
		else if (GetKey('D').bPressed) { // dijkstra
			hardRestart();
			set_userChoice('D');
			hardRestart();
		}
		else if (GetKey('S').bPressed) { // dfs
			hardRestart();
			set_userChoice('S');
			hardRestart();
		}
		else if (GetKey('U').bPressed) { // user Solve
			hardRestart();
			set_userChoice('U');
			hardRestart();
		}
		else if (GetKey('W').bPressed) { // wallfollower
			hardRestart();
			set_userChoice('W');
			hardRestart();
		}
		else if (GetKey('T').bPressed) { // rat in a maze
			hardRestart();
			set_userChoice('T');
			hardRestart();
		}
		else if (GetKey('C').bPressed) { // rat in a maze
			hardRestart();
			set_userChoice('C');
			hardRestart();
		}
		else if (GetKey('Q').bPressed) { // Quit || doesnt really work
			continueflag = false;;
		}




	}
	else {
		// Draw the maze
		draw();
	}

	return continueflag;
}

void Maze::generate()
{
	// Step 1: Create a set of unvisited neighbors
	auto offset = [&](int x, int y) {
		return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
	};


	vector<int> neighbors;

	// North Neightbor
	if (m_stack.top().second > 0 && (!m_maze[offset(0, -1)].get_visit()))
		neighbors.push_back(0);

	// East Neightbor
	if (m_stack.top().first < m_nMazeWidth - 1 && (!m_maze[offset(1, 0)].get_visit()))
		neighbors.push_back(1);

	// South Neighbor
	if (m_stack.top().second < m_nMazeHeight - 1 && (!m_maze[offset(0, 1)].get_visit()))
		neighbors.push_back(2);

	// West Neighbor
	if (m_stack.top().first > 0 && (!m_maze[offset(-1, 0)].get_visit()))
		neighbors.push_back(3);

	// Are there any neighbors avail?
	if (!neighbors.empty()) {
		//choose one available neighbor at random
		int next_cell_dir = neighbors[rand() % neighbors.size()];

		// Create a path between the neighbor and the current cell
		switch (next_cell_dir) {

		case 0: // North 
			m_maze[offset(0, -1)].visit();
			m_maze[offset(0, -1)].set_Path_S();
			m_maze[offset(0, 0)].set_Path_N();
			m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
			break;

		case 1: // East
			m_maze[offset(+1, 0)].visit();
			m_maze[offset(+1, 0)].set_Path_W();
			m_maze[offset(0, 0)].set_Path_E();
			m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
			break;

		case 2: // South
			m_maze[offset(0, +1)].visit();
			m_maze[offset(0, +1)].set_Path_N();
			m_maze[offset(0, 0)].set_Path_S();
			m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
			break;

		case 3: // West
			m_maze[offset(-1, 0)].visit();
			m_maze[offset(-1, 0)].set_Path_E();
			m_maze[offset(0, 0)].set_Path_W();
			m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
			break;

		}
		// new cell
		m_nVisitedCells++;
	}
	else {
		m_stack.pop(); // Backtrack
	}

}

void Maze::draw()
{

	if (userChoice == 'D' || algCalls == 1) {
		for (int x = 0; x < m_nMazeWidth; x++) {
			for (int y = 0; y < m_nMazeHeight; y++) {
				for (int py = 0; py < m_nPathWidth; py++) {
					for (int px = 0; px < m_nPathWidth; px++) {
						if (m_maze[y * m_nMazeWidth + x].get_visit()) {
							if (x == m_stack.top().first && y == m_stack.top().second) { // draw top of stack
								Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_GREEN);
							}
							else if ((x == 0 && y == 0) || (x == m_nMazeWidth - 1 && y == m_nMazeHeight - 1)) { // draw start/end point
								Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_RED);
							}
							else if (mazeSolved) {
								if (m_maze[y * m_nMazeWidth + x].get_Sol()) { // draw yellow and magenta path (solved and Solution paths)
									Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_MAGENTA);
								}
								else if (m_maze[y * m_nMazeWidth + x].get_Cost() > 0) {
									djikstraDraW(x, px, y, py);
								}
								else {
									if (m_maze[y * m_nMazeWidth + x].get_Sol()) { // draw yellow and magenta path (solved and Solution paths)
										Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_MAGENTA);
									}
									else {
										Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_WHITE);
									}
								}
							}
							else if (m_maze[y * m_nMazeWidth + x].get_Cost() > 0) {
								if (m_maze[y * m_nMazeWidth + x].get_Sol()) { // draw yellow and magenta path (solved and Solution paths)
									Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_MAGENTA);
								}
								else if (m_maze[y * m_nMazeWidth + x].get_Cost() > 0) {
									djikstraDraW(x, px, y, py);
								}
							}
							else
								Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_WHITE);
						}
						else {
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_BLUE);
						}
					}
				}
				for (int p = 0; p < m_nPathWidth; p++) {
					if (m_maze[y * m_nMazeWidth + x].get_Path_S()) {
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth);
					}
					if (m_maze[y * m_nMazeWidth + x].get_Path_E()) {
						Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p);
					}
				}
			}
		}




	}



	else {
		for (int x = 0; x < m_nMazeWidth; x++) {
			for (int y = 0; y < m_nMazeHeight; y++) {
				for (int py = 0; py < m_nPathWidth; py++) {
					for (int px = 0; px < m_nPathWidth; px++) {
						if (m_maze[y * m_nMazeWidth + x].get_visit()) {
							if (x == m_stack.top().first && y == m_stack.top().second) { // draw top of stack
								Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_GREEN);
							}
							else if ((x == 0 && y == 0) || (x == m_nMazeWidth - 1 && y == m_nMazeHeight - 1)) { // draw start/end point
								Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_RED);
							}
							else if (mazeSolved) {
								if (m_maze[y * m_nMazeWidth + x].get_Solved() && m_maze[y * m_nMazeWidth + x].get_Sol()) { // draw yellow and magenta path (solved and Solution paths)
									Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_MAGENTA);
								}
								else if (m_maze[y * m_nMazeWidth + x].get_Solved()) {
									Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_DARK_YELLOW);
								}
								else {
									Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_WHITE);
								}
							}
							else if (m_maze[y * m_nMazeWidth + x].get_Solved()) {
								Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_DARK_YELLOW);
							}
							else
								Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_WHITE);
						}
						else {
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_BLUE);
						}
					}
				}
				for (int p = 0; p < m_nPathWidth; p++) {
					if (m_maze[y * m_nMazeWidth + x].get_Path_S()) {
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth);
					}
					if (m_maze[y * m_nMazeWidth + x].get_Path_E()) {
						Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p);
					}
				}
			}
		}
	}



}


// user choice 
void Maze::set_userChoice(char c)
{
	userChoice = c;
}
char Maze::get_userChoice()const
{
	return userChoice;
}

// restart
void Maze::restart()
{	// reset the maze
	mazeSolved = false;
	nCall = 0;
	for (int i = 0; i < m_nMazeWidth * m_nMazeHeight; ++i) {
		m_maze[i].reset();
	}
}
void Maze::hardRestart()
{	// reset the maze
	mazeSolved = false;
	nCall = 0;
	algCalls = 0;
	for (int i = 0; i < m_nMazeWidth * m_nMazeHeight; ++i) {
		m_maze[i].reset();
	}
}

// maze solving algs

bool Maze::solver(char c) {

	switch (c) {
	case 'D':
		return djikstra();
		break;
	case 'S':
		return dfs();
		break;
	case 'U':
		return userSolver();
		break;
	case 'W':
		return wallFollower();
		break;
	case 'T':
		return ratMaze(); // need to implement rat in a maze 
		break;
	case 'C':
		return compare(); // need to implement time comparison
	default:
		return djikstra();
		break;
	}
	//if (c == 'R') {
	//	restart();
	//}
}

bool Maze::wallFollower()
{


	// step 1 is to get potential neighbors
	auto offset = [&](int x, int y) {
		return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
	};

	if (offset(0, 0) == m_nMazeHeight * m_nMazeWidth - 1) { // have we reached the end??
		return true;
	}



	vector<int> neighbors;
	bool go_west = false;
	bool go_south = false;


	// North Neightbor
	if (m_stack.top().second > 0 && (m_maze[offset(0, -1)].get_Path_S() && !m_maze[offset(0, -1)].get_Solved()))
		neighbors.push_back(0);

	// East Neightbor
	if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)].get_Path_W() && !m_maze[offset(1, 0)].get_Solved()))
		neighbors.push_back(1);

	// South Neighbor
	if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)].get_Path_N() && !m_maze[offset(0, 1)].get_Solved())) {
		neighbors.push_back(2);
		go_south = true;
	}
	// West Neighbor
	if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)].get_Path_E() && !m_maze[offset(-1, 0)].get_Solved())) {
		neighbors.push_back(3);
		go_west = true;
	}
	// Are there any neighbors avail?
	if (!neighbors.empty()) {
		//choose one available neighbor at random
		//int next_cell_dir = neighbors[rand() % neighbors.size()];
		int next_cell_dir;
		if (go_west)
			next_cell_dir = 3;
		else if (go_south)
			next_cell_dir = 2;
		else
			next_cell_dir = neighbors[rand() % neighbors.size()];


		// Create a path between the neighbor and the current cell
		switch (next_cell_dir) {

		case 0: // North 
			m_maze[offset(0, -1)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
			break;

		case 1: // East
			m_maze[offset(+1, 0)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
			break;

		case 2: // South
			m_maze[offset(0, +1)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
			break;

		case 3: // West
			m_maze[offset(-1, 0)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
			break;

		}
	}
	else {
		m_maze[offset(0, 0)].unset_Sol();
		m_stack.pop(); // Backtrack
	}
	return false;
}

bool Maze::userSolver()
{
	// step 1 is to get potential neighbors
	auto offset = [&](int x, int y) {
		return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
	};

	if (offset(0, 0) == m_nMazeHeight * m_nMazeWidth - 1) { // have we reached the end??
		return true;
	}

	int xpos, ypos = 0;

	// move North
	if (GetKey('W').bPressed) {
		if (m_stack.top().second > 0 && m_maze[offset(0, -1)].get_Path_S()) { // Check for legal move 
			xpos = m_stack.top().first;
			ypos = m_stack.top().second;
			m_stack.pop();
			m_stack.push(make_pair(xpos, ypos - 1));
		}
	}


	// move South
	else if (GetKey('S').bPressed) {
		if (m_stack.top().second < m_nMazeHeight - 1 && m_maze[offset(0, 1)].get_Path_N()) { // Check for legal move 
			xpos = m_stack.top().first;
			ypos = m_stack.top().second;
			m_stack.pop();
			m_stack.push(make_pair(xpos, ypos + 1));
		}
	}

	// move West
	else if (GetKey('A').bPressed) {
		if (m_stack.top().first > 0 && m_maze[offset(-1, 0)].get_Path_E()) { // Check for legal move 
			xpos = m_stack.top().first;
			ypos = m_stack.top().second;
			m_stack.pop();
			m_stack.push(make_pair(xpos - 1, ypos));
		}
	}

	// move East
	else if (GetKey('D').bPressed) {
		if (m_stack.top().first < m_nMazeWidth - 1 && m_maze[offset(1, 0)].get_Path_W()) { // Check for legal move 
			xpos = m_stack.top().first;
			ypos = m_stack.top().second;
			m_stack.pop();
			m_stack.push(make_pair(xpos + 1, ypos));
		}
	}

	m_maze[offset(0, 0)].set_Solved();


	return false;
}

bool Maze::dfs()
{
	auto offset = [&](int x, int y) {
		return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
	};

	if (offset(0, 0) == m_nMazeHeight * m_nMazeWidth - 1) { // have we reached the end??
		return true;
	}

	// setp 1: get neighbors
	vector<int> neighbors;

	// North Neightbor
	if (m_stack.top().second > 0 && (m_maze[offset(0, -1)].get_Path_S() && !m_maze[offset(0, -1)].get_Solved()))
		neighbors.push_back(0);

	// East Neightbor
	if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)].get_Path_W() && !m_maze[offset(1, 0)].get_Solved()))
		neighbors.push_back(1);

	// South Neighbor
	if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)].get_Path_N() && !m_maze[offset(0, 1)].get_Solved())) {
		neighbors.push_back(2);
	}
	// West Neighbor
	if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)].get_Path_E() && !m_maze[offset(-1, 0)].get_Solved())) {
		neighbors.push_back(3);
	}

	// Are there any neighbors avail?
	if (!neighbors.empty()) {
		//choose one available neighbor at random
		int next_cell_dir = neighbors[rand() % neighbors.size()];


		// Create a path between the neighbor and the current cell
		switch (next_cell_dir) {

		case 0: // North 
			m_maze[offset(0, -1)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
			break;

		case 1: // East
			m_maze[offset(+1, 0)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
			break;

		case 2: // South
			m_maze[offset(0, +1)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
			break;

		case 3: // West
			m_maze[offset(-1, 0)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
			break;

		}
	}
	else {
		m_maze[offset(0, 0)].unset_Sol();
		m_stack.pop(); // Backtrack
	}



	return false;
}

bool Maze::djikstra() {



	auto offset = [&](int x, int y) {
		return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
	};

	//if (offset(0, 0) == m_nMazeHeight * m_nMazeWidth - 1) { // have we reached the end?
	//	return true;
	//}


	int xpos, ypos = 0;
	vector<int> neighbors;
	vector<int> cost;
	if (m_nSolvedCells < m_nMazeHeight * m_nMazeWidth) {
		// step 3: record the cost of reaching current cell: Find neighbors

			// North Neightbor
		if (m_stack.top().second > 0 && (m_maze[offset(0, -1)].get_Path_S() && !m_maze[offset(0, -1)].get_Solved()))
			neighbors.push_back(0);

		// East Neightbor
		if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)].get_Path_W() && !m_maze[offset(1, 0)].get_Solved()))
			neighbors.push_back(1);

		// South Neighbor
		if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)].get_Path_N() && !m_maze[offset(0, 1)].get_Solved())) {
			neighbors.push_back(2);
		}
		// West Neighbor
		if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)].get_Path_E() && !m_maze[offset(-1, 0)].get_Solved())) {
			neighbors.push_back(3);
		}

		// record the cost of reaching neighbors
		//m_maze[offset(0, -1)].set_Cost(currentCost + 1);

		if (!neighbors.empty()) {
			int currentCost = m_maze[offset(0, 0)].get_Cost();
			int cell = neighbors[rand() % neighbors.size()];
			switch (cell) {

			case 0: // North 
				m_maze[offset(0, -1)].set_SoftSolved();
				m_maze[offset(0, -1)].set_Cost(currentCost + 1);
				m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
				//m_nSolvedCells++;
				break;

			case 1: // East
				m_maze[offset(+1, 0)].set_SoftSolved();
				m_maze[offset(+1, 0)].set_Cost(currentCost + 1);
				m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
				//m_nSolvedCells++;
				break;

			case 2: // South
				m_maze[offset(0, +1)].set_SoftSolved();
				m_maze[offset(0, +1)].set_Cost(currentCost + 1);
				m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
				//m_nSolvedCells++;
				break;

			case 3: // West
				m_maze[offset(-1, 0)].set_SoftSolved();
				m_maze[offset(-1, 0)].set_Cost(currentCost + 1);
				m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
				//m_nSolvedCells++;
				break;

			}
			m_nSolvedCells++;

		}
		else {
			//m_maze[offset(0, 0)].unset_Sol();
			m_stack.pop(); // Backtrack
		}
	}
	else { // step 6, work backwards to solve
		if (m_nSolvedCells == m_nMazeHeight * m_nMazeWidth) {
			m_stack.pop();
			m_stack.push(make_pair(m_nMazeWidth - 1, m_nMazeHeight - 1));
			m_nSolvedCells++;
		}

		// Find the smallest Cost neighbor (will be unique)

		//northern neighbor
		if (m_stack.top().second > 0 && m_maze[offset(0, -1)].get_Path_S()) {
			if (m_maze[offset(0, 0)].get_Cost() > m_maze[offset(0, -1)].get_Cost()) {
				xpos = m_stack.top().first;
				ypos = m_stack.top().second;
				m_stack.pop();
				m_stack.push(make_pair(xpos, ypos - 1));
			}
		}

		// Souther Neighbor
		if (m_stack.top().second < m_nMazeHeight - 1 && m_maze[offset(0, +1)].get_Path_N()) {
			if (m_maze[offset(0, 0)].get_Cost() > m_maze[offset(0, +1)].get_Cost()) {
				xpos = m_stack.top().first;
				ypos = m_stack.top().second;
				m_stack.pop();
				m_stack.push(make_pair(xpos, ypos + 1));
			}
		}

		// West Neighbor
		if (m_stack.top().first > 0 && m_maze[offset(-1, 0)].get_Path_E()) {
			if (m_maze[offset(0, 0)].get_Cost() > m_maze[offset(-1, 0)].get_Cost()) {
				xpos = m_stack.top().first;
				ypos = m_stack.top().second;
				m_stack.pop();
				m_stack.push(make_pair(xpos - 1, ypos));
			}
		}

		// East Neighbor
		if (m_stack.top().first < m_nMazeWidth - 1 && m_maze[offset(+1, 0)].get_Path_W()) {
			if (m_maze[offset(0, 0)].get_Cost() > m_maze[offset(+1, 0)].get_Cost()) {
				xpos = m_stack.top().first;
				ypos = m_stack.top().second;
				m_stack.pop();
				m_stack.push(make_pair(xpos + 1, ypos));
			}
		}


		m_maze[offset(0, 0)].set_Solved();

		if (offset(0, 0) == 0) { // we are done!
			return true;
		}
	}


	return false;
}

void Maze::djikstraDraW(int x, int px, int y, int py)
{
	if (m_maze[y * m_nMazeWidth + x].get_Cost() > 0 && m_maze[y * m_nMazeWidth + x].get_Cost() < 50)
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_GREY);
	else if (m_maze[y * m_nMazeWidth + x].get_Cost() >= 50 && m_maze[y * m_nMazeWidth + x].get_Cost() < 100)
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_DARK_GREY);
	else if (m_maze[y * m_nMazeWidth + x].get_Cost() >= 100 && m_maze[y * m_nMazeWidth + x].get_Cost() < 150)
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_CYAN);
	else if (m_maze[y * m_nMazeWidth + x].get_Cost() >= 150 && m_maze[y * m_nMazeWidth + x].get_Cost() < 200)
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_DARK_CYAN);
	else if (m_maze[y * m_nMazeWidth + x].get_Cost() >= 200 && m_maze[y * m_nMazeWidth + x].get_Cost() < 250)
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_BLUE);
	else if (m_maze[y * m_nMazeWidth + x].get_Cost() >= 250 && m_maze[y * m_nMazeWidth + x].get_Cost() < 300)
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_DARK_BLUE);
	else if (m_maze[y * m_nMazeWidth + x].get_Cost() >= 300 && m_maze[y * m_nMazeWidth + x].get_Cost() < 350)
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_YELLOW);
	else if (m_maze[y * m_nMazeWidth + x].get_Cost() >= 350 && m_maze[y * m_nMazeWidth + x].get_Cost() < 400)
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_DARK_YELLOW);
	else if (m_maze[y * m_nMazeWidth + x].get_Cost() >= 400 && m_maze[y * m_nMazeWidth + x].get_Cost() < 450)
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_RED);
	else
		Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_DARK_RED);



}

bool Maze::ratMaze()
{

	// step 1 is to get potential neighbors
	auto offset = [&](int x, int y) {
		return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
	};

	if (offset(0, 0) == m_nMazeHeight * m_nMazeWidth - 1) { // have we reached the end??
		return true;
	}



	vector<int> neighbors;
	bool go_east = false;
	bool go_south = false;


	// North Neightbor
	if (m_stack.top().second > 0 && (m_maze[offset(0, -1)].get_Path_S() && !m_maze[offset(0, -1)].get_Solved()))
		neighbors.push_back(0);

	// East Neightbor
	if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)].get_Path_W() && !m_maze[offset(1, 0)].get_Solved())) {
		neighbors.push_back(1);
		go_east = true;
	}

	// South Neighbor
	if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)].get_Path_N() && !m_maze[offset(0, 1)].get_Solved())) {
		neighbors.push_back(2);
		go_south = true;
	}
	// West Neighbor
	if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)].get_Path_E() && !m_maze[offset(-1, 0)].get_Solved())) {
		neighbors.push_back(3);
	}
	// Are there any neighbors avail?
	if (!neighbors.empty()) {
		//choose one available neighbor at random
		//int next_cell_dir = neighbors[rand() % neighbors.size()];
		int next_cell_dir;
		if (go_east)
			next_cell_dir = 1;
		else if (go_south)
			next_cell_dir = 2;
		else
			next_cell_dir = neighbors[rand() % neighbors.size()];


		// Create a path between the neighbor and the current cell
		switch (next_cell_dir) {

		case 0: // North 
			m_maze[offset(0, -1)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
			break;

		case 1: // East
			m_maze[offset(+1, 0)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
			break;

		case 2: // South
			m_maze[offset(0, +1)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
			break;

		case 3: // West
			m_maze[offset(-1, 0)].set_Solved();
			m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
			break;

		}
	}
	else {
		m_maze[offset(0, 0)].unset_Sol();
		m_stack.pop(); // Backtrack
	}
	return false;
}



// timer stuff

void Maze::start_timer1()
{
	timer1 = std::chrono::high_resolution_clock::now();
}
void Maze::start_timer2()
{
	timer2 = std::chrono::high_resolution_clock::now();
}
void Maze::set_timerDuration()
{
	differenceTimer = chrono::duration_cast<chrono::seconds>(timer2 - timer1).count();
}

long unsigned int Maze::get_timerDuration()const
{
	return differenceTimer;
}






// options menu on SPACE bar press
void Maze::options()
{
	wstring filler, reset, dijkstra, depth, user, wall, rat, end, comp;
	filler += L"#######################################";
	reset += L"PRESS 'R' TO RESET | PRESS 'R' TO RESET";
	dijkstra += L"PRESS 'D' For Dijkstra | PRESS 'D' For ";
	depth += L"PRESS 'S' For DFS | PRESS 'S' For DFS  ";
	user += L"PRESS 'U' For user Solve | PRESS 'U'   ";
	wall += L"PRESS 'W' For WallFollower | PRESS     ";
	rat += L"PRESS 'T' For Rat in a Maze | PRESS    ";
	end += L"PRESS 'Q' To Quit | PRESS 'Q' To Quit  ";
	comp += L"PRESS 'C' To Compare | PRESS 'C' To Com";


	for (int i = 0; i < m_nMazeWidth; ++i) {
		for (int j = 0; j < m_nMazeHeight; ++j) {
			if (j == 0)
				Draw(i, j * (m_nPathWidth + 1), filler[i], FG_WHITE);
			if (j == 1)
				Draw(i, j * (m_nPathWidth + 1), reset[i], FG_WHITE);
			else if (j == 2)
				Draw(i, j * (m_nPathWidth + 1), dijkstra[i], FG_WHITE);
			else if (j == 3)
				Draw(i, j * (m_nPathWidth + 1), depth[i], FG_WHITE);
			else if (j == 4)
				Draw(i, j * (m_nPathWidth + 1), user[i], FG_WHITE);
			else if (j == 5)
				Draw(i, j * (m_nPathWidth + 1), user[i], FG_WHITE);
			else if (j == 6)
				Draw(i, j * (m_nPathWidth + 1), wall[i], FG_WHITE);
			else if (j == 7)
				Draw(i, j * (m_nPathWidth + 1), rat[i], FG_WHITE);
			else if (j == 8)
				Draw(i, j * (m_nPathWidth + 1), comp[i], FG_WHITE);
			else if (j == 9)
				Draw(i, j * (m_nPathWidth + 1), end[i], FG_WHITE);
			if (j == 10)
				Draw(i, j * (m_nPathWidth + 1), filler[i], FG_WHITE);
		}
	}

}

// compare time
bool Maze::compare()
{

	std::string outfile;
	ofstream fout;
	if (algCalls == 0) {
		outfile = "ratMaze.txt";
		if (mazeSolved) { // maze solved
			//start_timer2();
			//set_timerDuration();
			fout.open(outfile.c_str(), ios::app);
			fout << differenceTimer << std::endl;
			fout.close();
			algCalls++;
			restart();
		}
		return ratMaze(); // keep the alg going
	}
	if (algCalls == 1) {
		outfile = "djikstra.txt";
		if (mazeSolved) { // maze solved
			//start_timer2();
			//set_timerDuration();
			fout.open(outfile.c_str(), ios::app);
			fout << differenceTimer << std::endl;
			fout.close();
			algCalls++;
			restart();
		}
		return djikstra(); // keep the alg going
	}
	if (algCalls == 2) {
		outfile = "wallFollower.txt";
		if (mazeSolved) { // maze solved
			//start_timer2();
			//set_timerDuration();
			fout.open(outfile.c_str(), ios::app);
			fout << differenceTimer << std::endl;
			fout.close();
			algCalls++;
			restart();
		}
		return wallFollower(); // keep the alg going
	}
	if (algCalls == 3) {
		outfile = "depthFirstSearch.txt";
		if (mazeSolved) { // maze solved
			//start_timer2();
			//set_timerDuration();
			fout.open(outfile.c_str(), ios::app);
			fout << differenceTimer << std::endl;
			fout.close();
			algCalls++;
			restart();
		}
		return dfs(); // keep the alg going
	}



	return true;
}