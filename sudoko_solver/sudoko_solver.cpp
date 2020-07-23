/* ======================================
* Author : Richard Chin
* Date   : July 2020
* =======================================*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "ConsoleIO.h"
#include "SBoard.h"
//#include "textfile.h"	// utf-8 text file support
#include "s_timer.h"

#define BUILD_VERSION L"0.2"

SBoard sboard;

void PrintHelp();
void PrintBoardToConsole();
wchar_t GetDisplayChar(SCellStruct);
void WriteBoardToFile(SBoard board, std::wstring filename);

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
		// TODO::solve

		PrintBoardToConsole();
	}

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