#include "stdafx.h"
#include "ScheduleChromosome.h"
#include <sstream>
#include <iomanip>

ScheduleChromosome::ScheduleChromosome(int dataSize) : dataSize(dataSize)
{
}

ScheduleChromosome::~ScheduleChromosome()
{
}

void ScheduleChromosome::Mutate()
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (Random::Range(0, 1) == 0)
		{
			data[i] = !data[i];
		}
	}
}

std::unique_ptr<Chromosome> ScheduleChromosome::Crossover(const Chromosome& other) const
{
	auto& typedOther = static_cast<const ScheduleChromosome&>(other);
	auto result = std::make_unique<ScheduleChromosome>(dataSize);
	result->data.resize(dataSize);
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (Random::Range(0,1) == 0)
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
	for (size_t i = 0; i < data.size(); ++i)
	{
		data[i] = (Random::Range(0,1) == 0);
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
			size_t dataIndex = day * employeeNames.size() + employeeInd;
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
