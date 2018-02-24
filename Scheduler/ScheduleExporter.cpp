#include "stdafx.h"
#include "ScheduleExporter.h"
#include <fstream>
#include <iostream>

ScheduleExporter::ScheduleExporter()
{
}


ScheduleExporter::~ScheduleExporter()
{
}

void ScheduleExporter::Export(const std::string& filename, const std::vector<bool>& schedule, const std::vector<std::string>& employeeNames, int workdayCount, char separator)
{
	try
	{
		std::ofstream file(filename);
		for (size_t employeeInd = 0; employeeInd < employeeNames.size(); ++employeeInd)
		{
			file << employeeNames[employeeInd] << separator;
			for (int day = 0; day < workdayCount; ++day)
			{
				size_t dataIndex = day * employeeNames.size() + employeeInd;
				if (schedule[dataIndex])
				{
					file << "10:00" << separator << "21:00" << separator << "11:00:00" << separator;
				}
				else
				{
					file << "dw" << separator << "" << separator << "00:00:00" << separator;
				}
			}
			file << std::endl;
		}
		file.close();
		std::cout << "Successfuly exported file " << filename << std::endl;
	}
	catch(std::exception)
	{
		std::cout << "Could not create file " << filename << std::endl;
		return;
	}	
}
