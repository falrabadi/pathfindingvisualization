#include <iostream>
#include <stack>
#include "maze.h"
#include "olcConsoleGameEngine.h"

//protoypes
char menu();


int main()
{

	std::cout << "\nWelcome to Pathfinding Visualisation" << endl;
	std::cout << "Produced by: Team C++ 1\n" << endl;
	std::cout << "You can use this program to select an alogirthm to solve mazes.\n";

	char choice = menu();
	std::cout << choice << endl;


	std::cin.clear();
	std::cin.ignore();
	std::cout << "Before We Begin: PRESS and HOLD 'SPACE' to view options:";
	do
	{
		cout << '\n' << "Press 'ENTER' to continue...";
	} while (cin.get() != '\n');

	if (choice != ' ') {
		Maze game;
		game.set_userChoice(choice);
		game.ConstructConsole(160, 100, 8, 8);
		game.Start();
	}

	return 0;
}



char menu()
{
	int selection;
	char c = ' ';


	std::cout << "Enter a number 1-5 to select an algorithm:" << endl
		<< "1: Dijkstra" << endl
		<< "2: Depth First Search" << endl
		<< "3: User Solve" << endl
		<< "4: Wallfollower" << endl
		<< "5: Rat in a Maze" << endl
		<< "6: Compare time between algorithms" << endl
		<< "0: Exit" << endl;
	std::cin >> selection;
	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		selection = -1; // error handling done inside switch
	}



	switch (selection) {
	default:
		std::cout << "!!! Error: Could not parse input !!!" << std::endl;
		std::cout << "~~~ Let's try that again ~~~" << std::endl;
		c = menu(); // recall menu()
		break;
	case 1: // dijkstra
		c = 'D';
		break;
	case 2: // DFS
		c = 'S';
		break;
	case 3: // userSolve
		c = 'U';
		break;
	case 4: // Wallfollower
		c = 'W';
		break;
	case 5: // Rat in a Maze
		c = 'T';
		break;
	case 6: // compare
		c = 'C';
		break;
	case 0:
		c = ' '; // Do not run : Break
		break;
	} // end switch(int selection)
	return c;
}