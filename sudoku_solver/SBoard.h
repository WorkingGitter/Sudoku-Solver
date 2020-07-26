/* ======================================
* Author : Richard Chin
* Date   : July 2020
* =======================================*/
#pragma once
#include <vector>
#include <set>

// How many cells per side on our board
#define BOARD_SIZE 9

// The size of each sub-blocks
#define BLOCK_SIZE (BOARD_SIZE / 3)

/*******************************
* Integer value of a sudoko cell
********************************/
enum class SValueEnum {
	SValue_Empty = 0,
	SValue_1 = 1,
	SValue_2 = 2,
	SValue_3 = 3,
	SValue_4 = 4,
	SValue_5 = 5,
	SValue_6 = 6,
	SValue_7 = 7,
	SValue_8 = 8,
	SValue_9 = 9,
};

/*
*/
enum class SStateEnum {
	SState_Fixed,		// Cannot be changed.
	SState_Solved,		// Originally free, but now solved.
	SState_New,
	SState_Free			// Not solved, free to change.
};

/*
* Encapsulates the position of a cell
*/
struct SPos 
{
	SPos() {}
	SPos(int c, int r) : col(c), row(r) {}
	bool operator <(const SPos& other) const { 
		return (col < other.col) || ((!(other.col < col)) && (row < other.row));
	}

	int GetBoardIndex() {
		int index = (BOARD_SIZE * row) + col;
		return index;
	}

	int row = 0;
	int col = 0;
};

/*
* Each cell will have a 'state'.
* This includes any indicated value, but could also store if the current value
* was from the user, or have been calculated.
*/
struct SCell 
{
public:
	SCell(SValueEnum v = SValueEnum::SValue_Empty, SStateEnum s = SStateEnum::SState_Free)
		: value(v), state(s)
	{
	}

	SPos position;
	SValueEnum value;
	SStateEnum state;

	bool IsSolved() {
		return (state != SStateEnum::SState_Free);
	}
};

/******************************************
* Encapsulates the sudoko board
*******************************************/
class SBoard
{
public:

	SBoard();

	// Returns the array of cells for board row
	std::vector<SCell> GetRow(int);

	// Returns the array of cells for board row
	std::vector<SCell> GetCol(int);

	/*
	* Returns the 3x3 blocks of the sudoku board as an array of cells.
	* The blocks are indexed from 0 - 8, starting from the top-left.
	*
	* Example, calling this function for the 4th block on the following
	* layout would give the subsequent result:
	*
	* '''''''''''''
	* '796 854 321'
	* '243 176 985'
	* '851 239 476'
	* '   +---+   '
	* '137|965|842'
	* '925|418|763'
	* '468|723|519'
	* '   +---+   '
	* '614 597 238'
	* '582 341 697'
	* '379 682 154'
	* '''''''''''''
	*
	* {9,6,5,4,1,8,7,2,3}
	*
	*/
	std::vector<SCell> GetBlock(int);

	/*
	* Returns the block index from the cell coordinates
	* see: GetBlock()
	*/
	int GetBlockIndexFrom(int col, int row);

	/*
	*/
	std::vector<SPos> GetFreeCells();

	std::vector<SPos> GetSolvedCells();

	/*
	* Returns the cell information structure
	* col and row are the zero-based index of the board cell.
	* This must be in the range 0 - 8.
	*/
	SCell GetCell(int col, int row);

	/*overload*/
	SCell GetCell(SPos p) {
		return GetCell(p.col, p.row);
	}

	/*
	* Faster access version ?
	*/
	SCell & GetCellDirect(int index) {
		return m_boarddata[index];
	}
		
	/*
	* Sets the cell value or state of the given board cell.
	*/
	void SetCell(int col, int row, SCell cell);

	/*overload*/
	void SetCell(SPos p, SCell c) {
		SetCell(p.col, p.row, c);
	}

	/*
	* Clears the entire board with empty values
	*/
	void ClearBoard();

	/*
	* Tests if given array of cells has all the numbers present
	*/
	bool IsSolved(std::vector<SCell>&);

	/*
	* Validate given array of cells
	* Tests for duplicates. Ignores empty cells.
	*/
	bool IsValid(std::vector<SCell>&);

	/*
	* Test to see if given value can be placed at the given cell
	*/
	bool IsValueValidAt(int col, int row, SValueEnum value);

	/* Overloaded function */
	bool IsValueValidAt(SPos pos, SValueEnum value) {
		return IsValueValidAt(pos.col, pos.row, value);
	}

	/*
	* Performs a test on the entire board in its current state for completeness.
	*/
	bool IsBoardSolved();

protected:
	std::vector<SCell> m_boarddata;

	/*
	* Returns the index within our internal array, given the cell coordinates.
	* Cells are referred to in columns/rows
	*/
	int GetCellIndexFrom(int col, int row);
};

