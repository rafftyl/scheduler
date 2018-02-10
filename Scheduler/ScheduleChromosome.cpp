#include "stdafx.h"
#include "ScheduleChromosome.h"
#include <sstream>
#include <iomanip>

ScheduleChromosome::ScheduleChromosome(int dataSize, std::mt19937& rng) :
	dataSize(dataSize), rng(rng)
{
}

ScheduleChromosome::~ScheduleChromosome()
{
}

void ScheduleChromosome::Mutate()
{
	std::uniform_int_distribution<int> dist(0, data.size());
	int index = dist(rng);
	data[index] = !data[index];
}

std::unique_ptr<Chromosome> ScheduleChromosome::Crossover(const Chromosome& other) const
{
	auto& typedOther = static_cast<const ScheduleChromosome&>(other);
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> dist(0, 1);
	auto result = std::make_unique<ScheduleChromosome>(dataSize, rng);
	result->data.resize(dataSize);
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

std::unique_ptr<Chromosome> ScheduleChromosome::Clone() const
{
	return std::make_unique<ScheduleChromosome>(*this);
}

void ScheduleChromosome::Randomize()
{
	data.resize(dataSize);
	std::uniform_int_distribution<int> dist(0, 1);
	for (size_t i = 0; i < data.size(); ++i)
	{
		data[i] = (dist(rng) == 0);
	}
}

std::string ScheduleChromosome::ToString() const
{
	std::stringstream ss;
	for (float dat : data)
	{
		ss << (dat ? true : false) << ", ";
	}
	return ss.str();
}

std::string ScheduleChromosome::Decode(const ScheduleChromosome& scheduleChrom, const std::vector<std::string>& employeeNames, int dayCount)
{
	std::stringstream ss;
	for (size_t employeeInd = 0; employeeInd < employeeNames.size(); ++employeeInd)
	{
		int workdays = 0;
		ss << employeeNames[employeeInd] << ": " << std::endl;
		for (int day = 0; day < dayCount; ++day)
		{
			int dataIndex = day * employeeNames.size() + employeeInd;
			if (scheduleChrom.data[dataIndex])
			{
				workdays++;
				ss << "Day " << day + 1 << ": " << "10 - 21" << std::endl;
			}
			else
			{
				ss << "Day " << day + 1 << ": " << "-" << std::endl;
			}
		}
		ss << "Workdays: " << workdays << std::endl << std::endl;
	}

	return ss.str();
}
