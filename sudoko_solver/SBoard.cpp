#include "SBoard.h"

// constructor
SBoard::SBoard()
{
	m_boarddata.reserve(BOARD_SIZE * BOARD_SIZE);
}

std::vector<SCellStruct> SBoard::GetRow(int)
{
	std::vector<SCellStruct> v(9);
	return v;
}

std::vector<SCellStruct> SBoard::GetCol(int)
{
	std::vector<SCellStruct> v(9);
	return v;
}

SCellStruct SBoard::GetCell(int col, int row)
{
	return SCellStruct();
}

void SBoard::SetCell(int col, int row, SCellStruct cell)
{
}

