#pragma once
#include <random>
#include <mutex>

class Random
{
private:
	static std::mutex mut;
	static std::mt19937 rng;
public:
	template<typename T>
	static T Range(T min, T max)
	{
		std::scoped_lock<std::mutex> lock(mut);
		std::uniform_int_distribution<T> dist(min, max);
		return dist(rng);
	}

	template<>
	static float Range<float>(float min, float max)
	{
		std::scoped_lock<std::mutex> lock(mut);
		std::uniform_real_distribution<float> dist(min, max);
		return dist(rng);
	}
};
