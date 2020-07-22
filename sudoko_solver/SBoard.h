#pragma once

// How many cells per side on our board
#define BOARD_SIZE 9

// The size of each sub-blocks
#define BLOCK_SIZE (BOARD_SIZE / 3)

/*******************************
* Integer value of a sudoko cell
********************************/
enum class SValueEnum {
	SValue_Empty = 0,
	SValue_1 = 1,
	SValue_2 = 2,
	SValue_3 = 3,
	SValue_4 = 4,
	SValue_5 = 5,
	SValue_6 = 6,
	SValue_7 = 7,
	SValue_8 = 8,
	SValue_9 = 9,
};

/*
*/
enum class SStateEnum {
	SState_Fixed,		// Cannot be changed.
	SState_Free			// Free to change
};

/*
* Each cell will have a 'state'.
* This includes any indicated value, but could also store if the current value
* was from the user, or have been calculated.
*/
struct SStateStruct {
	SValueEnum value;
	SStateEnum state;
};

/******************************************
* Encapsulates the sudoko board
*******************************************/
class SBoard
{
public:
protected:
};

