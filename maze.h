#pragma once
#pragma once
#include <iostream>
#include <fstream>;
#include <stack>
#include <chrono>
using namespace std;

#include "olcConsoleGameEngine.h"
#include "state.h"

class Maze : public olcConsoleGameEngine
{
public:
	Maze();

	void generate();
	void draw();

	bool solver(char c);
	void restart();
	void hardRestart();

	void set_userChoice(char c);
	char get_userChoice()const;

private:
	int m_nMazeWidth;
	int m_nMazeHeight;
	state* m_maze;

	int m_nVisitedCells;
	stack<pair<int, int>> m_stack;
	int m_nPathWidth;


	int m_nSolvedCells;
	int algCalls;
	// maze solving algs
private:
	bool mazeSolved;
	bool wallFollower();
	int nCall = 0; // how many times the else func is called in OnUserUpdate()

	bool userSolver();
	bool dfs();
	bool djikstra();
	void djikstraDraW(int x, int px, int y, int py); // big fancy dijkstra needs his own draw func smh
	bool ratMaze();


	char userChoice;

private:
	// timer variables
	std::chrono::time_point<std::chrono::high_resolution_clock> timer1;
	std::chrono::time_point<std::chrono::high_resolution_clock> timer2;
	long unsigned int differenceTimer;

public:
	void start_timer1();
	void start_timer2();
	void set_timerDuration();
	long unsigned int get_timerDuration()const;

private:
	void options();
	bool compare();


protected:
	virtual bool OnUserCreate()override;
	virtual bool OnUserUpdate(float fElapsedTime)override;
};