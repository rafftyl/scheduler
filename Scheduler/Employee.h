#pragma once
#include <set>

class Employee
{
public:
	std::set<int> features;
	Employee(const std::set<int>& features);
	~Employee();
};

