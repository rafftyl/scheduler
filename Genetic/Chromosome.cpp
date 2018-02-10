#include "Chromosome.h"



Chromosome::Chromosome()
{
}


Chromosome::~Chromosome()
{
}

void Chromosome::ResetFitnessComputedFlag()
{
	isFitnessComputed = false;
}

const std::vector<float>& Chromosome::GetOrComputeFitness(FitnessFunction fitnessFunction) const
{
	if (!isFitnessComputed)
	{
		cachedFitness = fitnessFunction(*this);
		isFitnessComputed = true;
	}
	return cachedFitness;
}
