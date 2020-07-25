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

#define BUILD_VERSION L"0.8"

#define CELL_COLOUR_FIXED FOREGROUND_WHITE
#define CELL_COLOUR_SOLVED FOREGROUND_LIGHTYELLOW
#define CELL_COLOUR_ATTEMPT FOREGROUND_LIGHTAQUA

// global puzzle board
SBoard sboard;

// iteration or moves in solve
int _iteration = 0;

void   PrintHelp();
void    DisplayBoardToConsole(SBoard &, int indent = 0);
void    WriteBoardToTextFile(SBoard board, std::wstring filename);
wchar_t GetBoardCellDisplayCharacter(SCell);

SCell GetBoardCellFrom(wchar_t c);
bool  LoadBoardState(std::wstring source, bool useClipboard = false);

// solving algorithms
bool FindByElimination(SBoard & board);
bool SolveBoardByRecursion(SBoard, SBoard* pBoard = nullptr);
bool SolveBoardByElimination(SBoard &);

/*********************************
* Main application entry point
**********************************/
int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	// print about line if no parameters given
	if(argc < 2)
		PrintHelp();

	// Options
	// These will be determined from the input parameters
	bool action_create           = false;			// generate blank template layout
	bool action_solve            = false;			// solve given board (using file or clipboard)
	bool action_useclipboarddata = false;			// use data in clipboard as source
	std::wstring param_create    = { L"-c" };		//
	std::wstring param_solve     = { L"-s" };		//
	std::wstring filename        = { L"" };			//

	// Loop through all our parameters and set our options variables
	// NB: The first parameter will be the application filename, so we want to
	//     skip this.
	for (int n = 1; n < argc; n++) {

		bool create = (param_create.compare(argv[n]) == 0);
		bool solve  = (param_solve.compare(argv[n]) == 0);
		action_create   |= create;
		action_solve    |= solve;

		if (!create && !solve) {
			filename = argv[n];
		}
	}

	// If no filename supplied, use a default.
	if (filename.empty()) {
		filename = L"puzzleboard.txt";
		action_useclipboarddata = true;
	}

	// remove any enclosing quotes
	filename.erase(std::remove(filename.begin(), filename.end(), L'\"'), filename.end());

	// If we are generating a blank layout, then print to target file and
	// also display to screen.
	if (action_create) {
		CConsoleIO con;
		con.ClearScreen();
		SBoard b;
		WriteBoardToTextFile(b, filename);
		DisplayBoardToConsole(b);
		return 0;
	}

	if (action_solve) {

		{
			CConsoleIO console;
			console.ClearScreen();
		}

		if (!LoadBoardState(filename, action_useclipboarddata)) {
			std::wcerr << L"# Failed to load board settings" << std::endl;
			return 1;
		}

		timer t;
		t.start();

		SBoard unsolved_board(sboard);
		SBoard solved_board;

		bool has_solved = SolveBoardByElimination(sboard);

		if (has_solved) {
			solved_board = sboard;
		}
		else {
			has_solved = SolveBoardByRecursion(sboard, &solved_board);
		}

		t.stop();

		// Print out final layout
		{
			CConsoleIO console;
			console.ClearScreen();
			DisplayBoardToConsole(unsolved_board, 0);
			DisplayBoardToConsole(solved_board, 15);

			// display legend
			auto tc = console.GetColourAttributes();
			console.PushColourAttributes();
			console.SetColourAttributes(CELL_COLOUR_FIXED);
			std::wcout << L"ллл";
			console.SetColourAttributes(tc);
			std::wcout << L" : Fixed Numbers" << std::endl;

			console.SetColourAttributes(CELL_COLOUR_SOLVED);
			std::wcout << L"ллл";
			console.SetColourAttributes(tc);
			std::wcout << L" : Solved Numbers" << std::endl;

			console.SetColourAttributes(CELL_COLOUR_ATTEMPT);
			std::wcout << L"ллл";
			console.SetColourAttributes(tc);
			std::wcout << L" : Current Attempts" << std::endl << std::endl;

			console.PopColourAttributes();
		}

		if (!has_solved) {
			CConsoleIO console;
			console.PushColourAttributes();
			console.SetColourAttributes(FOREGROUND_LIGHTRED);
			std::wcerr << L"Failed to solve given board, in ";
			std::wcout << _iteration << L" attempts" << std::endl;
			console.PopColourAttributes();
		}
		else {
		std::wcout << L"Board has been solved, in ";
		std::wcout << _iteration << L" attempts" << std::endl;
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
void DisplayBoardToConsole(SBoard& board, int indent /*= 0*/)
{
	CConsoleIO console;
	std::wstring dividerline = L"+---+---+---+";

	console.SetCursorPos(indent, 0);

	for (int line = 0; line < BOARD_SIZE; line++) {

		auto board_line = board.GetRow(line);

		if ((line % 3) == 0) {
			std::wcout << dividerline.c_str() << std::endl;
			console.SetCursorX(indent);
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
				console.SetColourAttributes(CELL_COLOUR_FIXED);
				break;
			case SStateEnum::SState_Solved:
				console.SetColourAttributes(CELL_COLOUR_SOLVED);
				break;
			case SStateEnum::SState_New:
				console.SetColourAttributes(CELL_COLOUR_ATTEMPT);
				break;
			default: 
				console.SetColourAttributes(CELL_COLOUR_FIXED);
				break;				
			}
			std::wcout << GetBoardCellDisplayCharacter(board_line[ncol]);
		}

		console.PopColourAttributes();
		std::wcout << L"|" << std::endl;
		console.SetCursorX(indent);
	}

	std::wcout << dividerline.c_str() << std::endl;
}

/*
* Creates the given file and writes the given board.
* The file will be created in the application directory.
*/
void WriteBoardToTextFile(SBoard board, std::wstring filename)
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

		ss  << GetBoardCellDisplayCharacter(board_line[0])
			<< GetBoardCellDisplayCharacter(board_line[1])
			<< GetBoardCellDisplayCharacter(board_line[2]);

		ss << L"|"
			<< GetBoardCellDisplayCharacter(board_line[3])
			<< GetBoardCellDisplayCharacter(board_line[4])
			<< GetBoardCellDisplayCharacter(board_line[5]);

		ss << L"|"
			<< GetBoardCellDisplayCharacter(board_line[6])
			<< GetBoardCellDisplayCharacter(board_line[7])
			<< GetBoardCellDisplayCharacter(board_line[8])
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
wchar_t GetBoardCellDisplayCharacter(SCell cell) 
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

/*************************************************
* Displayed if no parameters provided by the user.
**************************************************/
void PrintHelp()
{
	CConsoleIO console;
	console.PushColourAttributes();

	console.SetColourAttributes(FOREGROUND_WHITE);
	std::wcout << L"Sudoko Solver " << BUILD_VERSION << std::endl;
	std::wcout << L"Usage:" << std::endl;

	console.SetColourAttributes(FOREGROUND_LIGHTYELLOW);
	std::wcout << L"  SSolve.exe -c -s <filename.txt>" << std::endl << std::endl;

	console.SetColourAttributes(FOREGROUND_WHITE);
	std::wcout << L"where:" << std::endl;

	console.SetColourAttributes(FOREGROUND_LIGHTYELLOW);
	std::wcout << L"  -c: Create blank board layout to given file/screen" << std::endl;
	std::wcout << L"  -s: Solve using layout in either file or clipboard" << std::endl;
	std::wcout << L"      If no input file given, the clipboard data will be used" << std::endl;

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
*
* PARAMETERS:
*		source - filename of text file containing board data.
*		useClipboard - If true, then the contents of the clipboard is used.
*					   'source' is ignored.
*
* REMARKS:
*******************************************************************************/
bool LoadBoardState(std::wstring source, bool useClipboard /*= false*/)
{
	// load the input file into a vector array
	std::vector<std::wstring> linevec;
	linevec.reserve(16);

	if (useClipboard) {

		// ********************
		// USING CLIPBOARD DATA
		// ********************

		std::wstringstream ss;
		if (::IsClipboardFormatAvailable(CF_UNICODETEXT) && ::OpenClipboard(nullptr))
		{
			HGLOBAL hData = ::GetClipboardData(CF_UNICODETEXT);
			if (nullptr != hData)
			{
				auto buffer = (wchar_t*)GlobalLock(hData);
				ss << buffer;
				GlobalUnlock(hData);

				// clip data into lines
				std::wstring line;
				while (std::getline(ss, line)) {
					linevec.emplace_back(line);
				}
			}
			::CloseClipboard();
		}

	}
	else {

		// ******************
		// USING SOURCE FILE
		// ******************

		// Get the full path of the input file
		// If the full path is not supplied, then use the current folder location.
		std::filesystem::path inpath(source);
		if (!inpath.has_parent_path()) {
			std::filesystem::path file(source);
			auto finalpath = std::filesystem::current_path() / inpath.filename();
			inpath = finalpath;
		}

		std::wifstream in(inpath.c_str());
		if (!in.is_open())
			return false;

		std::wstring line;
		while (std::getline(in, line)) {
			linevec.emplace_back(line);
		}
		in.close();
	}


	// lets do some validity checks
	if (linevec.size() < 9) {
		std::wcerr << L"# not enough board lines" << std::endl;
		return false;
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

			// skip if we have enough
			if (col >= BOARD_SIZE)
				break;
		}

		if (col < BOARD_SIZE) {
			std::wcerr << L"# not enough columns in row " << row << std::endl;
			return false;
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

bool SolveBoardByRecursion(SBoard board, SBoard* pBoard /*= nullptr*/)
{
	FindByElimination(board);
	DisplayBoardToConsole(board);
	if (board.IsBoardSolved()) {

		// copy solved board 
		if (pBoard != nullptr) {
			*pBoard = board;
		}

		return true;
	}

	_iteration++;

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

						if (SolveBoardByRecursion(board, pBoard))
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

/*
*/
bool SolveBoardByElimination(SBoard & board)
{
	DisplayBoardToConsole(board);
	bool is_solved = false;

	while (!is_solved) {
		bool boardHasChanged = FindByElimination(board);
		is_solved = board.IsBoardSolved();

		// Give up if we are stuck.
		if (!is_solved && !boardHasChanged)
			break;

		DisplayBoardToConsole(board);
		_iteration++;
	}
	return is_solved;
}
