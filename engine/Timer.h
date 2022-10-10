#pragma once
#include <chrono>

using uint32_t = unsigned int;

class Timer
{
	std::chrono::steady_clock::time_point previous, current;
	float check_time;

public:
	Timer(float frame_time): check_time(frame_time)
	{
	}

	float get_check_time() const { return check_time; }
	float time_passed() const { return std::chrono::duration<float>(current - previous).count(); }

	void start() { previous = std::chrono::steady_clock::now(); }
	void advance_current() { previous = current; }

	bool frame_time_check()
	{
		current = std::chrono::steady_clock::now();
		return std::chrono::duration<float>(current - previous).count() > check_time;
	}

};