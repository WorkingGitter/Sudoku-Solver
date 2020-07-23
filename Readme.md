# Sudoku Solver

## About
A simple, command line app that solves given Sudoku puzzles.

## Instructions
The state of a given board is submitted to the application via a text file.
A template of the text file can be generated from the application.


1. Generate a blank template input file using the following command:
''SSolve.exe -c[reate] <sudoku_template.txt>

The filename is optional. If none is provided, then one with the default filename will be created.

2. Fill in the initial board state.
Fill in the blank cell spaces with the available numbers.
Please ensure that the layout is maintained.

**Here is a sample**
>>+---+---+---+
>>|79 |   |3  |
>>|   |  6|9  |
>>|8  | 3 | 76|
>>+---+---+---+
>>|   |  5|  2|
>>|  5|418|7  |
>>|4  |7  |   |
>>+---+---+---+
>>|61 | 9 |  8|
>>|  2|3  |   |
>>|  9|   | 54|
>>+---+---+---+

3. Save your changes.

4. Let the application solve the puzzle by supplying it with the edited file.
Use the following command

''SSolve.exe -s[olve] "sudoku_template.txt"


The application should now display the board in its solved state.