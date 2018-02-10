#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <string>

class Chromosome;
typedef std::function<std::vector<float>(const Chromosome&)> FitnessFunction;
class Chromosome
{
private:
	mutable bool isFitnessComputed = false;
	mutable std::vector<float> cachedFitness;
public:
	Chromosome();
	virtual ~Chromosome();

	void ResetFitnessComputedFlag();
	const std::vector<float>& GetOrComputeFitness(FitnessFunction fitnessFunction) const;
	virtual void Mutate() = 0;
	virtual std::unique_ptr<Chromosome> Crossover(const Chromosome& other) const = 0;
	virtual std::unique_ptr<Chromosome> Clone() const = 0;
	virtual void Randomize() = 0;
	virtual std::string ToString() const { return ""; };
};

