#include "SBoard.h"
#include <algorithm>
#include <set>
#include <cassert>

// constructor
SBoard::SBoard()
{
	m_boarddata.reserve(BOARD_SIZE * BOARD_SIZE);
	ClearBoard();
}

/*
* Sets all the cells to empty
*/
void SBoard::ClearBoard()
{
	m_boarddata.clear();
	m_boarddata.resize(BOARD_SIZE * BOARD_SIZE);

	for (auto j = 0; j < BOARD_SIZE; j++) {
		for (auto i = 0; i < BOARD_SIZE; i++) {
			SetCell(i, j, SCell{});
		}
	}
}

/*
* Returns the index within the internal board array (m_boarddata).
*/
int SBoard::getCellIndexFrom(int col, int row) 
{
	int index = (BOARD_SIZE * row) + col;
	assert(index < (BOARD_SIZE * BOARD_SIZE));

	return index;
}

/*
*/
std::vector<SCell> SBoard::GetRow(int r)
{
	std::vector<SCell> v;
	for (auto c = 0; c < BOARD_SIZE; c++) {
		v.push_back(GetCell(c, r));
	}
	return v;
}

std::vector<SCell> SBoard::GetCol(int c)
{
	std::vector<SCell> v;
	for (auto r = 0; r < BOARD_SIZE; r++) {
		v.push_back(GetCell(c, r));
	}
	return v;
}

SCell SBoard::GetCell(int col, int row)
{
	if ((col >= BOARD_SIZE) || (row >= BOARD_SIZE))
		return SCell();

	return m_boarddata[getCellIndexFrom(col, row)];
}

void SBoard::SetCell(int col, int row, SCell cell)
{
	if ((col >= BOARD_SIZE) || (row >= BOARD_SIZE))
		return;

	cell.position = SPos{ col,row };
	m_boarddata[getCellIndexFrom(col, row)] = cell;
}

std::vector<SCell> SBoard::GetBlock(int index)
{
	std::vector<SCell> vec;

	int start_column = (index % BLOCK_SIZE) * BLOCK_SIZE;
	int start_row = (index / BLOCK_SIZE) * BLOCK_SIZE;

	for (auto i = start_column; i < (start_column + BLOCK_SIZE); i++) {
		for (auto j = start_row; j < (start_row + BLOCK_SIZE); j++) {
			vec.push_back(GetCell(i, j ));
		}
	}

	return vec;
}

/*
* Checks the 'solved' state of this line of cells.
*/
bool SBoard::IsSolved(std::vector<SCell>& vec)
{
	if (vec.size() != BOARD_SIZE)
		return false;

	// Checks to see if all of the cells have been filled
	bool filled = (std::find_if(begin(vec), end(vec), [](SCell c) { return (c.value == SValueEnum::SValue_Empty); }) == vec.end());
	if (!filled)
		return false;

	// Checks to ensure that all numbers are unique.
	bool unique = (std::unique(vec.begin(), vec.end(), [](SCell a, SCell b) { return (a.value == b.value); }) == vec.end());
	return unique;
}

/*
* Checking to ensure that the values entered is valid.
* Empty cells are allowed, as this can be called on an incomplete board.
*
* Validity at this time only checks for dulicates.
*/
bool SBoard::IsValid(std::vector<SCell>& vec)
{
	std::set<SValueEnum> numset;
	for (auto & c : vec) {
		if (c.value == SValueEnum::SValue_Empty)
			continue;

		// The second parameter of a returned insertion will be false, if the item
		// already exist.
		auto it = numset.insert(c.value);
		if (!it.second)
			return false;
	}
	return true;
}

/*
* Test to see if the given value can be safely inserted at this cell
*/
bool SBoard::IsValueValidAt(int col, int row, SValueEnum value)
{
	bool isvalid = true;

	// Test Row
	// Iterate through the row cells, and compare against our test value.
	// If it is the same (and not empty), then the test fails.
	int rowindex = BOARD_SIZE * row;
	for (int r = rowindex; r < (rowindex + BOARD_SIZE); r++) {
		auto row_value = m_boarddata[r].value;
		if ((row_value != SValueEnum::SValue_Empty) && (row_value == value)) {
			isvalid = false;
		}
	}

	// Test Column
	// Here we are moving down the grid.
	int colindex = getCellIndexFrom(col, 0);
	for (int c = 0; c < BOARD_SIZE; c++) {
		auto col_value = m_boarddata[colindex].value;
		if ((col_value != SValueEnum::SValue_Empty) && (col_value == value)) {
			isvalid = false;
		}
		colindex += BOARD_SIZE;
	}

	// Test Block
	int bidx = GetBlockIndexFrom(col, row);
	int start_column = (bidx % BLOCK_SIZE) * BLOCK_SIZE;
	int start_row = (bidx / BLOCK_SIZE) * BLOCK_SIZE;
	int bindex = (start_row * BOARD_SIZE) + start_column;

	for (int r = 0; r < BLOCK_SIZE; r++) {
		for (int c = 0; c < BLOCK_SIZE; c++) {
			int block_index = (r * BOARD_SIZE) + c + bindex;
			auto block_value = m_boarddata[block_index].value;
			if ((block_value != SValueEnum::SValue_Empty) && (block_value == value)) {
				isvalid = false;
			}
		}
	}
	
	return isvalid;
}

/*
* Returns the index of the block for the given cell
*/
int SBoard::GetBlockIndexFrom(int col, int row)
{
	int cell_index = getCellIndexFrom(col, row);
	int block_col = (cell_index / BLOCK_SIZE) % BLOCK_SIZE;
	int block_row = (cell_index / BOARD_SIZE) / BLOCK_SIZE;
	int block_index = (block_row * BLOCK_SIZE) + block_col;
	return block_index;
}

bool SBoard::IsBoardSolved()
{
	bool IsRowSolved   = true;
	bool IsColSolved   = true;
	bool IsBlockSolved = true;

	for (auto col = 0; col < BOARD_SIZE; col++) {
		auto col_vec = GetCol(col);
		IsColSolved &= IsSolved(col_vec);
	}

	for (auto row = 0; row < BOARD_SIZE; row++) {
		auto row_vec = GetCol(row);
		IsRowSolved &= IsSolved(row_vec);
	}

	for (auto b : { 0,1,2,3,4,5,6,7,8 }) {
		auto block_vec = GetCol(b);
		IsBlockSolved &= IsSolved(block_vec);
	}

	return IsRowSolved && IsColSolved && IsBlockSolved;
}
