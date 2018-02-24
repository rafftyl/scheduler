#include "Population.h"
#include <iostream>

Population::Population(int size, int tournamentSize, Chromosome& templateChromosome, FitnessFunction fitnessFunction, float mutationProb, float crossoverProb) :
	tournamentSize(tournamentSize), fitnessFunction(fitnessFunction),
	mutationProbability(mutationProb), crossoverProbability(crossoverProb),
	jobScheduler(std::thread::hardware_concurrency()), parentIndices(size, 0), dominationRanks(size, 0)
{	
	for (int i = 0; i < size; ++i)
	{
		chromosomes.push_back(templateChromosome.Clone());
		chromosomes.back()->Randomize();
		newPopulation.push_back(nullptr);
	}	
	jobScheduler.Init();
}

Population::~Population()
{
}

void Population::Breed()
{
	ComputeDominationRanks();	
	int jobCount = jobScheduler.GetWorkerCount();
	size_t chromosomesPerJob = chromosomes.size() / jobCount;	
	size_t popSize = static_cast<int>(chromosomes.size());

	size_t parentsChosen = 0;
	for (int i = 0; i < jobCount; ++i)
	{		
		size_t startIndex = i * chromosomesPerJob;
		size_t chroms = (i == jobCount - 1) ? chromosomes.size() - parentsChosen : chromosomesPerJob;
		auto job = [this, chroms, startIndex, popSize]()
		{
			for (size_t counter = 0; counter < chroms; ++counter)
			{
				size_t winnerIndex = 0;
				int minDomRank = static_cast<int>(popSize);
				for (int t = 0; t < tournamentSize; ++t)
				{
					size_t competitorIndex = Random::Range<size_t>(0, popSize - 1);
					if (dominationRanks[competitorIndex] < minDomRank)
					{
						minDomRank = dominationRanks[competitorIndex];
						winnerIndex = competitorIndex;
					}
				}				
				parentIndices[startIndex + counter] = winnerIndex;
			}
		};
		jobScheduler.ScheduleJob(job);
		parentsChosen += chroms;
	}

	jobScheduler.WaitForCompletion();	
	
	size_t childrenGenerated = 0;
	for (int i = 0; i < jobCount; ++i)
	{
		size_t startIndex = i * chromosomesPerJob;
		size_t chroms = (i == jobCount - 1) ? chromosomes.size() - childrenGenerated : chromosomesPerJob;
		auto job = [this, chroms, startIndex, popSize]()
		{
			for (size_t counter = 0; counter < chroms; ++counter)
			{
				size_t parentIndex_1 = Random::Range<size_t>(0, chromosomes.size() - 1);
				size_t parentIndex_2 = Random::Range<size_t>(0, chromosomes.size() - 1);

				if (Random::Range(0.0f, 1.0f) < crossoverProbability)
				{
					newPopulation[startIndex + counter] = chromosomes[parentIndices[parentIndex_1]]->Crossover(*chromosomes[parentIndices[parentIndex_2]]);
					if (Random::Range(0.0f, 1.0f) < mutationProbability)
					{
						newPopulation[startIndex + counter]->Mutate();
					}
				}
				else
				{
					std::uniform_int_distribution<int> coin(0, 1);
					if (Random::Range(0, 1) == 0)
					{
						newPopulation[startIndex + counter] = chromosomes[parentIndices[parentIndex_1]]->Clone();
					}
					else
					{
						newPopulation[startIndex + counter] = chromosomes[parentIndices[parentIndex_2]]->Clone();
					}
				}
			}
		};
		jobScheduler.ScheduleJob(job);
		childrenGenerated += chroms;
	}
	jobScheduler.WaitForCompletion();

	for(size_t i = 0; i < popSize; ++i)
	{
		chromosomes[i] = std::move(newPopulation[i]);
	}
}

void Population::StopComputation()
{
	jobScheduler.Shutdown();
}

const Chromosome* Population::GetBestChromosome() const
{
	ComputeDominationRanks();
	size_t index;
	int minDom = static_cast<int>(chromosomes.size());
	for (size_t i = 0; i < dominationRanks.size(); ++i)
	{
		if (dominationRanks[i] < minDom)
		{
			minDom = dominationRanks[i];
			index = i;
		}
	}

	return chromosomes[index].get();
}

std::vector<const Chromosome*> Population::GetBestChromosomes() const
{
	ComputeDominationRanks();
	int minDom = static_cast<int>(chromosomes.size());
	for (size_t i = 0; i < dominationRanks.size(); ++i)
	{
		if (dominationRanks[i] < minDom)
		{
			minDom = dominationRanks[i];
		}
	}

	std::vector<const Chromosome*> result;
	for (size_t i = 0; i < dominationRanks.size(); ++i)
	{
		if (dominationRanks[i] == minDom)
		{
			result.push_back(chromosomes[i].get());
		}
	}

	return result;
}

void Population::ComputeDominationRanks() const
{
	size_t popSize = chromosomes.size();

	int jobCount = jobScheduler.GetWorkerCount();
	size_t chromosomesPerJob = chromosomes.size() / jobCount;

	size_t fitnessComputed = 0;
	for (int i = 0; i < jobCount; ++i)
	{
		size_t startIndex = i * chromosomesPerJob;
		size_t chroms = (i == jobCount - 1) ? popSize - fitnessComputed : chromosomesPerJob;
		auto job = [this, chroms, startIndex]()
		{
			for (size_t counter = 0; counter < chroms; ++counter)
			{
				chromosomes[startIndex + counter]->GetOrComputeFitness(fitnessFunction);
			}
		};
		jobScheduler.ScheduleJob(job);
		fitnessComputed += chroms;
	}
	jobScheduler.WaitForCompletion();

	for (auto& val : dominationRanks)
	{
		val = 0;
	}

	size_t dominationRanksEvaluated = 0;
	for (int i = 0; i < jobCount; ++i)
	{
		size_t startIndex = i * chromosomesPerJob;
		size_t chroms = (i == jobCount - 1) ? popSize - dominationRanksEvaluated : chromosomesPerJob;
		auto job = [this, chroms, startIndex, &popSize]()
		{
			for (size_t counter = 0; counter < chroms; ++counter)
			{
				size_t chromosomeIndex_1 = startIndex + counter;
				for (size_t chromosomeIndex_2 = 0; chromosomeIndex_2 < popSize; ++chromosomeIndex_2)
				{
					if (chromosomeIndex_1 != chromosomeIndex_2)
					{
						auto fitness_1 = chromosomes[chromosomeIndex_1]->GetOrComputeFitness(fitnessFunction);
						auto fitness_2 = chromosomes[chromosomeIndex_2]->GetOrComputeFitness(fitnessFunction);
						bool isFirstBetter = false;
						for (size_t fitInd = 0; fitInd < fitness_1.size() && !isFirstBetter; ++fitInd)
						{
							if (fitness_1[fitInd] > fitness_2[fitInd])
							{
								isFirstBetter = true;
							}
						}
						if (!isFirstBetter)
						{
							++dominationRanks[chromosomeIndex_1];
						}
					}
				}
			}
		};
		jobScheduler.ScheduleJob(job);
		dominationRanksEvaluated += chroms;
	}
	jobScheduler.WaitForCompletion();
}
