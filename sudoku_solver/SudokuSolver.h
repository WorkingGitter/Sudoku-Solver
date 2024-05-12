/*
*  Auther: Richard Chin
*  Date  : May 2024
*/
#pragma once

#include "SBoard.h"


class SudokuSolver
{

public:
	bool ReverseSolve(SBoard&);
	bool FindByElimination(SBoard& board);
	bool SolveBoardByElimination(SBoard&, int& steps);
	bool SolveBoardByRecursion(SBoard, SBoard* pBoard /*= nullptr*/, int & steps);
};

