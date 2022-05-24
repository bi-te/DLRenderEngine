#pragma once
#include <chrono>

using uint32_t = unsigned int;

class Timer
{
	std::chrono::steady_clock::time_point previous, current;
	float frame_time;

public:
	Timer(float frame_time): frame_time(frame_time)
	{
	}

	float get_frame_time() const { return frame_time; }
	float time_passed() const { return std::chrono::duration<float>(current - previous).count(); }

	void start() { previous = std::chrono::steady_clock::now(); }
	void advance_current() { previous = current; }

	bool frame_time_check()
	{
		current = std::chrono::steady_clock::now();
		return std::chrono::duration<float>(current - previous).count() > frame_time;
	}

};