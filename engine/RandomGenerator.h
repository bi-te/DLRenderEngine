#pragma once
#include<random>
#include<chrono>
#include <memory>

class RandomGenerator
{
	std::default_random_engine random;
	std::uniform_real_distribution<float> distribution;

	RandomGenerator():
		random((std::chrono::system_clock::now().time_since_epoch().count())),
		distribution(0.f, std::nextafter(1.f, 2.f))
	{
	}

public:
	static RandomGenerator& generator()
	{
		static std::unique_ptr<RandomGenerator> random_generator(new RandomGenerator);
		return *random_generator;
	}

	float get_real() { return distribution(random); }
};
