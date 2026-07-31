#include "WorldGraph.h"

#include <stdexcept>

void WorldGraph::addLocation(const std::string& location)
{
	adjacencyList.try_emplace(location);
}

void WorldGraph::connectLocations(
	const std::string& first,
	const std::string& second)
{
	addLocation(first);
	addLocation(second);

	adjacencyList[first].push_back(second);
	adjacencyList[second].push_back(first);
}

bool WorldGraph::areConnected(
	const std::string& first,
	const std::string& second) const
{
	const auto iterator = adjacencyList.find(first);

	if (iterator == adjacencyList.end())
	{
		return false;
	}

	for (const std::string& location : iterator->second)
	{
		if (location == second)
		{
			return true;
		}
	}

	return false;
}

const std::vector<std::string>&
WorldGraph::getConnectedLocations(
	const std::string& location) const
{
	const auto iterator = adjacencyList.find(location);

	if (iterator == adjacencyList.end())
	{
		throw std::out_of_range(
			"World location does not exist."
		);
	}

	return iterator->second;
}