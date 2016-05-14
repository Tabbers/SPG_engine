#pragma once
#include <random>

namespace Randomizer
{
	template<class T>
	static T GetRandom(T min, T max)
	{
		static std::random_device rd;
		static std::mt19937 randomGenerator = std::mt19937(rd());

		std::uniform_int_distribution<T> dist(min, max);
		return dist(randomGenerator);
	}

	static float GetRandom(float min, float max)
	{
		static std::random_device rd;
		static std::mt19937 randomGenerator = std::mt19937(rd());

		std::uniform_real_distribution<float> dist(min, max);
		return dist(randomGenerator);
	}
	static float GetRandom(double min, double max)
	{
		static std::random_device rd;
		static std::mt19937 randomGenerator = std::mt19937(rd());

		std::uniform_real_distribution<double> dist(min, max);
		return dist(randomGenerator);
	}
}