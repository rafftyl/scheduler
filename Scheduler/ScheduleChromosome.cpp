#include "stdafx.h"
#include "ScheduleChromosome.h"
#include <sstream>
#include <iomanip>

ScheduleChromosome::ScheduleChromosome(int dataSize, float start, float end, float granularity, std::mt19937& rng) :
	dataSize(dataSize), startTime(start), endTime(end), granularity(granularity), rng(rng)
{
}

ScheduleChromosome::~ScheduleChromosome()
{
}

void ScheduleChromosome::Mutate()
{
	std::uniform_int_distribution<int> dist(0, 1);
	for(size_t i = 0; i < data.size(); ++i)
	{
		if (dist(rng) == 0)
		{
			data[i] += granularity;
		}
		else
		{
			data[i] -= granularity;
		}
		if (data[i] < startTime)
		{
			data[i] = startTime;
		}
		else if (data[i] > endTime)
		{
			data[i] = endTime;
		}
	}
}

std::unique_ptr<Chromosome> ScheduleChromosome::Crossover(const Chromosome& other) const
{
	auto& typedOther = static_cast<const ScheduleChromosome&>(other);
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> dist(0, 1);
	auto result = std::make_unique<ScheduleChromosome>(dataSize, startTime, endTime, granularity, rng);
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
	std::uniform_real_distribution<float> dist(startTime, endTime);
	for (size_t i = 0; i < data.size(); ++i)
	{
		data[i] = dist(rng);
	}
}

std::string ScheduleChromosome::ToString() const
{
	std::stringstream ss;
	for (float dat : data)
	{
		ss << dat << ", ";
	}
	return ss.str();
}

std::string ScheduleChromosome::Decode(const ScheduleChromosome& scheduleChrom, const std::vector<std::string>& employeeNames, int dayCount)
{
	std::stringstream ss;
	for (size_t employeeInd = 0; employeeInd < employeeNames.size(); ++employeeInd)
	{
		ss << employeeNames[employeeInd] << ": " << std::endl;
		for (int day = 0; day < dayCount; ++day)
		{
			int startIndex = 2 * (day * employeeNames.size() + employeeInd);
			float minHour = scheduleChrom.data[startIndex];
			float maxHour = scheduleChrom.data[startIndex + 1];
			if (maxHour - minHour > scheduleChrom.granularity)
			{
				ss << "Day " << day << ": " << std::setprecision(2) << minHour << " - " << maxHour << std::endl;
			}
			else
			{
				ss << "Day " << day << ": " << "-" << std::endl;
			}
		}
		ss << std::endl;
	}

	return ss.str();
}
