// Scheduler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ScheduleChromosome.h"
#include "../Genetic/Population.h"
#include <iostream>
#include <sstream>

int main()
{
	std::vector<std::string> employeeNames{ "emp_1", "emp_2", "emp_3", "emp_4", "emp_5"};
	std::vector<int> maxDaysPerEmployee{ 6, 6, 6, 6, 4};
	std::vector<int> staffNeededPerDay{ 3,3,4,4,4,4,4 };
	int workingDayCount = 7;
	int employeeCount = static_cast<int>(employeeNames.size());
	int initDay = 0;

	FitnessFunction fitness =
		[&](const Chromosome& chrom)
	{
		auto& typedChrom = static_cast<const ScheduleChromosome&>(chrom);
		int daysCovered = 0;
		int spuriousMandays = 0;
		int overtime = 0;
		float streakDayPenalty = 0;
		std::vector<int> streakDays(employeeCount, 0);
		std::vector<int> daysWorked(employeeCount, 0);
		std::vector<bool> workedPrevDay(employeeCount, false);

		for (int day = 0; day < workingDayCount; ++day)
		{
			int employees = 0;
			for (int emp = 0; emp < employeeCount; ++emp)
			{
				int dataIndex = day * employeeCount + emp;
				if (typedChrom.data[dataIndex])
				{
					++employees;
					++daysWorked[emp];
					if (daysWorked[emp] > maxDaysPerEmployee[emp])
					{
						overtime += daysWorked[emp] - maxDaysPerEmployee[emp];
					}
					if (workedPrevDay[emp])
					{
						++streakDays[emp];
						streakDayPenalty += streakDays[emp];
					}
					else
					{
						workedPrevDay[emp] = true;
					}
				}
				else
				{
					workedPrevDay[emp] = false;
					streakDays[emp] = 0;
				}
			}			

			int offsetDay = day + initDay;
			int dayOfAWeek = offsetDay % 7;
			int needed = staffNeededPerDay[dayOfAWeek];
			if (employees == needed)
			{
				++daysCovered;				
			}
			else if (employees > needed)
			{
				spuriousMandays += employees - needed;
			}
		}
	
		return std::vector<float>{static_cast<float>(daysCovered), static_cast<float>(-spuriousMandays), static_cast<float>(-overtime), -streakDayPenalty};
	};

	int dataSize = workingDayCount * employeeCount;
	ScheduleChromosome chrom(dataSize);
	Population pop(50, 7, chrom, fitness);
	
	for (int i = 0; i < 10; ++i)
	{
		pop.Breed();
	}
	pop.StopComputation();

	auto chroms = pop.GetBestChromosomes();
	std::sort(chroms.begin(), chroms.end(),
	[&](const Chromosome* a, const Chromosome* b) -> bool
	{
		return a->GetOrComputeFitness(fitness)[0] > b->GetOrComputeFitness(fitness)[0];
	});

	size_t chromIndex = 0;
	while (chromIndex < chroms.size())
	{
		system("cls");
		std::cout << "Schedule with " << chroms[chromIndex]->GetOrComputeFitness(fitness)[0] << " days fully covered:"<< std::endl;
		std::cout << ScheduleChromosome::Decode(*static_cast<const ScheduleChromosome*>(chroms[chromIndex]), employeeNames, workingDayCount);
		getchar();
		chromIndex++;
	}

	std::cout << std::endl << "Finished" << std::endl;
	getchar();
}

