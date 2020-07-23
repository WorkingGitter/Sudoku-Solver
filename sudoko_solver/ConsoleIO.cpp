////////////////////////////////////////////////////////////////////////////
//                                                                        //
// This file may not be redistributed, unmodified or sold for profit      //
// without the company's written consent.                                 //
//                                                                        //
// Do expect bugs.                                                        //
// Please let me know of any bugs/mods/improvements.                      //
// and I will try to fix/incorporate them into this file.                 //
// Enjoy!                                                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
//
// CLASS        : CConsoleIO
// COPYRIGHT    : BMRB International 2003
// AUTHOR       : Richard Chin
// DATE         : July 2003.
// UPDATED      : July 2020
//				  
// DESCRIPTION  : Set of useful routines, to encapsulate the 'old' conio.h
//				  libraries of Turbo C++.
//
////////////////////////////////////////////////////////////////////////////

#include "ConsoleIO.h"
#include <cassert>

WORD CConsoleIO::m_wAttribute = FOREGROUND_WHITE | BACKGROUND_BLACK;

CConsoleIO::CConsoleIO()
{
	m_hConsole = GetStdHandle ( STD_OUTPUT_HANDLE );
	assert( m_hConsole != INVALID_HANDLE_VALUE );
}

CConsoleIO::~CConsoleIO()
{
}

/******************************************************************************
*				SetColourAttributes
*
*******************************************************************************/
void CConsoleIO::SetColourAttributes(WORD wAttributes)
{
	SetConsoleTextAttribute(m_hConsole, wAttributes);
}

/******************************************************************************
*				GetColourAttributes
*
*******************************************************************************/
WORD CConsoleIO::GetColourAttributes(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(m_hConsole, &csbi);
	return csbi.wAttributes;
}

/******************************************************************************
*				GetCursorPos
*
*******************************************************************************/
COORD CConsoleIO::GetCursorPos()
{
	COORD pos = {0,0};
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(m_hConsole, &csbi);
	pos = csbi.dwCursorPosition;
	return pos;
}

/******************************************************************************
*				SetCursorPos
*******************************************************************************/
void  CConsoleIO::SetCursorPos(COORD pos)
{
	SetConsoleCursorPosition (m_hConsole, pos);
}

/******************************************************************************
*				SetCursorPos
*
*******************************************************************************/
void  CConsoleIO::SetCursorPos(short x, short y)
{
	COORD pos = {x, y};
	SetCursorPos(pos);
}

void  CConsoleIO::SetCursorX(int x)
{
	COORD c = this->GetCursorPos();
	c.X = x;
	this->SetCursorPos(c);
}
void  CConsoleIO::SetCursorY(int y)
{
	COORD c = this->GetCursorPos();
	c.Y = y;
	this->SetCursorPos(c);
}


void  CConsoleIO::GotoStartOfLine()
{
	COORD pos = GetCursorPos();
	pos.X=0;
	SetCursorPos(pos);
}

/******************************************************************************
*				PushColourAttributes
*
*******************************************************************************/
void CConsoleIO::PushColourAttributes(void)
{
	CConsoleIO::m_wAttribute = GetColourAttributes();
}

/******************************************************************************
*				PopColourAttributes
*
*******************************************************************************/
void CConsoleIO::PopColourAttributes(void)
{
	SetColourAttributes(CConsoleIO::m_wAttribute);
}


/******************************************************************************
*				ClearScreen
*
*******************************************************************************/
void CConsoleIO::ClearScreen()
{
	WORD wAttribute;
    DWORD dwConSize;
    COORD Origin = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    
    GetConsoleScreenBufferInfo(m_hConsole, &csbi);

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;		// get the number of character cells in the current buffer
	wAttribute = csbi.wAttributes;					// and the current attributes

    /* fill the entire screen with blanks */ 
    FillConsoleOutputCharacter(m_hConsole,(TCHAR)' ', dwConSize, Origin, &cCharsWritten);

    // now set the buffer's attributes accordingly
	FillConsoleOutputAttribute(m_hConsole, wAttribute, dwConSize, Origin, &cCharsWritten);
	SetCursorPos(Origin);
}

void CConsoleIO::ClearScreen(int l, int t, int r, int b)
{
	WORD wAttribute;
	DWORD dwLength;
	DWORD dwCharWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(m_hConsole, &csbi);
	wAttribute = csbi.wAttributes;

	COORD coordPosition;
	coordPosition.X = l; 
	coordPosition.Y = t;

	dwLength = r - l;
	for(int line = t; line <= b; line++)
	{
		// Calculate the length of characters to clear
		coordPosition.Y = line;

		// Fill the rest of line with blanks. Also set current attributes.
		FillConsoleOutputCharacter(m_hConsole, (TCHAR)' ', dwLength, coordPosition, &dwCharWritten);
		FillConsoleOutputAttribute(m_hConsole, wAttribute, dwLength, coordPosition, &dwCharWritten );
	}

	SetCursorPos(l, t);
}

/******************************************************************************
*				ClearEndOfLine
*
*******************************************************************************/
void CConsoleIO::ClearEndOfLine()
{
	COORD coordPosition;
	DWORD dwLength;
	DWORD dwCharWritten;
	WORD wAttribute;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(m_hConsole, &csbi);

	wAttribute = csbi.wAttributes;
	coordPosition = csbi.dwCursorPosition;

	// Calculate the length of characters to the end of line
	dwLength = csbi.dwSize.X - coordPosition.X;

	// Fill the rest of line with blanks. Also set current attributes.
	FillConsoleOutputCharacter(m_hConsole, (TCHAR)' ', dwLength, coordPosition, &dwCharWritten);
    FillConsoleOutputAttribute(m_hConsole, wAttribute, dwLength, coordPosition, &dwCharWritten );

	// finally place the cursor back where it belongs
    SetConsoleCursorPosition(m_hConsole, coordPosition);
}
