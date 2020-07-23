#include "SBoard.h"
#include <algorithm>
#include <set>

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
	for (auto n = 0; n < BOARD_SIZE * BOARD_SIZE; n++)
		m_boarddata.push_back({});
}

/*
*/
std::vector<SCellStruct> SBoard::GetRow(int r)
{
	std::vector<SCellStruct> v;
	for (auto c = 0; c < BOARD_SIZE; c++) {
		v.push_back(GetCell(c, r));
	}
	return v;
}

std::vector<SCellStruct> SBoard::GetCol(int c)
{
	std::vector<SCellStruct> v;
	for (auto r = 0; r < BOARD_SIZE; r++) {
		v.push_back(GetCell(c, r));
	}
	return v;
}

SCellStruct SBoard::GetCell(int col, int row)
{
	if ((col >= BOARD_SIZE) || (row >= BOARD_SIZE))
		return SCellStruct();

	int index = (BOARD_SIZE * row) + col;
	return m_boarddata[index];
}

void SBoard::SetCell(int col, int row, SCellStruct cell)
{
	if ((col >= BOARD_SIZE) || (row >= BOARD_SIZE))
		return;

	int index = (BOARD_SIZE * row) + col;
	m_boarddata[index] = cell;
}

#if 0
bool SBoard::IsRowSolved(int row)
{
	return false;
}

bool SBoard::IsColSolved(int col)
{
	return false;
}

bool SBoard::IsRowValid(int row)
{
	return false;
}

bool SBoard::IsColValid(int col)
{
	return false;
}
#endif 

std::vector<SCellStruct> SBoard::GetBlock(int index)
{
	std::vector<SCellStruct> vec;

	int start_column = (index % 3);
	int start_row = (index / 3);

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
bool SBoard::IsSolved(std::vector<SCellStruct>& vec)
{
	if (vec.size() != BOARD_SIZE)
		return false;

	// Checks to see if all of the cells have been filled
	bool filled = (std::find_if(begin(vec), end(vec), [](SCellStruct c) { return (c.value == SValueEnum::SValue_Empty); }) == vec.end());
	if (!filled)
		return false;

	// Checks to ensure that all numbers are unique.
	bool unique = (std::unique(vec.begin(), vec.end(), [](SCellStruct a, SCellStruct b) { return (a.value == b.value); }) == vec.end());
	return unique;
}

/*
* Checking to ensure that the values entered is valid.
* Empty cells are allowed, as this can be called on an incomplete board.
*
* Validity at this time only checks for dulicates.
*/
bool SBoard::IsValid(std::vector<SCellStruct>& vec)
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
