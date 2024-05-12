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

/*!	\brief Integer value of a sudoko cell
*/
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

enum class SStateEnum {
	SState_Fixed,		// Cannot be changed.
	SState_Solved,		// Originally free, but now solved.
	SState_New,
	SState_Free			// Not solved, free to change.
};

/*!	\brief Encapsulates the position of a cell
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

/*!	\brief Encapsulates the cell information
* 
* Each cell will have a 'state'.
* This includes any indicated value, but could also store if the current value
* was from the user, or have been calculated.
* 
*/
struct SCell 
{
public:
	SCell() {
		position = SPos(0, 0);
		value = SValueEnum::SValue_Empty;
		state = SStateEnum::SState_Free;
	}

	SCell(SValueEnum v, SStateEnum s = SStateEnum::SState_Free)
		: position{}, value(v), state(s)
	{
	}
	~SCell() = default;

	// copy constructor
	SCell(const SCell& other) {
		position = other.position;
		value = other.value;
		state = other.state;
	}	

	// assignment operator
	SCell& operator=(const SCell& other) {
		if (this != &other) {
			position = other.position;
			value = other.value;
			state = other.state;
		}
		return *this;
	}

	SPos position = SPos(0, 0);
	SValueEnum value = SValueEnum::SValue_Empty;
	SStateEnum state = SStateEnum::SState_Free;

	[[nodiscard]] bool IsSolved() const {
		return (state != SStateEnum::SState_Free);
	}
};

/*!	\brief Encapsulates a sudoko board
* 
*   The algorithm to solve the board is
*   kept in `SudokuSolver`.
*/
class SBoard
{
public:

	SBoard();

	// Returns the array of cells for board row
	std::vector<SCell> GetRow(int) const;

	// Returns the array of cells for board row
	std::vector<SCell> GetCol(int) const;

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
	std::vector<SCell> GetBlock(int) const;

	/*!	\brief Returns the block index from the cell coordinates
	* 
	*	
	*	\see: GetBlock()
	*/
	int GetBlockIndexFrom(int col, int row) const;

	std::vector<SPos> GetFreeCells();

	std::vector<SPos> GetSolvedCells();

	/*
	* Returns the cell information structure
	* col and row are the zero-based index of the board cell.
	* This must be in the range 0 - 8.
	*/
	SCell GetCell(int col, int row) const;

	/*overload*/
	SCell GetCell(SPos p) const {
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
    [[nodiscard]] bool IsSolved(std::vector<SCell>& cells) const;

	/*
	* Validate given array of cells
	* Tests for duplicates. Ignores empty cells.
	*/
	[[nodiscard]] bool IsValid(std::vector<SCell>&) const;

	/*
	* Test to see if given value can be placed at the given cell
	*/
	[[nodiscard]] bool IsValueValidAt(int col, int row, SValueEnum value) const;

	/* Overloaded function */
	[[nodiscard]] bool IsValueValidAt(SPos pos, SValueEnum value) const {
		return IsValueValidAt(pos.col, pos.row, value);
	}

	/*
	* Performs a test on the entire board in its current state for completeness.
	*/
	bool IsBoardSolved() const;

	/*!	\brief Returns the visual character to represent given cell state
	* 
	*	\return A valid representation of a given board cell.
	*			Valid values are '1' - '9' for the numbers, and '.' for empty cells.
	*/
	static wchar_t CellToCharacter(SCell);

	/*!	\brief returns the board cell representation from the given character
	*	
	*	\param c - A valid board cell character, as defined in `CellToCharacter()`
	*	\return SCell
	*/
	static SCell CharacterToCell(wchar_t c);


protected:
	std::vector<SCell> m_boarddata;

	/*
	* Returns the index within our internal array, given the cell coordinates.
	* Cells are referred to in columns/rows
	*/
	[[nodiscard]] int GetCellIndexFrom(int col, int row) const;
};

