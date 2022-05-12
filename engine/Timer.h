#pragma once
#include <chrono>

using uint = unsigned int;

class Timer
{
	std::chrono::steady_clock::time_point previous, current;
	float dt, frame_time;

public:
	Timer(float frame_time): frame_time(frame_time)
	{
	}

	void start()
	{
		previous = std::chrono::steady_clock::now();
		dt = 0;
	}

	float get_dt() const
	{
		return dt;
	}

	float get_frame_time() const
	{
		return frame_time;
	}

	bool frame_time_check()
	{
		current = std::chrono::steady_clock::now();
		dt += std::chrono::duration<float>(current - previous).count();
		previous = current;
		if(dt > frame_time)
		{
			return true;
		}
		return false;
	}

	float time_passed()
	{
		current = std::chrono::steady_clock::now();
		return std::chrono::duration<float>(current - previous).count();;
	}

	void restart()
	{
		previous = std::chrono::steady_clock::now();
		dt = 0;
	}

	void stop()
	{
		previous = std::chrono::steady_clock::time_point{};
		dt = 0;
	}
};