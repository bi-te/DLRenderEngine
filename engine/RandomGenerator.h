#pragma once

#include <cassert>
#include<random>
#include<chrono>
#include <memory>

class RandomGenerator
{
	static RandomGenerator* s_random;

	std::default_random_engine random;
	std::uniform_real_distribution<float> distribution;

	RandomGenerator() :
		random((std::chrono::system_clock::now().time_since_epoch().count())),
		distribution(0.f, std::nextafter(1.f, 2.f))
	{
	}

	RandomGenerator(const RandomGenerator& other) = delete;
	RandomGenerator(RandomGenerator&& other) noexcept = delete;
	RandomGenerator& operator=(const RandomGenerator& other) = delete;
	RandomGenerator& operator=(RandomGenerator&& other) noexcept = delete;

public:
	static void init()
	{
		if (s_random) s_random->reset();
		s_random = new RandomGenerator;
	}

	static RandomGenerator& generator()
	{
		assert(s_random && "RandomGenerator not initialized");
		return *s_random;
	}

	static void reset()
	{
		delete s_random;
	}

	float get_real() { return distribution(random); }
};