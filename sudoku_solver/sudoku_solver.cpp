/* ======================================
* Author : Richard Chin
* Date   : July 2020
* =======================================*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include <map>
#include <set>
#include "ConsoleIO.h"
#include "SBoard.h"
#include "s_timer.h"

#define BUILD_VERSION L"0.7"

// Method 2
// Stores: (value within a block) => [set of cell coord]
using BlockDivisionCollection = std::map< SValueEnum, std::set<std::pair<int,int>> >;


SBoard sboard;

void PrintHelp();
void PrintBoardToConsole(SBoard &);
wchar_t GetDisplayChar(SCell);
void WriteBoardToFile(SBoard board, std::wstring filename);

SCell GetBoardCellFrom(wchar_t c);
bool LoadBoardState(std::wstring);

bool FindByElimination(SBoard & board);
bool SolveBoardByRecursion(SBoard);
bool SolveBoardByElimination(SBoard &);

/*********************************
* Main application entry point
**********************************/
int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	// print about line if no parameters given
	if(argc < 2)
		PrintHelp();

	// options variables
	bool iscreate = false;
	bool issolve = false;
	std::wstring filename;

	std::wstring param_create{ L"-c" };
	std::wstring param_solve{ L"-s" };

	// loop through all our parameters and set our options variables
	// NB: The first parameter will be the application filename, so we want to
	//     skip this.
	for (int n = 1; n < argc; n++) {

		bool create = (param_create.compare(argv[n]) == 0);
		bool solve  = (param_solve.compare(argv[n]) == 0);
		iscreate   |= create;
		issolve    |= solve;

		if (!create && !solve) {
			filename = argv[n];
		}
	}

	if (filename.empty()) {
		filename = L"sudoku_template.txt";
	}

	// remove any enclosing quotes
	filename.erase(std::remove(filename.begin(), filename.end(), L'\"'), filename.end());

	// create a new, empty board and write to file
	if (iscreate) {
		WriteBoardToFile({}, filename);
	}

	if (issolve) {

		{
			CConsoleIO console;
			console.ClearScreen();
		}

		if (!LoadBoardState(filename)) {
			std::wcerr << L"# Failed to load board settings" << std::endl;
			return 1;
		}

		timer t;
		t.start();

		bool has_solved = SolveBoardByElimination(sboard);
		if (!has_solved) {
			has_solved = SolveBoardByRecursion(sboard);
		}

		t.stop();

		if (!has_solved) {
			std::wcerr << L"# Failed to solve given board" << std::endl;
		}
		else {
			std::wcout << L"Board has been solved" << std::endl;
		}

		std::wcout << L"Completed in " << t.get_elapsedtime_sec() << L" secs" << std::endl;

	}

	return 0;
}

/*
* Draws the current contents of the board to the console window.
* 
* Example:
* +---+---+---+
* |123|123|123|
* |456| 56|4 6|
* |789|7  | 89|
* +---+---+---+
* |123|123|123|
* |456|456|456|
* |789|789|789|
* +---+---+---+
* |123|123|123|
* |4X6|456|456|
* |789|789|789|
* +---+---+---+
*/
void PrintBoardToConsole(SBoard& board)
{
	std::wstring dividerline = L"+---+---+---+";	

	CConsoleIO console;
	console.SetCursorPos(0, 0);	

	for (int line = 0; line < BOARD_SIZE; line++) {

		auto board_line = board.GetRow(line);

		if ((line % 3) == 0) {
			std::wcout << dividerline.c_str() << std::endl;
		}

		console.PushColourAttributes();
		for (auto ncol = 0; ncol < 9; ncol++) {

			// draw vertical separator
			if ((ncol % 3) == 0) {
				std::wcout << L"|";
			}

			// draw value
			switch (board_line[ncol].state) {
			case SStateEnum::SState_Fixed:
				console.SetColourAttributes(FOREGROUND_WHITE);
				break;
			case SStateEnum::SState_Solved:
				console.SetColourAttributes(FOREGROUND_LIGHTYELLOW);
				break;
			case SStateEnum::SState_New:
				console.SetColourAttributes(FOREGROUND_LIGHTAQUA);
				break;
			default: 
				console.SetColourAttributes(FOREGROUND_WHITE);
				break;				
			}
			std::wcout << GetDisplayChar(board_line[ncol]);
		}

		console.PopColourAttributes();
		std::wcout << L"|" << std::endl;
	}

	std::wcout << dividerline.c_str() << std::endl;
	
}

