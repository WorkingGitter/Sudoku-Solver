/*
* AUTHOR	: Richard Chin
* DATE		: July 2020
*/

#if !defined(__STIMER_H_27AA3EDA_0C3F_4446_B7A4_A746C6CD3157__)
#define __STIMER_H_27AA3EDA_0C3F_4446_B7A4_A746C6CD3157__

#pragma once
#include <cstdarg>
#include <chrono>
#include <string>

/***********************************************************************
* Small Timer Class
********************************************************************/
class timer
{
public:
	timer() {
	}
	timer(std::wstring title) : _title(title)
	{
	}

protected:
	std::wstring _title;
	long long _elapsed_ms;
	std::chrono::time_point<std::chrono::steady_clock> _start;
	std::chrono::time_point<std::chrono::steady_clock> _end;

public:
	void start() {
		_start = std::chrono::steady_clock().now();
	}
	void stop() {
		_end = std::chrono::steady_clock().now();
		_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
	}

	//CustomTrace(L"   : %0.1f secs\n", elapsed_ms / 1000.0);
	double get_elapsedtime_sec()	{
		return (double)(_elapsed_ms / 1000.0);
	}

	double get_elapsedtime_ms()	{
		return (double)_elapsed_ms;
	}
};
#endif // #if !defined(__STIMER_H_27AA3EDA_0C3F_4446_B7A4_A746C6CD3157__)
