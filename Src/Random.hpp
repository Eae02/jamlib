#pragma once

#include <random>
#include <chrono>
#include <pcg_random.hpp>

namespace jm
{
	template <typename G = pcg32_fast>
	class Random
	{
	public:
		Random()
			: rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()) { }
		
		explicit Random(uint64_t seed)
			: rng(seed) { }
		
		float GenFloat(float min, float max)
		{
			return std::uniform_real_distribution<float>(min, max)(rng);
		}
		
		double GenDouble(double min, double max)
		{
			return std::uniform_real_distribution<double>(min, max)(rng);
		}
		
		int GenInt(int min, int max)
		{
			return std::uniform_int_distribution<int>(min, max)(rng);
		}
		
		int64_t GenInt64(int64_t min, int64_t max)
		{
			return std::uniform_int_distribution<int64_t>(min, max)(rng);
		}
		
		G& Generator()
		{
			return rng;
		}
		
	private:
		G rng;
	};
}
