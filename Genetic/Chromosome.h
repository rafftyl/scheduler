#pragma once
#include <functional>
#include <memory>
#include <vector>

class Chromosome;
typedef std::function<std::vector<float>(const Chromosome&)> FitnessFunction;
class Chromosome
{
private:
	bool isFitnessComputed = false;
	std::vector<float> cachedFitness;
public:
	Chromosome();
	~Chromosome();

	void ResetFitnessComputedFlag();
	const std::vector<float>& GetOrComputeFitness(FitnessFunction fitnessFunction);
	virtual void Mutate() = 0;
	virtual std::unique_ptr<Chromosome> Crossover(Chromosome& other) = 0;
	virtual std::unique_ptr<Chromosome> Clone() = 0;
	virtual void Randomize() = 0;
};