/*
* Creates the given file and writes the given board.
* The file will be created in the application directory.
*/
void WriteBoardToFile(SBoard board, std::wstring filename)
{
	std::filesystem::path file(filename);
	auto curpath = std::filesystem::current_path();
	auto finalpath = curpath / file;

	std::wofstream outfile(finalpath.c_str());

	// write to file
	// 
	// Similar code structure to the one used in PrintBoardToConsole().
	// TODO: Consolidate 
	std::wstringstream ss;
	std::wstring dividerline = L"---+---+---";

	for (int line = 0; line < BOARD_SIZE; line++) {

		auto board_line = board.GetRow(line);

		if ((line > 0) && (line % 3) == 0) {
			ss << dividerline.c_str() << std::endl;
		}

		ss  << GetDisplayChar(board_line[0])
			<< GetDisplayChar(board_line[1])
			<< GetDisplayChar(board_line[2]);

		ss << L"|"
			<< GetDisplayChar(board_line[3])
			<< GetDisplayChar(board_line[4])
			<< GetDisplayChar(board_line[5]);

		ss << L"|"
			<< GetDisplayChar(board_line[6])
			<< GetDisplayChar(board_line[7])
			<< GetDisplayChar(board_line[8])
			<< std::endl;
	}
	ss << std::endl;
	outfile << ss.str();
	outfile.close();
}

/*
* Returns the character representation of the given 
* board cell.
*/
wchar_t GetDisplayChar(SCell cell) 
{
	wchar_t c{ (TCHAR)' ' };

	switch (cell.value) {

	case SValueEnum::SValue_1: c = L'1'; break;
	case SValueEnum::SValue_2: c = L'2'; break;
	case SValueEnum::SValue_3: c = L'3'; break;
	case SValueEnum::SValue_4: c = L'4'; break;
	case SValueEnum::SValue_5: c = L'5'; break;
	case SValueEnum::SValue_6: c = L'6'; break;
	case SValueEnum::SValue_7: c = L'7'; break;
	case SValueEnum::SValue_8: c = L'8'; break;
	case SValueEnum::SValue_9: c = L'9'; break;
	case SValueEnum::SValue_Empty: c = L'.';
	default:
		break;
	}

	return c;
}

/*
* Displayed if no parameters provided by the user.
*/
void PrintHelp()
{
	CConsoleIO console;
	console.PushColourAttributes();

	console.SetColourAttributes(FOREGROUND_WHITE);
	std::wcout << L"Sudoko Solver " << BUILD_VERSION << std::endl;
	std::wcout << L"Usage:" << std::endl;

	console.SetColourAttributes(FOREGROUND_LIGHTYELLOW);
	std::wcout << L"  SSolve.exe -c[reate] -s[solve] <filename.txt>" << std::endl;
	console.PopColourAttributes();
}

