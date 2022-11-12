#pragma once

#include "EngineClock.h"

class Timer
{
protected:
	std::chrono::steady_clock::time_point previous, current;
	float check_time, last_check_time;

public:
	Timer(){}

	Timer(float frame_time): check_time(frame_time)
	{
	}

	void set_check_time(float frame_time){check_time = frame_time;}

	float get_check_time() const { return check_time; }
	float time_passed() const { return float_duration(current - previous).count(); }
	float get_last_check_time() const { return last_check_time; }

	void start() { previous = EngineClock::instance().now(); }
	void advance_current() {
		last_check_time = float_duration(current - previous).count();
		previous = current; 
	}

	void restart()
	{
		current = EngineClock::instance().now();
		previous = current;
	}

	bool frame_time_check()
	{
		current = EngineClock::instance().now();
		return float_duration(current - previous).count() > check_time;
	}

};