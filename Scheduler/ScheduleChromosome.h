#pragma once
#include "../Genetic/Chromosome.h"
#include <vector>
#include <random>

class ScheduleChromosome : public Chromosome
{	
public:
	int dataSize;
	float startTime;
	float endTime;
	float granularity;
	std::vector<float> data;
	std::mt19937& rng;

	ScheduleChromosome(int dataSize, float start, float end, float granularity, std::mt19937& rng);
	ScheduleChromosome(const ScheduleChromosome& other) = default;
	~ScheduleChromosome();

	virtual void Mutate() override;
	virtual std::unique_ptr<Chromosome> Crossover(const Chromosome& other) const override;
	virtual std::unique_ptr<Chromosome> Clone() const override;
	virtual void Randomize() override;
	virtual std::string ToString() const override;

	static std::string Decode(const ScheduleChromosome& scheduleChrom, const std::vector<std::string>& employeeNames, int dayCount);
};

