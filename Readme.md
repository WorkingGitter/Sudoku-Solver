# Sudoku Solver

A simple, command line app that solves given Sudoku puzzles.
Written in C++ using Visual Studio 2017.

[![Build status](https://dev.azure.com/RichardChin/GitHub%20Projects/_apis/build/status/GitHub-Sudoku-CI)](https://dev.azure.com/RichardChin/GitHub%20Projects/_build/latest?definitionId=6)

## Instructions
The state of a given board is submitted to the application via a text file.
A template of the text file can be generated from the application.


1. Generate a blank template input file using the following command:
```
SSolve.exe -c[reate] <sudoku_template.txt>
```

The filename is optional. If none is provided, then one with the default filename will be created.

2. Fill in the initial board state.
Fill in the blank cell spaces with the available numbers.
Please ensure that the layout is maintained.

**Here is a sample**
```
 +---+---+---+
 |79 |   |3  |
 |   |  6|9  |
 |8  | 3 | 76|
 +---+---+---+
 |   |  5|  2|
 |  5|418|7  |
 |4  |7  |   |
 +---+---+---+
 |61 | 9 |  8|
 |  2|3  |   |
 |  9|   | 54|
 +---+---+---+
 ```

3. Save your changes.

4. Let the application solve the puzzle by supplying it with the edited file.
Use the following command
```
SSolve.exe -s[olve] "sudoku_template.txt"
```

The application will now display the board whilst solving.

## Remarks
The format of the input file can be more elaborate, as shown here:

```
+---+---+---+
|2 4|1..| ..|
| ..|5.3|6.7|
|...|9 .|4 .|
+---+---+---+
|9. |4..|.1.|
|65.|.1.|.74|
|.2.|..8| .9|
+---+---+---+
|. 9|. 5|...|
|5.2|3.1|.  |
|.. |..4|1 2|
+---+---+---+
```

Or it can be more compact, as follows:

```
.35.6.4..
.685....1
..4.1.9..
.9.....3.
.5....7.8
....3....
...1.6..9
.8.....76
.7.8..1..
```
The solver will ignore blank lines or separators (those that being with '-' or '+').
