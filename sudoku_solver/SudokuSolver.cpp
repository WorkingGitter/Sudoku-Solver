/*
*  Auther: Richard Chin
*  Date  : May 2024
*/

#include "SudokuSolver.h"
#include <map>
#include <random>


/*
* Finds values for cells through a process of eliminating all other possibilities.
* Return true if any addition to the board has been made.
*/
bool SudokuSolver::FindByElimination(SBoard& board)
{
	// Stores: (value within a block) => [set of cell coord]
	using ValueLookupMap = std::map< SValueEnum, std::set<SPos> >;

	// For each block on our board, we will monitor the free cells.
	// Keeping a list of all valid values for each cell (within a block).
	// Each block stores: [value] => {valid cells list}
	std::vector< ValueLookupMap > openBlocks(BOARD_SIZE);

	for (auto nblock : { 0,1,2,3,4,5,6,7,8 }) {

		int start_column = (nblock % BLOCK_SIZE) * BLOCK_SIZE;
		int start_row = (nblock / BLOCK_SIZE) * BLOCK_SIZE;
		int bindex = (start_row * BOARD_SIZE) + start_column;

		for (int r = 0; r < BLOCK_SIZE; r++) {
			for (int c = 0; c < BLOCK_SIZE; c++) {

				int block_index = (r * BOARD_SIZE) + c + bindex;
				auto& cell = board.GetCellDirect(block_index);
				if (cell.IsSolved())
					continue;

				// If any of the values are valid for this cell, then add to 
				// collection
				for (auto& v : { 1,2,3,4,5,6,7,8,9 }) {

					SValueEnum testValue = static_cast<SValueEnum>(v);
					if (board.IsValueValidAt(cell.position, testValue)) {

						auto it = openBlocks[nblock].insert({ testValue, {} });
						it.first->second.insert(cell.position);

					}
				}
			}
		}
	}

	// Check for any definitive solution
	bool aSolutionFound = false;
	for (auto& block : openBlocks) {
		for (auto& validvalues : block) {
			if (validvalues.second.size() == 1) {
				board.SetCell(
					validvalues.second.begin()->col,
					validvalues.second.begin()->row,
					{ validvalues.first, SStateEnum::SState_Solved }	// value/state
				);
				aSolutionFound = true;
			}
		}
	}
	return aSolutionFound;
}

/*
*
*/
bool SudokuSolver::SolveBoardByRecursion(SBoard board, SBoard* pBoard /*= nullptr*/, int& steps)
{
	FindByElimination(board);

	if (board.IsBoardSolved()) {

		// copy solved board 
		if (pBoard != nullptr) {
			*pBoard = board;
		}

		return true;
	}

	steps++;

	std::random_device rd;
	std::mt19937 g(rd());

	// Find next free cell
	for (auto j = 0; j < BOARD_SIZE; j++) {
		for (auto i = 0; i < BOARD_SIZE; i++) {
			auto cell = board.GetCell(i, j);
			if (!cell.IsSolved()) {

				// Go through all the possible values and see if we have solved
				// the board.
				//
				// NB: Randomly shuffle the order. This is useful if we are generating
				//     from a blank canvas.
				std::vector<int> seq{ 1,2,3,4,5,6,7,8,9 };
				std::shuffle(seq.begin(), seq.end(), g);

				for (auto& v : seq) {
					SValueEnum testValue = static_cast<SValueEnum>(v);
					if (board.IsValueValidAt(cell.position, testValue)) {
						cell.value = testValue;
						cell.state = SStateEnum::SState_New;
						board.SetCell(cell.position.col, cell.position.row, cell);

						if (SolveBoardByRecursion(board, pBoard, steps))
						{
							return true;
						}
					}
				}

				board.SetCell(cell.position.col, cell.position.row, SCell{});

				// copy solved board 
				if (pBoard != nullptr) {
					*pBoard = board;
				}

				return false;
			}
		}
	}
	return false;
}

bool SudokuSolver::SolveBoardByElimination(SBoard& board, int& steps)
{
	bool is_solved = false;

	while (!is_solved) {
		bool boardHasChanged = FindByElimination(board);
		is_solved            = board.IsBoardSolved();

		// Give up if we are stuck.
		if (!is_solved && !boardHasChanged)
			break;

		steps++;
	}
	return is_solved;
}

bool SudokuSolver::ReverseSolve(SBoard& board)
{
	std::random_device rd;
	std::mt19937 g(rd());

	// compile array of available cells.
	auto solved_cells = board.GetSolvedCells();

	// shuffle the cells so that we get a random location
	std::shuffle(std::begin(solved_cells), std::end(solved_cells), g);

	for (auto pos : solved_cells) {

		// Algorithm
		// If I remove this value from this position, how many other values can
		// take its place ?
		// If only one, then go ahead and remove.

		// clear cell and store previous
		auto prevcell = board.GetCell(pos);
		board.SetCell(pos, SCell{ SValueEnum::SValue_Empty });

		bool onlyone = true;
		SValueEnum value_found = SValueEnum::SValue_Empty;
		for (auto& v : { 1,2,3,4,5,6,7,8,9 }) {

			SValueEnum test_value = static_cast<SValueEnum>(v);
			if (board.IsValueValidAt(pos, test_value)) {
				value_found = test_value;
				if (value_found != prevcell.value)
					onlyone = false;
			}
		}

		//if (!(onlyone && (value_found == prevcell.value)))
		//{
		//	assert(false);
		//	return false;
		//}

		if (!onlyone) {
			board.SetCell(pos, prevcell);
		}
	}

	return true;
}