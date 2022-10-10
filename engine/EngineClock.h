#pragma once

#include <cassert>
#include <chrono>

using floatSeconds = std::chrono::duration<float>;
using std::chrono::steady_clock;

class EngineClock
{
	steady_clock::time_point startFrameTime;

	static EngineClock* s_clock;
	EngineClock(){}

	EngineClock(const EngineClock& other) = delete;
	EngineClock(EngineClock&& other) noexcept = delete;
	EngineClock& operator=(const EngineClock& other) = delete;
	EngineClock& operator=(EngineClock&& other) noexcept = delete;

public:

	void startFrame()
	{
		startFrameTime = steady_clock::now();
	}

	float frameStartTime()
	{
		return floatSeconds(startFrameTime.time_since_epoch()).count();
	}

	static float now()
	{
		return floatSeconds(steady_clock::now().time_since_epoch()).count();
	}

	static void init()
	{
		if (s_clock) s_clock->reset();
		s_clock = new EngineClock;
	}

	static EngineClock& instance()
	{
		assert(s_clock && "EngineClock not initialized");
		return *s_clock;
	}

	static void reset()
	{
		delete s_clock;
	}
};