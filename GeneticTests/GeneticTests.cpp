#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Genetic/Population.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class TestChromosome : public Chromosome
{
public:
	std::vector<float> data;

	TestChromosome() : Chromosome(), data{0,1,0,1,0,1,0,1,0,1,0,1}
	{

	}

	TestChromosome(const TestChromosome& other) = default;

	virtual void Mutate() override
	{
		for (float& val : data)
		{
			val += static_cast<float>(rand()) / RAND_MAX;
		}
	}

	virtual std::unique_ptr<Chromosome> Crossover(const Chromosome& other) const override
	{
		auto& typedOther = static_cast<const TestChromosome&>(other);
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> dist(0, 1);
		auto result = std::make_unique<TestChromosome>();
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
		for (float& val : data)
		{
			val = static_cast<float>(rand()) / RAND_MAX;
		}
	}	

	bool operator==(const TestChromosome& other) const
	{
		return data == other.data;
	}
};

namespace GeneticTests
{		
	TEST_CLASS(ChromosomeTest)
	{
	public:
		TEST_METHOD(ChromosomeCloning)
		{
			TestChromosome chr;
			Assert::IsTrue(chr == *static_cast<TestChromosome*>(chr.Clone().get()));
		}

		TEST_METHOD(ChromosomeRandomization)
		{
			TestChromosome chr;
			TestChromosome chr_1;
			chr_1.Randomize();
			Assert::IsFalse(chr == chr_1);
		}

		TEST_METHOD(ChromosomeMutation)
		{
			TestChromosome chr;
			TestChromosome chr_1;
			chr_1.Mutate();
			Assert::IsFalse(chr == chr_1);
		}

		TEST_METHOD(ChromosomeCrossover)
		{
			TestChromosome chr;
			TestChromosome chr_1;
			chr_1.data = { 5,5,8,9,7,1,5,3,5,1,0,1 };
			auto child = chr.Crossover(chr_1);
			Assert::IsFalse(chr == *static_cast<TestChromosome*>(child.get()));
			Assert::IsFalse(chr_1 == *static_cast<TestChromosome*>(child.get()));
		}
	};

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

	TEST_CLASS(PopulationTest)
	{
	public:

		TEST_METHOD(Breeding)
		{
			Population pop(100, 7, TestChromosome(), fitness);
			auto maxFitness_1 = pop.GetBestChromosome()->GetOrComputeFitness(fitness);
			for (int i = 0; i < 200; ++i)
			{
				pop.Breed();
			}
			auto maxFitness_2 = pop.GetBestChromosome()->GetOrComputeFitness(fitness);
			Assert::IsTrue(maxFitness_2[0] > maxFitness_1[0]);
		}

	};
}