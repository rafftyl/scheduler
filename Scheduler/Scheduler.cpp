// Scheduler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ScheduleChromosome.h"
#include "../Genetic/Population.h"
#include <iostream>
#include <sstream>

int main()
{
	float startTime = 10;
	float endTime = 21;
	float granularity = 1;
	int workingDayCount = 7;
	int employeeCount = 2;
	int employeesNeeded = 4;

	FitnessFunction fitness =
		[&](const Chromosome& chrom)
	{
		auto& typedChrom = static_cast<const ScheduleChromosome&>(chrom);
		float sum = 0;
		int freeDays = 0;
		for (int emp = 0; emp < employeeCount; ++emp)
		{
			for (int day = 0; day < workingDayCount; ++day)
			{
				int startIndex = 2 * (day * employeeCount + emp);
				float minHour = typedChrom.data[startIndex];
				float maxHour = typedChrom.data[startIndex + 1];
				float workTime = maxHour - minHour;
				sum += workTime;
				if (workTime < typedChrom.granularity)
				{
					freeDays++;
				}
			}
		}
		return std::vector<float>{sum, static_cast<float>(freeDays)};
	};
	
	std::vector<std::string> employeeNames{ "emp_1", "emp_2" };

	int dataSize = workingDayCount * employeeCount * 2;

	std::random_device rd;
	std::mt19937 rng(rd());

	ScheduleChromosome chrom(dataSize, startTime, endTime, granularity, rng);
	Population pop(250, 7, chrom, fitness);
	for (int i = 0; i < 500; ++i)
	{
		pop.Breed();
	}
	std::cout << ScheduleChromosome::Decode(*static_cast<const ScheduleChromosome*>(pop.GetBestChromosome()), employeeNames, workingDayCount);
	getchar();
}

