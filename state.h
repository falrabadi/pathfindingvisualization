#pragma once
#pragma once

class state
{
public:
	state();
	void reset();

	// mutators
	void visit();
	void set_Path_N();
	void set_Path_E();
	void set_Path_S();
	void set_Path_W();
	void clear();

	void set_Solved();
	void set_SoftSolved();
	void set_Sol();
	void unset_Sol();

	void set_Cost(int x);
	void incrementCost();


	// accessors
	bool get_Path_N()const;
	bool get_Path_E()const;
	bool get_Path_S()const;
	bool get_Path_W()const;
	bool get_visit()const;

	bool get_Solved()const;
	bool get_Sol()const;


	int get_Cost()const;



private:
	bool CELL_PATH_N;
	bool CELL_PATH_E;
	bool CELL_PATH_S;
	bool CELL_PATH_W;
	bool CELL_VISITED;

	bool CELL_SOLVED;
	bool MAZE_SOL;

	int COST;

};