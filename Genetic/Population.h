#pragma once
#include "Chromosome.h"
#include <vector>
#include "Rand.h"
#include <functional>
#include <mutex>
#include "JobSystem.h"

class Population
{
private:
	std::vector<std::unique_ptr<Chromosome>> chromosomes;
	std::vector<std::unique_ptr<Chromosome>> newPopulation;
	std::vector<size_t> parentIndices;
	mutable std::vector<int> dominationRanks;
	FitnessFunction fitnessFunction;
	int tournamentSize;
	float mutationProbability;
	float crossoverProbability;

	mutable JobScheduler jobScheduler;
public:
	Population(int size, int tournamentSize, Chromosome& templateChromosome, FitnessFunction fitnessFunction, float mutationProb = 0.03f, float crossoverProb = 0.8f);
	Population(const Population& other) = delete;
	~Population();

	void Breed();
	void StopComputation();
	const Chromosome* GetBestChromosome() const;
	std::vector<const Chromosome*> GetBestChromosomes() const;
private:
	void ComputeDominationRanks() const;
};

