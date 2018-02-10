// Scheduler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Genetic/Population.h"
#include <iostream>
#include <sstream>

class TestChromosome : public Chromosome
{	
public:
	static int mutationCount;
	static int crossoverCount;
	std::vector<float> data;

	TestChromosome() : Chromosome()
	{

	}

	TestChromosome(const TestChromosome& other) = default;

	virtual void Mutate() override
	{
		mutationCount++;
		for (float& val : data)
		{
			val += static_cast<float>(rand()) / RAND_MAX;
		}
	}

	virtual std::unique_ptr<Chromosome> Crossover(const Chromosome& other) const override
	{
		crossoverCount++;
		auto& typedOther = static_cast<const TestChromosome&>(other);
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> dist(0, 1);
		auto result = std::make_unique<TestChromosome>();
		result->data.resize(15);
		for (size_t i = 0; i < data.size(); ++i)
		{
			if (dist(rd) == 0)
			{
				result->data[i] = data[i];
			}
			else
			{
				result->data[i] = typedOther.data[i];
			}
		}
		return result;
	}

	virtual std::unique_ptr<Chromosome> Clone() const override
	{
		return std::make_unique<TestChromosome>(*this);
	}

	virtual void Randomize() override
	{
		data.resize(15);
		for (float& val : data)
		{
			val = static_cast<float>(rand()) / RAND_MAX;
		}
	}

	virtual std::string ToString() const override
	{
		std::stringstream ss;
		for (float val : data)
		{
			ss << val << ", ";
		}
		return ss.str();
	}

	bool operator==(const TestChromosome& other) const
	{
		return data == other.data;
	}
};

int TestChromosome::mutationCount;
int TestChromosome::crossoverCount;

int main()
{
	FitnessFunction fitness =
		[&](const Chromosome& chrom)
	{
		auto& typedChrom = static_cast<const TestChromosome&>(chrom);
		float sum = 0;
		for (float val : typedChrom.data)
		{
			sum += val;
		}
		return std::vector<float>{sum};
	};
	TestChromosome::mutationCount = 0;
	TestChromosome::crossoverCount = 0;

	Population pop(100, 7, TestChromosome(), fitness);
	auto best = pop.GetBestChromosome();
	auto maxFitness_1 = best->GetOrComputeFitness(fitness);
	std::cout << best->ToString() << "\t" << maxFitness_1[0] << std::endl << std::endl;
	for (int i = 0; i < 200; ++i)
	{
		pop.Breed();
	}
	best = pop.GetBestChromosome();
	auto maxFitness_2 = best->GetOrComputeFitness(fitness);
	std::cout << best->ToString() << "\t" << maxFitness_2[0] << std::endl << std::endl;
	std::cout << TestChromosome::mutationCount << " " << TestChromosome::crossoverCount;
	getchar();
}

