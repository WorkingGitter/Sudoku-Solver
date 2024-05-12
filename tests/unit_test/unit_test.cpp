#include "pch.h"
#include "CppUnitTest.h"
#include "../../sudoku_solver/SudokuSolver.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <format>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{

	TEST_CLASS(SBoardTests)
	{
	public:
        TEST_METHOD(TestLoadBoardFromStringLayout)
        {
            SBoard board;
            SudokuSolver solver;
            std::wstring layout = L"123456789456789123789123456234567891567891234891234567345678912678912345912345678";
            Assert::IsTrue(solver.LoadBoardFromStringLayout(board, layout));
        }

        TEST_METHOD(TestGetBoardAsStringLayout)
        {
            SBoard board;
            SudokuSolver solver;
            std::wstring layout1 = L"123456789456789123789123456234567891567891234891234567345678912678912345912345678";
            std::wstring layout2 = L"987654321654321987321987654765432198198765432432198765576849213849213576213576849";
            std::wstring layout3 = L"534678912672195348198342567859761423426853791713924856961537284287419635345286179";
            
            solver.LoadBoardFromStringLayout(board, layout1);
            Assert::AreEqual(layout1, solver.GetBoardAsStringLayout(board));

            solver.LoadBoardFromStringLayout(board, layout2);
            Assert::AreEqual(layout2, solver.GetBoardAsStringLayout(board));

            solver.LoadBoardFromStringLayout(board, layout3);
            Assert::AreEqual(layout3, solver.GetBoardAsStringLayout(board));
        }

        TEST_METHOD(TestIntermediateSolution)
        {
			int steps = 0;
            SBoard board;
			SBoard solved_board;
            SudokuSolver solver;

            std::wstring unsolved_layout = L"004000900930210500650007023000000010075800040400006007080701090009640052200003100";
            std::wstring solved_layout = L"724365981938214576651987423863479215175832649492156837586721394319648752247593168";

            solver.LoadBoardFromStringLayout(board, unsolved_layout);
			Assert::IsTrue(solver.Solve(board, &solved_board, steps));

			auto solved_board_local = solver.GetBoardAsStringLayout(solved_board);

			// compare the solved board with the expected board
			Assert::AreEqual(solved_layout, solved_board_local);

        }

        void TestSolutionFromFile(const std::wstring& filename)
        {
            int steps = 0;
            SBoard board;
            SBoard board_solved;
            SudokuSolver solver;

            std::wifstream file(filename);
            Assert::IsTrue(file.is_open());

            std::wstring line;

            // Skip the header line
            std::getline(file, line);

            int line_pos = 1;
            while (std::getline(file, line))
            {
                line_pos++;
                std::wstringstream ss(line);
                std::wstring unsolvedBoard, solvedBoard;

                // Get the fields from the line
                std::getline(ss, unsolvedBoard, L',');
                std::getline(ss, solvedBoard, L',');

                // Normalise the board cell characters
                std::replace(unsolvedBoard.begin(), unsolvedBoard.end(), L'.', L'0');

                board.ClearBoard();
                board_solved.ClearBoard();

                // Load the unsolved board
                Assert::IsTrue(solver.LoadBoardFromStringLayout(board, unsolvedBoard));
                Assert::AreEqual(unsolvedBoard, solver.GetBoardAsStringLayout(board));

                // Solve the board
                Assert::IsTrue(solver.Solve(board, &board_solved, steps));

                auto solved_board_local = solver.GetBoardAsStringLayout(board_solved);

                // compare the solved board with the expected board
                Assert::AreEqual(solvedBoard, solved_board_local, 
                                std::format(L"File: {} on line {}", filename, line_pos).c_str());
            }
        }
        TEST_METHOD(TestSolutionFromSampleFiles)
        {
            TestSolutionFromFile(L"C:/dev/Sudoku-Solver/tests/data/sudoku_tests_small.csv");
            TestSolutionFromFile(LR"(C:\dev\Sudoku-Solver\tests\data\sudoku_tests_intermediate.csv)");
            TestSolutionFromFile(LR"(C:\dev\Sudoku-Solver\tests\data\sudoku_tests_expert.csv)");
        }
	};
}