/******************************************************************************
* Initialises our global board with the values provided in the input file.
* This file must be in a fixed format. Users can generate a blank board using the 
* '-c' parameter. You can then use this template to set the cell values.
*
* Input file content must be of the form:
*
* ...|...|...
* ...|...|...
* ...|...|...
* ---+---+---
* ...|...|...
* ...|...|...
* ...|...|...
* ---+---+---
* ...|...|...
* ...|...|...
* ...|...|...
*
* Or the slimline format:
* .........
* .........
* .........
* .........
* .........
* .........
* .........
* .........
* .........
*******************************************************************************/
bool LoadBoardState(std::wstring filename)
{
	// Get the full path of the input file
	// If the full path is not supplied, then use the current folder location.
	std::filesystem::path inpath(filename);
	if (!inpath.has_parent_path()) {
		std::filesystem::path file(filename);
		auto finalpath = std::filesystem::current_path() / inpath.filename();
		inpath = finalpath;
	}

	// load the input file into a vector array
	std::vector<std::wstring> linevec;
	linevec.reserve(16);
	std::wifstream in(inpath.c_str());
	if (!in.is_open())
		return false;

	std::wstring line;
	while (std::getline(in, line)) {
		linevec.emplace_back(line);
	}
	in.close();

	// lets do some validity checks
	if (linevec.size() < 9) {
		std::wcerr << L"# not enough board lines" << std::endl;
		return false;
	}

	int currow = 0;
	for (auto l : linevec) {
		if (l.size() < 9) {
			std::wcerr << L"# not enough board columns on row " << (currow + 1) << std::endl;
		}
		currow++;
	}

	int row = 0;
	for (auto & l : linevec) {
		if (l.empty() || (l[0] == L'+') || (l[0] == L'-'))
			continue;

		int col = 0;
		for (auto & c : l) {
			if ((c == L'|'))
				continue;

			sboard.SetCell(col, row, GetBoardCellFrom(c));
			col++;
		}
		row++;

		if (row > BOARD_SIZE)
			break;
	}

	return true;
}

SCell GetBoardCellFrom(wchar_t c)
{
	SCell cell{ SValueEnum::SValue_Empty, SStateEnum::SState_Fixed };

	switch (c) {
	case L'1': cell.value = SValueEnum::SValue_1; break;
	case L'2': cell.value = SValueEnum::SValue_2; break;
	case L'3': cell.value = SValueEnum::SValue_3; break;
	case L'4': cell.value = SValueEnum::SValue_4; break;
	case L'5': cell.value = SValueEnum::SValue_5; break;
	case L'6': cell.value = SValueEnum::SValue_6; break;
	case L'7': cell.value = SValueEnum::SValue_7; break;
	case L'8': cell.value = SValueEnum::SValue_8; break;
	case L'9': cell.value = SValueEnum::SValue_9; break;
	default: cell.state = SStateEnum::SState_Free; break;
	}
	return cell;
}

/*
* Finds values for cells through a process of eliminating all other possibilities.
* Return true if any addition to the board has been made.
*/
bool FindByElimination(SBoard & board) 
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
				auto & cell = board.GetCellDirect(block_index);
				if (cell.IsSolved())
					continue;

				// If any of the values are valid for this cell, then add to 
				// collection
				for (auto & v : { 1,2,3,4,5,6,7,8,9 }) {

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
	for (auto & block : openBlocks) {
		for (auto & validvalues : block) {
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

bool SolveBoardByRecursion(SBoard board)
{
	FindByElimination(board);
	PrintBoardToConsole(board);
	if (board.IsBoardSolved())
		return true;

	// Find next free cell
	for (auto j = 0; j < BOARD_SIZE; j++) {
		for (auto i = 0; i < BOARD_SIZE; i++) {
			auto cell = board.GetCell(i, j);
			if (!cell.IsSolved()) {
				// For all values, set and solve.
				for (auto & v : { 1,2,3,4,5,6,7,8,9 }) {
					SValueEnum testValue = static_cast<SValueEnum>(v);
					if (board.IsValueValidAt(cell.position, testValue)) {
						cell.value = testValue;
						cell.state = SStateEnum::SState_New;
						board.SetCell(cell.position.col, cell.position.row, cell);
						if (SolveBoardByRecursion(board))
						{
							return true;
						}
					}
				}
				board.SetCell(cell.position.col, cell.position.row, SCell{});
				return false;
			}
		}
	}
	return false;
}

/*
*/
bool SolveBoardByElimination(SBoard & board)
{
	bool is_solved = false;

	int iteration = 0;
	while (!is_solved) {
		bool boardHasChanged = FindByElimination(board);
		is_solved = board.IsBoardSolved();

		// Give up if we are stuck.
		if (!is_solved && !boardHasChanged)
			break;

		PrintBoardToConsole(board);
		iteration++;
	}

	return is_solved;
}
