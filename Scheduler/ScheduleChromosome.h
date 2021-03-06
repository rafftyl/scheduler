#pragma once
#include "../Genetic/Chromosome.h"
#include <vector>
#include "../Genetic/Rand.h"

class ScheduleChromosome : public Chromosome
{	
public:
	int dataSize;
	std::vector<bool> data;

	ScheduleChromosome(int dataSize);
	ScheduleChromosome(const ScheduleChromosome& other) = default;
	~ScheduleChromosome();

	virtual void Mutate() override;
	virtual std::unique_ptr<Chromosome> Crossover(const Chromosome& other) const override;
	virtual std::unique_ptr<Chromosome> Clone() const override;
	virtual void Randomize() override;
	virtual std::string ToString() const override;

	static std::string Decode(const ScheduleChromosome& scheduleChrom, const std::vector<std::string>& employeeNames, int dayCount);
};

