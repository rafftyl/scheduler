#include "Population.h"

Population::Population()
{
}

Population::Population(int size, int tournamentSize, Chromosome& templateChromosome, FitnessFunction fitnessFunction, float mutationProb, float crossoverProb) :
	tournamentSize(tournamentSize), fitnessFunction(fitnessFunction), mutationProbability(mutationProb), crossoverProbability(crossoverProb)
{
	std::random_device rd;
	rng = std::mt19937(rd());
	uniformInt = std::uniform_int_distribution<size_t>(0, size - 1);
	uniformFloat = std::uniform_real_distribution<float>(0.0f, 1.0f);
	for (int i = 0; i < size; ++i)
	{
		chromosomes.push_back(templateChromosome.Clone());
		chromosomes.back()->Randomize();
	}
}

Population::~Population()
{
}

void Population::Breed()
{
	auto domRanks = ComputeDominationRanks();
	std::vector<std::unique_ptr<Chromosome>> newPopulation;
	newPopulation.reserve(chromosomes.size());
	std::vector<size_t> parentIndices;
	parentIndices.reserve(chromosomes.size());
	for (size_t i = 0; i < chromosomes.size(); ++i)
	{
		size_t winnerIndex = 0;
		int minDomRank = static_cast<int>(chromosomes.size());
		for (int t = 0; t < tournamentSize; ++t)
		{
			size_t competitorIndex = uniformInt(rng);
			if (domRanks[competitorIndex] < minDomRank)
			{
				minDomRank = domRanks[competitorIndex];
				winnerIndex = competitorIndex;
			}
		}
		parentIndices.push_back(winnerIndex);
	}

	for (size_t i = 0; i < chromosomes.size(); ++i)
	{
		size_t parentIndex_1 = uniformInt(rng);
		size_t parentIndex_2 = uniformInt(rng);
		float crossoverParam = uniformFloat(rng);
		if (crossoverParam < crossoverProbability)
		{
			newPopulation.push_back(chromosomes[parentIndices[parentIndex_1]]->Crossover(*chromosomes[parentIndices[parentIndex_2]]));
		}
		else
		{
			std::uniform_int_distribution<int> coin(0, 1);
			if (coin(rng) == 0)
			{
				newPopulation.push_back(chromosomes[parentIndices[parentIndex_1]]->Clone());
			}
			else
			{
				newPopulation.push_back(chromosomes[parentIndices[parentIndex_2]]->Clone());
			}
		}
	}

	chromosomes = newPopulation;
}

const Chromosome* Population::GetBestChromosome() const
{
	auto domRanks = ComputeDominationRanks();
	size_t index;
	int minDom = static_cast<int>(chromosomes.size());
	for (size_t i = 0; i < domRanks.size(); ++i)
	{
		if (domRanks[i] < minDom)
		{
			minDom = domRanks[i];
			index = i;
		}
	}

	return chromosomes[index].get();
}

std::vector<int> Population::ComputeDominationRanks() const
{	
	size_t popSize = chromosomes.size();
	std::vector<int> result(popSize, 0);
	for (size_t chromosomeIndex_1 = 0; chromosomeIndex_1 < popSize; ++chromosomeIndex_1)
	{
		for (size_t chromosomeIndex_2 = chromosomeIndex_1 + 1; chromosomeIndex_2 < popSize; ++chromosomeIndex_2)
		{
			auto fitness_1 = chromosomes[chromosomeIndex_1]->GetOrComputeFitness(fitnessFunction);
			auto fitness_2 = chromosomes[chromosomeIndex_2]->GetOrComputeFitness(fitnessFunction);
			bool isFirstBetter = false;
			bool isSecondBetter = false;
			for (size_t fitInd = 0; fitInd < fitness_1.size(); ++fitInd)
			{
				if (fitness_1[fitInd] > fitness_2[fitInd])
				{
					isFirstBetter = true;
				}
				else if (fitness_2[fitInd] > fitness_1[fitInd])
				{
					isSecondBetter = true;
				}
				if (isFirstBetter && isSecondBetter)
				{
					break;
				}
			}

			if (isFirstBetter && !isSecondBetter)
			{
				++result[chromosomeIndex_2];
			}
			else if (isSecondBetter && !isFirstBetter)
			{
				++result[chromosomeIndex_1];
			}
		}
	}
	return result;
}
