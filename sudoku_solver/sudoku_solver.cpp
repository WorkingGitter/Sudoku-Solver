/* ======================================
* Author : Richard Chin
* Date   : July 2020
* =======================================*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <cassert>
#include <map>
#include <set>
#include "ConsoleIO.h"
#include "SudokuSolver.h"
#include "s_timer.h"

#define BUILD_VERSION L"Alpha 0.1.0"

#define CELL_COLOUR_FIXED FOREGROUND_WHITE
#define CELL_COLOUR_SOLVED FOREGROUND_LIGHTYELLOW
#define CELL_COLOUR_ATTEMPT FOREGROUND_LIGHTAQUA

// global puzzle board
SBoard sboard;
SudokuSolver solver;

// iteration or moves in solve
int _iteration = 0;

void    PrintHelp();
void    DisplayBoardToConsole(SBoard &, int indent = 0);
void    WriteBoardToTextFile(SBoard board, std::wstring filename);
bool    LoadBoardState(std::wstring source, bool useClipboard = false);


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
	bool action_generate         = false;			// generate a new puzzle
	bool action_create           = false;			// generate blank template layout
	bool action_solve            = false;			// solve given board (using file or clipboard)
	bool action_useclipboarddata = false;			// use data in clipboard as source
	std::wstring param_create    = { L"-c" };		//
	std::wstring param_solve     = { L"-s" };		//
	std::wstring param_gen       = { L"-g" };
	std::wstring filename        = { L"" };			//

	// Loop through all our parameters and set our options variables
	// NB: The first parameter will be the application filename, so we want to
	//     skip this.
	for (int n = 1; n < argc; n++) {

		bool create = (param_create.compare(argv[n]) == 0);
		bool solve  = (param_solve.compare(argv[n]) == 0);
		bool gen = (param_gen.compare(argv[n]) == 0);
		action_create   |= create;
		action_solve    |= solve;
		action_generate |= gen;

		if (!create && !solve && !gen) {
			filename = argv[n];
		}
	}

	// If no filename supplied, use a default.
	if (filename.empty()) {
		filename = L"puzzleboard.txt";
		action_useclipboarddata = true;
	}
	else {
		// if we supplied a filename, but no action options, then just assume we
		// want to use that file for a solve.
		if (!action_create && !action_solve && !action_generate) {
			action_solve = true;
		}
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

	// New Puzzle
	if (action_generate) {

		CConsoleIO console;
		console.ClearScreen();
		std::wcout << L"Generating..." << std::endl;

		// Start with a solved board.
		// It will be initially blank, so no need to use the elimitation process
		// first.
		timer t;
		SBoard board;

		t.start();
		bool has_solved = solver.SolveBoardByRecursion(SBoard {}, &board, _iteration);
		assert(has_solved); if (!has_solved) return 1;

		//
		bool has_finished = solver.ReverseSolve(board);

		// Display to screen
		t.stop();
		DisplayBoardToConsole(board);
		std::wcout << L"Completed in " << t.get_elapsedtime_sec() << L" secs" << std::endl;
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

		bool has_solved = solver.SolveBoardByElimination(sboard, _iteration);

		if (has_solved) {
			solved_board = sboard;
		}
		else {
			has_solved = solver.SolveBoardByRecursion(sboard, &solved_board, _iteration);
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
			auto backcolour = (tc & 0xF0);
			console.PushColourAttributes();
			console.SetColourAttributes(CELL_COLOUR_FIXED | backcolour);
			std::wcout << L"ллл";
			console.SetColourAttributes(tc);
			std::wcout << L" : Fixed Numbers" << std::endl;

			console.SetColourAttributes(CELL_COLOUR_SOLVED | backcolour);
			std::wcout << L"ллл";
			console.SetColourAttributes(tc);
			std::wcout << L" : Solved Numbers" << std::endl;

			console.SetColourAttributes(CELL_COLOUR_ATTEMPT | backcolour);
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

	auto back_colour = console.GetColourAttributes() & 0xF0;
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
				console.SetColourAttributes(CELL_COLOUR_FIXED | back_colour);
				break;
			case SStateEnum::SState_Solved:
				console.SetColourAttributes(CELL_COLOUR_SOLVED | back_colour);
				break;
			case SStateEnum::SState_New:
				console.SetColourAttributes(CELL_COLOUR_ATTEMPT | back_colour);
				break;
			default: 
				console.SetColourAttributes(CELL_COLOUR_FIXED | back_colour);
				break;				
			}
			std::wcout << SBoard::CellToCharacter(board_line[ncol]);
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

		ss  << SBoard::CellToCharacter(board_line[0])
			<< SBoard::CellToCharacter(board_line[1])
			<< SBoard::CellToCharacter(board_line[2]);

		ss << L"|"
			<< SBoard::CellToCharacter(board_line[3])
			<< SBoard::CellToCharacter(board_line[4])
			<< SBoard::CellToCharacter(board_line[5]);

		ss << L"|"
			<< SBoard::CellToCharacter(board_line[6])
			<< SBoard::CellToCharacter(board_line[7])
			<< SBoard::CellToCharacter(board_line[8])
			<< std::endl;
	}
	ss << std::endl;
	outfile << ss.str();
	outfile.close();
}


/*************************************************
* Displayed if no parameters provided by the user.
**************************************************/
void PrintHelp()
{
	CConsoleIO console;

	// get the topmost 4 bits 
	auto back_colour = console.GetColourAttributes() & 0xF0;

	console.PushColourAttributes();

	console.SetColourAttributes(FOREGROUND_WHITE | back_colour);
	std::wcout << L"Sudoko Solver " << BUILD_VERSION << std::endl;
	std::wcout << L"Usage:" << std::endl;

	console.SetColourAttributes(FOREGROUND_LIGHTYELLOW | back_colour);
	std::wcout << L"  SSolve.exe -g -c -s <filename.txt>" << std::endl << std::endl;

	console.SetColourAttributes(FOREGROUND_WHITE | back_colour);
	std::wcout << L"where:" << std::endl;

	console.SetColourAttributes(FOREGROUND_LIGHTYELLOW | back_colour);
	std::wcout << L"  -g: Generate a fully valid puzzle" << std::endl;
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

			sboard.SetCell(col, row, SBoard::CharacterToCell(c));
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
