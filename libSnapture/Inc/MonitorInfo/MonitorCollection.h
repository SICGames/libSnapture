#pragma once
#include <vector>
#include "Monitor.h"

class MonitorCollection
{
public:
	MonitorCollection() {

	}
	~MonitorCollection() 
	{
		if(Monitors.size() > 0)
			Monitors.clear();
	}
	void Add(Monitor monitor) {
		Monitors.push_back(monitor);
	}
	const Monitor Get(size_t index) {
		return Monitors.at(index);
	}
	size_t Count() {
		return Monitors.size();
	}
	void Clear() {
		Monitors.clear();
	}
private:
	std::vector<Monitor> Monitors;
};