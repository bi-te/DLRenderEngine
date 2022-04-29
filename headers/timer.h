#pragma once
#include <chrono>

using uint = unsigned int;

class timer
{
	std::chrono::steady_clock::time_point previous, current;
	double dt, frame_time;

public:
	timer(double frame_time): frame_time(frame_time)
	{
	}

	void start()
	{
		previous = std::chrono::steady_clock::now();
		dt = 0;
	}

	double get_dt() const
	{
		return dt;
	}

	double get_frame_time() const
	{
		return frame_time;
	}

	bool frame_time_check()
	{
		current = std::chrono::steady_clock::now();
		dt += std::chrono::duration<double>(current - previous).count();
		previous = current;
		if(dt > frame_time)
		{
			dt -= frame_time;
			return true;
		}
		return false;
	}

	double time_passed()
	{
		current = std::chrono::steady_clock::now();
		return std::chrono::duration<double>(current - previous).count();;
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