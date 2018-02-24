#pragma once
#include <string>
#include <vector>

class ScheduleExporter
{
public:
	ScheduleExporter();
	~ScheduleExporter();
	static void Export(const std::string& filename, const std::vector<bool>& schedule, const std::vector<std::string>& employeeNames, int workdayCount, char separator = ';');
};

