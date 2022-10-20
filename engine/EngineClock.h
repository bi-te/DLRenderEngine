#pragma once

#include <cassert>
#include <chrono>

using float_duration = std::chrono::duration<float>;

class EngineClock
{
	std::chrono::steady_clock::time_point previous, current;
	std::chrono::steady_clock::time_point engineTime;

	static EngineClock* s_clock;
	EngineClock(){
		previous = current = std::chrono::steady_clock::now();
		engineTime = std::chrono::steady_clock::time_point{};
	}

	EngineClock(const EngineClock& other) = delete;
	EngineClock(EngineClock&& other) noexcept = delete;
	EngineClock& operator=(const EngineClock& other) = delete;
	EngineClock& operator=(EngineClock&& other) noexcept = delete;

public:
	void tick()
	{
		previous = current;
		current = std::chrono::steady_clock::now();
		engineTime += (current - previous);
	}

	void skip() {
		previous = current = std::chrono::steady_clock::now();
	}

	std::chrono::steady_clock::time_point now() {
		return engineTime;
	}

	float nowf() {
		return float_duration(engineTime.time_since_epoch()).count();
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