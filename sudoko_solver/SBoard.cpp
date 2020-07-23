#include "SBoard.h"

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

