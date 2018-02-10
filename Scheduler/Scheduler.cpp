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
	std::vector<int> maxDaysPerEmployee{ 18, 18, 18, 18, 16};
	int workingDayCount = 31;
	int employeeCount = employeeNames.size();
	int employeesNeeded = 4;
	int initDay = 3;

	FitnessFunction fitness =
		[&](const Chromosome& chrom)
	{
		auto& typedChrom = static_cast<const ScheduleChromosome&>(chrom);
		int daysCovered = 0;
		int freeDays = 0;
		int spuriousMandays = 0;
		int cost = 0;
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
					++cost;
					++daysWorked[emp];
					if (daysWorked[emp] > maxDaysPerEmployee[emp])
					{
						cost += daysWorked[emp] - maxDaysPerEmployee[emp];
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
					++freeDays;
				}
			}			

			int offsetDay = day + initDay;
			int needed = (offsetDay % 7 == 0 || offsetDay % 7 == 1) ? employeesNeeded - 1 : employeesNeeded;
			if (employees >= employeesNeeded)
			{
				++daysCovered;
				if (employees > employeesNeeded)
				{
					spuriousMandays += employees - employeesNeeded;
				}
			}
		}
		if (daysCovered < workingDayCount)
		{
			freeDays = 0;
			streakDayPenalty = 1e20;
		}
	
		return std::vector<float>{static_cast<float>(daysCovered), static_cast<float>(-spuriousMandays), static_cast<float>(freeDays), static_cast<float>(-cost), -streakDayPenalty};
	};

	int dataSize = workingDayCount * employeeCount;

	std::random_device rd;
	std::mt19937 rng(rd());

	ScheduleChromosome chrom(dataSize, rng);
	Population pop(500, 7, chrom, fitness);
	for (int i = 0; i < 200; ++i)
	{
		pop.Breed();
	}
	auto chroms = pop.GetBestChromosomes();
	size_t chromIndex = 0;
	while (chromIndex < chroms.size())
	{
		system("cls");
		std::cout << "Schedule with " << chroms[chromIndex]->GetOrComputeFitness(fitness)[0] << " days fully covered and " << chroms[chromIndex]->GetOrComputeFitness(fitness)[1] << " spurious mandays: " << std::endl;
		std::cout << ScheduleChromosome::Decode(*static_cast<const ScheduleChromosome*>(chroms[chromIndex]), employeeNames, workingDayCount);
		getchar();
		chromIndex++;
	}

	std::cout << std::endl << "Finished" << std::endl;
	getchar();
}

