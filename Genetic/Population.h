#pragma once
#include "Chromosome.h"
#include <vector>
#include <random>

class Population
{
private:
	std::vector<std::unique_ptr<Chromosome>> chromosomes;
	FitnessFunction fitnessFunction;
	int tournamentSize;
	float mutationProbability;
	float crossoverProbability;

	std::mt19937 rng;
	std::uniform_int_distribution<size_t> uniformInt;
	std::uniform_real_distribution<float> uniformFloat;
public:
	Population();
	Population(int size, int tournamentSize, Chromosome& templateChromosome, FitnessFunction fitnessFunction, float mutationProb = 0.03f, float crossoverProb = 0.8f);
	~Population();

	void Breed();
	const Chromosome* GetBestChromosome() const;
private:
	std::vector<int> ComputeDominationRanks() const;
};

