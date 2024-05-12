/*
*  Auther: Richard Chin
*  Date  : May 2024
*/

#pragma once

#include "SBoard.h"
#include <string>

/**
 * @class SudokuSolver
 * @brief Class for solving Sudoku puzzles
 */
class SudokuSolver
{
public:

    bool Solve(SBoard& board, SBoard* board_ptr, int& steps);

    /**
     * @brief Reverse solves a Sudoku puzzle by removing values from solved cells
     * @param board The Sudoku board to reverse solve
     * @return True if the reverse solve is successful, false otherwise
     */
    bool ReverseSolve(SBoard& board);

    /**
     * @brief Finds values for cells through a process of eliminating all other possibilities
     * @param board The Sudoku board to find values for
     * @return True if any addition to the board has been made, false otherwise
     */
    bool FindByElimination(SBoard& board);

    /**
     * @brief Solves a Sudoku board by elimination
     * @param board The Sudoku board to solve
     * @param steps The number of steps taken to solve the board
     * @return True if the board is solved, false otherwise
     */
    bool SolveBoardByElimination(SBoard& board, int& steps);

    /**
     * @brief Solves a Sudoku board by recursion
     * @param board The Sudoku board to solve
     * @param pBoard Pointer to the solved board (optional)
     * @param steps The number of steps taken to solve the board
     * @return True if the board is solved, false otherwise
     */
    bool SolveBoardByRecursion(SBoard board, SBoard* pBoard, int& steps);

    /*!	\brief Populates the board with the prepared set of cell states
    *
    *   The input layout string must be an array of the 9x9 grid cells.
    *   Therefore we are expecting 81 characters. Each charater must be of the
    *   following:
    *	'1' - '9'         : A value for the cell
    *   '.' or ' ' or '0' : An empty cell
    *
    *	\param board The board to populate represented as a list of cell characters.
    */
    bool LoadBoardFromStringLayout(SBoard& board, const std::wstring& layout);


    /**
     * @brief Converts the Sudoku board to a string layout
     * @param board The Sudoku board to convert
     * @return The string representation of the Sudoku board layout
     */
    std::wstring GetBoardAsStringLayout(const SBoard& board);
};
