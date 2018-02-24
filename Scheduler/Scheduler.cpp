// Scheduler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ScheduleChromosome.h"
#include "../Genetic/Population.h"
#include "ScheduleExporter.h"
#include <iostream>
#include <sstream>
#include <set>

const std::vector<std::string> employeeNames{ "Anna Dlugolecka", "Tatiana", "Irina", "Jana", "Alona" };
const std::vector<int> maxDaysPerEmployee{ 4, 5, 5, 5, 5 };
const std::vector<int> staffNeededPerDay{ 3,3,3,4,4,4,4 };
const std::vector<std::set<int>> employeeSkills{ {0}, {0}, {0, 1}, {0}, {0, 1} };
const std::vector<std::set<int>> daysOff{ {}, {}, {}, {}, {} };

const int workingDayCount = 7;
const std::set<int> skillsetNeeded{ 0, 1 };
const int employeeCount = static_cast<int>(employeeNames.size());
const int initDay = 0; //monday

//genetic algorithm params
const int iterationCount = 200;
const int populationSize = 300;
const int tournamentSize = 7;
const float mutationProb = 0.05f;
const float crossoverProb = 0.99f;

int main()
{
	FitnessFunction fitness =
		[&](const Chromosome& chrom)
	{
		auto& typedChrom = static_cast<const ScheduleChromosome&>(chrom);
		int daysCovered = 0;
		int spuriousMandays = 0;	
		int skillsLacking = 0;

		std::set<int> skillsNotFound = skillsetNeeded;
		std::vector<int> daysWorked(employeeCount, 0);
		std::vector<int> daysOffViolated(employeeCount, 0);
		std::vector<bool> workedPreviousDay(employeeCount, false);
		std::vector<int> streaks(employeeCount, 0);
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
					for (const int& skillId : employeeSkills[emp])
					{
						skillsNotFound.erase(skillId);
					}
					if (daysOff[emp].find(day) != daysOff[emp].end())
					{
						++daysOffViolated[emp];
					}
					if (workedPreviousDay[emp])
					{
						++streaks[emp];
					}
					else
					{
						workedPreviousDay[emp] = true;
					}
				}
				else
				{
					workedPreviousDay[emp] = false;
				}
			}			

			skillsLacking += skillsNotFound.size();
			int offsetDay = day + initDay;
			int dayOfAWeek = offsetDay % 7;
			int needed = staffNeededPerDay[dayOfAWeek];
			if (employees >= needed)
			{
				++daysCovered;	
				if (employees > needed)
				{
					spuriousMandays += employees - needed;
				}
			}
		}

		std::vector<float> result;
		result.reserve(3 + employeeCount);
		result.push_back(static_cast<float>(daysCovered));
		result.push_back(static_cast<float>(-spuriousMandays));
		result.push_back(static_cast<float>(-skillsLacking));
		for (int emp = 0; emp < employeeCount; ++emp)
		{
			int dif = maxDaysPerEmployee[emp] - daysWorked[emp];
			result.push_back(-dif*dif);
			result.push_back(-daysOffViolated[emp]);
			result.push_back(std::min(0, 2 - streaks[emp]));
		}
	
		return result;
	};

	int dataSize = workingDayCount * employeeCount;
	ScheduleChromosome chrom(dataSize);
	Population pop(populationSize, tournamentSize, chrom, fitness, mutationProb, crossoverProb);
	
	for (int i = 0; i < iterationCount; ++i)
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
		std::cout << "Schedule with " << chroms[chromIndex]->GetOrComputeFitness(fitness)[0] << " days fully covered; "<<
			-chroms[chromIndex]->GetOrComputeFitness(fitness)[1] << " spurious mandays; " <<
			-chroms[chromIndex]->GetOrComputeFitness(fitness)[2] << " days without full skillset:" << std::endl;
		std::cout << ScheduleChromosome::Decode(*static_cast<const ScheduleChromosome*>(chroms[chromIndex]), employeeNames, workingDayCount);
		std::string command;
		std::cin >> command;
		std::transform(command.begin(), command.end(), command.begin(), tolower);
		if (command == "export")
		{
			std::cout << "Enter file name: " << std::endl;
			std::cin >> command;
			ScheduleExporter::Export(command, static_cast<const ScheduleChromosome*>(chroms[chromIndex])->data, employeeNames, workingDayCount);
		}
		else
		{
			chromIndex++;
		}		
	}

	std::cout << std::endl << "Finished" << std::endl;
	getchar();
}

