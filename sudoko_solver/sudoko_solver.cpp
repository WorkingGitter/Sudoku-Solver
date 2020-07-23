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
#include "ConsoleIO.h"
#include "SBoard.h"
#include "s_timer.h"

#define BUILD_VERSION L"0.2"

SBoard sboard;

void PrintHelp();
void PrintBoardToConsole();
wchar_t GetDisplayChar(SCellStruct);
void WriteBoardToFile(SBoard board, std::wstring filename);

SCellStruct GetBoardCellFrom(wchar_t c);
bool LoadBoardState(std::wstring);
bool SolveBoard();

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
		
		if (!LoadBoardState(filename)) {
			std::wcerr << L"# Failed to load board settings" << std::endl;
			return 1;
		}

		bool status = SolveBoard();
		PrintBoardToConsole();

		if (!status) {
			std::wcerr << L"# Failed to solve given board" << std::endl;
		}
		else {
			std::wcout << L"Board has been solved" << std::endl;
		}

		
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
void PrintBoardToConsole()
{
	std::wstring dividerline = L"+---+---+---+";	

	CConsoleIO console;
	console.ClearScreen();

	for (int line = 0; line < BOARD_SIZE; line++) {

		std::wstringstream ss;
		auto board_line = sboard.GetRow(line);

		if ((line % 3) == 0) {
			ss << dividerline.c_str() << std::endl;
		}

		ss << L"|"
			<< GetDisplayChar(board_line[0])
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
			<< L"|" << std::endl;

		std::wcout << ss.str();
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
	std::wstring dividerline = L"+---+---+---+";

	for (int line = 0; line < BOARD_SIZE; line++) {

		std::wstringstream ss;
		auto board_line = board.GetRow(line);

		if ((line % 3) == 0) {
			ss << dividerline.c_str() << std::endl;
		}

		ss << L"|"
			<< GetDisplayChar(board_line[0])
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
			<< L"|" << std::endl;

		outfile << ss.str().c_str();
	}

	outfile << dividerline.c_str();
	outfile << L"\n";
	outfile.close();
}

/*
* Returns the character representation of the given 
* board cell.
*/
wchar_t GetDisplayChar(SCellStruct cell) 
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
	case SValueEnum::SValue_Empty: c = L' ';
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

/*
* Initialises our global board with the values provided in the input file.
* This file must be in a fixed format. Users can generate a blank board using the 
* '-c' parameter. You can then use this template to set the cell values.
*/
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

	// The input file, and therefore the vector array will be of the form:
	// +---+---+---+
	// |123|123|123|
	// |456| 56|4 6|
	// |789|7  | 89|
	// +---+---+---+
	// |123|123|123|
	// |456|456|456|
	// |789|789|789|
	// +---+---+---+
	// |123|123|123|
	// |4X6|456|456|
	// |789|789|789|
	// +---+---+---+
	//
	// This needs to be parsed and loaded to the board.

	// lets do some validity checks
	if (linevec.size() < 13) {
		std::wcerr << L"# not enough board lines" << std::endl;
		return false;
	}

	int currow = 0;
	for (auto l : linevec) {
		if (l.size() < 13) {
			std::wcerr << L"# not enough board columns on row " << (currow + 1) << std::endl;
		}
		currow++;
	}

	// remove borders 
	std::vector<std::wstring> newlinevec;
	newlinevec.push_back(linevec[1]);
	newlinevec.push_back(linevec[2]);
	newlinevec.push_back(linevec[3]);
	newlinevec.push_back(linevec[5]);
	newlinevec.push_back(linevec[6]);
	newlinevec.push_back(linevec[7]);
	newlinevec.push_back(linevec[9]);
	newlinevec.push_back(linevec[10]);
	newlinevec.push_back(linevec[11]);
	assert(newlinevec.size() == BOARD_SIZE);

	for (auto row = 0; row < BOARD_SIZE; row++) {
		auto line = newlinevec[row];
		sboard.SetCell(0, row, GetBoardCellFrom(line[1]));
		sboard.SetCell(1, row, GetBoardCellFrom(line[2]));
		sboard.SetCell(2, row, GetBoardCellFrom(line[3]));
		sboard.SetCell(3, row, GetBoardCellFrom(line[5]));
		sboard.SetCell(4, row, GetBoardCellFrom(line[6]));
		sboard.SetCell(5, row, GetBoardCellFrom(line[7]));
		sboard.SetCell(6, row, GetBoardCellFrom(line[9]));
		sboard.SetCell(7, row, GetBoardCellFrom(line[10]));
		sboard.SetCell(8, row, GetBoardCellFrom(line[11]));
	}

	return true;
}

SCellStruct GetBoardCellFrom(wchar_t c)
{
	SCellStruct cell{ SValueEnum::SValue_Empty, SStateEnum::SState_Fixed };

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
*/
bool SolveBoard()
{
	bool IsRowSolved = true;
	bool IsColSolved = true;
	bool IsBlockSolved = true;

	for (auto col = 0; col < BOARD_SIZE; col++) {
		auto col_vec = sboard.GetCol(col);
		IsColSolved &= sboard.IsSolved(col_vec);
	}

	for (auto row = 0; row < BOARD_SIZE; row++) {
		auto row_vec = sboard.GetCol(row);
		IsRowSolved &= sboard.IsSolved(row_vec);
	}

	for (auto b : { 0,1,2,3,4,5,6,7,8 }) {
		auto block_vec = sboard.GetCol(b);
		IsBlockSolved &= sboard.IsSolved(block_vec);
	}

	return IsRowSolved && IsColSolved && IsBlockSolved;
}
